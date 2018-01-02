// File Author: Isaiah Hoffman
// File Created: January 1, 2018
#include <string>
#include <map>
#include <iostream>
#include "script_lexer.hpp"

namespace hoffman::isaiah {
	namespace parser {
		std::map<std::string, ScriptCommands> commandTable {
			{"Unknown", ScriptCommands::Unknown}, {"Begin_Script", ScriptCommands::Begin_Script},
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

		ScriptLexer::ScriptLexer(std::istream& script) :
			script_text {""},
			lookahead {'\0'},
			token {""},
			ttoken {ScriptTokenTypes::Unknown},
			ctoken {ScriptCommands::Unknown},
			my_location {0U} {
			std::string buffer {};
			while (script >> buffer) {
				this->script_text += buffer + ' ';
			}
			// Get rid of final space and append end token
			this->script_text.pop_back();
			this->script_text.append("\0");
			// Set lookahead
			this->lookahead = &this->script_text.at(0);
		}

		void ScriptLexer::getNext() {
			// Make sure we do not go past the end of the file.
			if (*(this->lookahead) != '\0') {
				// Keep track of start location
				const char* startChar = this->lookahead;
				if ((*this->lookahead >= '0' && *this->lookahead <= '9') || *this->lookahead == '-') {
					this->ttoken = ScriptTokenTypes::Number;
				}
				else {
					this->ttoken = ScriptTokenTypes::Unknown;
				}
				if (this->ttoken == ScriptTokenTypes::Number) {
					if (*this->lookahead == '-') {
						this->token += *lookahead;
						++lookahead;
					}
					while (*this->lookahead >= '0' && *this->lookahead <= '9') {
						this->token += *lookahead;
						++lookahead;
					}
				}
				else {
					while ((*this->lookahead >= 'a' && *this->lookahead <= 'z') ||
						(*this->lookahead >= 'A' && *this->lookahead <= 'Z') || *this->lookahead == '_') {
						this->token += *lookahead;
						++lookahead;
					}
				}
				this->my_location = this->lookahead - startChar;
				this->skipWhite();
			}
		}

		void ScriptLexer::scan() noexcept {
			if (this->ttoken == ScriptTokenTypes::Unknown) {
				auto loc = parser::commandTable.find(this->token);
				if (loc == parser::commandTable.end()) {
					this->ctoken = ScriptCommands::Unknown;
				}
				else {
					this->ctoken = loc->second;
				}
			}
		}

		void ScriptLexer::skipWhite() noexcept {
			// This is safe because the text of the script is guaranteed
			// to be terminated by a null character ('\0').
			while (*this->lookahead == '\t' || *this->lookahead == '\n'
				|| *this->lookahead == '\r' || *this->lookahead == ' ') {
				++this->lookahead;
				++this->my_location;
			}
		}
	}
}