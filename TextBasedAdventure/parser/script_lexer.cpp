// File Author: Isaiah Hoffman
// File Created: January 1, 2018
#include <string>
#include <map>
#include <iostream>
#include "script_lexer.hpp"
#include "script_globals.hpp"

namespace hoffman::isaiah {
	namespace parser {
		std::map<std::string, ScriptCommands> commandTable {
			// These first two are dummy commands that will not actually be recognized.
			// They exist in case I end up needing to map the symbolic names back
			// into the textual values.
			{"_Unknown", ScriptCommands::Unknown}, {"_Number", ScriptCommands::Number},
			{"Begin_Script", ScriptCommands::Begin_Script},
			{"End_Script", ScriptCommands::End_Script}, {"State", ScriptCommands::State},
			{"Comment", ScriptCommands::Comment}, {"End_Comment", ScriptCommands::End_Comment},
			{"Rem", ScriptCommands::Comment}, {"End_Rem", ScriptCommands::End_Comment},
			{"Show_Text", ScriptCommands::Show_Text}, {"Set_Flag", ScriptCommands::Set_Flag},
			{"Set_Flag_Indirect", ScriptCommands::Set_Flag_Indirect}, {"Set_Flag_I", ScriptCommands::Set_Flag_Indirect},
			{"Retrieve_Flag", ScriptCommands::Retrieve_Flag}, {"Set_Flag_To_Buffer", ScriptCommands::Retrieve_Flag},
			{"Store_Flag", ScriptCommands::Store_Flag}, {"Set_Buffer_To_Flag", ScriptCommands::Store_Flag},
			{"Increment_Flag", ScriptCommands::Increment_Flag}, {"Change_Flag", ScriptCommands::Increment_Flag},
			{"Add_Flags", ScriptCommands::Add_Flags}, {"Subtract_Flags", ScriptCommands::Subtract_Flags},
			{"Test_Flags", ScriptCommands::Test_Flags}, {"Jump_If_Zero", ScriptCommands::Jump_If_Zero},
			{"JZ", ScriptCommands::Jump_If_Zero}, {"Jump_If_Not_Zero", ScriptCommands::Jump_If_Not_Zero},
			{"JNZ", ScriptCommands::Jump_If_Not_Zero}, {"Jump_If_Positive", ScriptCommands::Jump_If_Positive},
			{"JGT", ScriptCommands::Jump_If_Positive}, {"Jump_If_Negative", ScriptCommands::Jump_If_Negative},
			{"JLT", ScriptCommands::Jump_If_Negative}, {"Reset_Buffer", ScriptCommands::Reset_Buffer},
			{"Clear_Buffer", ScriptCommands::Reset_Buffer}, {"Set_Buffer_To_Zero", ScriptCommands::Reset_Buffer},
			{"Add_To_Buffer", ScriptCommands::Add_To_Buffer}, {"Increment_Buffer", ScriptCommands::Add_To_Buffer},
			{"Test_Buffer", ScriptCommands::Test_Buffer}, {"Subtract_From_Buffer", ScriptCommands::Test_Buffer},
			{"Jump_To_State", ScriptCommands::Jump_To_State}, {"Go_To_State", ScriptCommands::Jump_To_State},
			{"Jump", ScriptCommands::Jump_To_State}, {"Change_Health", ScriptCommands::Change_Health},
			{"Kill_Player", ScriptCommands::Kill_Player}, {"Get_Input", ScriptCommands::Get_Input}
		};

		ScriptLexer::ScriptLexer(std::istream& script, std::string file) :
			script_text {""},
			lookahead {'\0'},
			token {""},
			ttoken {ScriptTokenTypes::Unknown},
			ctoken {ScriptCommands::Unknown},
			my_location {0U},
			file_name {file},
			line_number {0} {
			std::string buffer {};
			while (std::getline(script, buffer, '\n')) {
				this->script_text += buffer + '\n';
			}
			// Get rid of final newline and append end token
			this->script_text.pop_back();
			this->script_text.append("\0");
			// Set lookahead
			this->lookahead = &this->script_text.at(0);
			// Skip leading whitespace
			this->skipWhite();
		}

		bool ScriptLexer::getNext() {
			// Make sure we do not go past the end of the file.
			if (this->getLookahead() != '\0') {
				// Keep track of start location
				const char* startChar = this->lookahead;
				// Determine token type
				if ((this->getLookahead() >= '0' && this->getLookahead() <= '9') || this->getLookahead() == '-') {
					this->ttoken = ScriptTokenTypes::Number;
					this->ctoken = ScriptCommands::Number;
				}
				else {
					this->ttoken = ScriptTokenTypes::Unknown;
					this->ctoken = ScriptCommands::Unknown;
				}
				if (this->ttoken == ScriptTokenTypes::Number) {
					// Scan a number (and handle leading minus sign)
					if (this->getLookahead() == '-') {
						this->token += this->getLookahead();
						++this->lookahead;
					}
					while (this->getLookahead() >= '0' && this->getLookahead() <= '9') {
						this->token += this->getLookahead();
						++this->lookahead;
					}
					// Validate
					if (this->getLookahead() != ' ' && this->getLookahead() != '\t'
						&& this->getLookahead() != '\r' && this->getLookahead() != '\n'
						&& this->getLookahead() != '\0') {
						throw ScriptLexicalError {"Invalid numerical literal.", this->getFileName(), this->getLineNumber()};
					}
				}
				else {
					// Scan a non-number
					while ((this->getLookahead() >= 'a' && this->getLookahead() <= 'z') ||
						(this->getLookahead() >= 'A' && this->getLookahead() <= 'Z') || this->getLookahead() == '_') {
						this->token += *lookahead;
						++lookahead;
					}
					// Validate
					if (this->getLookahead() != ' ' && this->getLookahead() != '\t'
						&& this->getLookahead() != '\r' && this->getLookahead() != '\n'
						&& this->getLookahead() != '\0') {
						throw ScriptLexicalError {"Invalid command token.", this->getFileName(), this->getLineNumber()};
					}
				}
				// Update location
				this->my_location = this->lookahead - startChar;
				this->skipWhite();
			}
			if (this->getLookahead() == '\0') {
				return true;
			}
			return false;
		}

		void ScriptLexer::scan() noexcept {
			if (this->ttoken == ScriptTokenTypes::Unknown) {
				auto loc = parser::commandTable.find(this->token);
				if (loc != parser::commandTable.end()) {
					this->ctoken = loc->second;
				}
			}
		}

		void ScriptLexer::skipWhite() noexcept {
			// This is safe because the text of the script is guaranteed
			// to be terminated by a null character ('\0').
			while (this->getLookahead() == '\t' || this->getLookahead() == '\n'
				|| this->getLookahead() == '\r' || this->getLookahead() == ' ') {
				char oldLook = this->getLookahead();
				++this->lookahead;
				++this->my_location;
				// Increment line numbers as appropriate
				// (Lines can end in \r, \n, or \r\n depending on the OS.
				// This is probably handled for me already if I use text mode,
				// but if I open a stream in binary mode, this matters.)
				if ((oldLook == '\r' || oldLook == '\n')
					&& !(oldLook == '\r' && this->getLookahead() == '\n')) {
					++this->line_number;
				}
			}
		}
	}
}