#ifndef TEXT_ADVENTURE_PARSER_SCRIPT_LEXER_HPP_INCLUDED
#define TEXT_ADVENTURE_PARSER_SCRIPT_LEXER_HPP_INCLUDED
// File Author: Isaiah Hoffman
// File Created: December 30, 2017
#include <string>
#include <iosfwd>
#include "script_globals.hpp"
namespace hoffman::isaiah {
	namespace parser {
		/// <summary>Represents an object that performs lexical scanning and splits an input
		/// string into individual tokens.</summary>
		class ScriptLexer {
		public:
			/// <param name="script">A stream with the script to load.</param>
			/// <param name="file">The name of the file being scanned. (Used for internal purposes.)</param>
			ScriptLexer(std::istream& script, std::string file);
			/// <param name="script">The script text to load.</param>
			ScriptLexer(std::string script) :
				script_text {script + '\0'},
				lookahead {&script.at(0)},
				token {""},
				ttoken {ScriptTokenTypes::Unknown},
				ctoken {ScriptCommands::Unknown},
				my_location {0U},
				file_name {"(Internal)"},
				line_number {1} {
				this->skipWhite();
			}
			/// <summary>Obtains the next token.</summary>
			/// <returns>True if the end of the script has been reached.</returns>
			bool getNext();
			/// <summary>Scans the current token for commands.</summary>
			void scan() noexcept;
			/// <summary>Checks if the current token matches the provided command.</summary>
			/// <param name="command">The command to test for.</param>
			void matchCommand(ScriptCommands command) {
				static const std::map<ScriptCommands, std::string> commandStrings {
					{ScriptCommands::Number, "a number"},
					{ScriptCommands::Begin_Script, "Begin_Script"}, {ScriptCommands::End_Script, "End_Script"},
					{ScriptCommands::State, "State"}, {ScriptCommands::Show_Text, "Show_Text"},
					{ScriptCommands::Set_Flag, "Set_Flag"}, {ScriptCommands::Set_Flag_Indirect, "Copy_Flag"},
					{ScriptCommands::Retrieve_Flag, "Retrieve_Flag"}, {ScriptCommands::Store_Flag, "Store_Flag"},
					{ScriptCommands::Increment_Flag, "Change_Flag"}, {ScriptCommands::Add_Flags, "Add_Flags"},
					{ScriptCommands::Subtract_Flags, "Subtract_Flags"}, {ScriptCommands::Test_Flags, "Test_Flags"},
					{ScriptCommands::Jump_If_Zero, "Jump_If_Zero"}, {ScriptCommands::Jump_If_Not_Zero, "Jump_If_Not_Zero"},
					{ScriptCommands::Jump_If_Positive, "Jump_If_Positive"}, {ScriptCommands::Jump_If_Negative, "Jump_If_Negative"},
					{ScriptCommands::Jump_On_Buffer, "Jump_On_Buffer"},
					{ScriptCommands::Reset_Buffer, "Reset_Buffer"}, {ScriptCommands::Add_To_Buffer, "Add_To_Buffer"},
					{ScriptCommands::Test_Buffer, "Subtract_From_Buffer"}, {ScriptCommands::Jump_To_State, "Jump_To_State"},
					{ScriptCommands::End_Scenario, "End_Scenario"}, {ScriptCommands::Change_Health, "Change_Health"},
					{ScriptCommands::Kill_Player, "Kill_Player"}, {ScriptCommands::Get_Input, "Get_Input"},
					{ScriptCommands::Pause, "Pause"}, {ScriptCommands::Store_Health, "Store_Health"},
					{ScriptCommands::Clear_String, "Clear_String"}, {ScriptCommands::Append_String, "Append_String"},
					{ScriptCommands::Append_String_Indirect, "Append_String_In_Flag"}, {ScriptCommands::Append_Number, "Append_Buffer"},
					{ScriptCommands::Append_Space, "Append_Space"}, {ScriptCommands::Display_String, "Display_String"},
					{ScriptCommands::Call_State, "Call_State"}, {ScriptCommands::Return, "Return_From_Call"}
				};
				if (this->getCommandToken() != command) {
					// This is a parsing error; the provided token is acceptable
					throw ScriptParseError {"Expected `" + commandStrings.at(command) + "`, but received `"
						+ this->getToken() + "`.", ErrorSeverity::Error, this->getFileName(), this->getLineNumber()};
				}
			}
			/// <returns>The value of the lookahead token.</returns>
			char getLookahead() const noexcept {
				return *this->lookahead;
			}
			/// <returns>The last scanned token.</returns>
			std::string getToken() const noexcept {
				return this->token;
			}
			/// <returns>The last scanned token represented as a numerical command.</returns>
			ScriptCommands getCommandToken() const noexcept {
				return this->ctoken;
			}
			/// <returns>The location that the lexer is at in the script.</returns>
			size_t getLocation() const noexcept {
				return this->my_location;
			}
			/// <returns>The file being scanned by the lexer.</returns>
			std::string getFileName() const noexcept {
				return this->file_name;
			}
			/// <returns>The current line number of the lexer.</returns>
			int getLineNumber() const noexcept {
				return this->line_number;
			}
			/// <param name="loc">The new location to jump to in the script.</param>
			/// <param name="line">The line number of the new location.</param>
			void setLocation(size_t loc, int line) {
				this->my_location = loc;
				this->lookahead = &this->script_text.at(loc);
				this->line_number = line;
				this->skipWhite();
			}
			/// <summary>Skips comments in the input text.</summary>
			void skipComments();
		protected:
			/// <summary>Skips whitespace in the input text.</summary>
			void skipWhite() noexcept;
		private:
			/// <summary>This variable stores the entire text given to the lexical scanner.</summary>
			std::string script_text;
			/// <summary>This variable stores the first character of the next token
			/// to be read by the lexical scanner. Actually, it points to the next
			/// character to be read.</summary>
			char* lookahead;
			/// <summary>This variable stores the last token read by the lexical scanner.</summary>
			std::string token;
			/// <summary>This variable stores the type of the last token read by the lexical scanner.</summary>
			ScriptTokenTypes ttoken;
			/// <summary>This variable stores the command represented by the last read token.</summary>
			ScriptCommands ctoken;
			/// <summary>This variable keeps track of where the lexical scanner is in the script text.</summary>
			size_t my_location;
			/// <summary>This variable keeps track of the file (if relevant) being scanned.</summary>
			std::string file_name;
			/// <summary>This variable keeps track of the current line the lexical scanner is at in the script text.</summary>
			int line_number;
		};
	}
}

#endif // TEXT_ADVENTURE_PARSER_SCRIPT_LEXER_HPP_INCLUDED