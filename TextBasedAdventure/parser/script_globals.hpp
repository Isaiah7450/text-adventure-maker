#ifndef TEXT_ADVENTURE_PARSER_SCRIPT_GLOBALS_HPP_INCLUDED
#define TEXT_ADVENTURE_PARSER_SCRIPT_GLOBALS_HPP_INCLUDED
// File Author: Isaiah Hoffman
// File Created: January 2, 2018
#include <string>
#include <map>
#include <stdexcept>
namespace hoffman::isaiah {
	namespace parser {
		/// <summary>Represents the commands recognized by the script parser.</summary>
		enum class ScriptCommands {
			Unknown, Begin_Script, End_Script, State, Comment, End_Comment, Show_Text,
			Set_Flag, Set_Flag_Indirect, Retrieve_Flag, Store_Flag,
			Increment_Flag, Add_Flags, Subtract_Flags, Test_Flags,
			Jump_If_Zero, Jump_If_Not_Zero, Jump_If_Positive, Jump_If_Negative,
			Reset_Buffer, Add_To_Buffer, Test_Buffer, Jump_To_State,
			End_Scenario, Change_Health, Kill_Player, Get_Input
		};

		/// <summary>Maps the textual representation of a command to its numerical representation.</summary>
		// (Defined in script_lexer.cpp)
		extern std::map<std::string, ScriptCommands> commandTable;

		// Forward declarations
		class ScriptLexer;
		class ScriptParser;

		// Exception classes
		class ScriptError : public std::runtime_error {
		public:
			ScriptError(std::string message, std::string file, int line) :
				runtime_error {message},
				my_message {"In file " + file + " on line " + std::to_string(line) + ": " + message + "\n"},
				file_name {file},
				line_number {line} {
			}
			const char* what() const override {
				return this->my_message.c_str();
			}
		private:
			/// <summary>This variable represents the message to display when calling the what() function.</summary>
			std::string my_message;
			/// <summary>This variable represents the file that the error occurred in.</summary>
			std::string file_name;
			/// <summary>This variable represents the line number that the error was encountered on.</param>
			int line_number;
		};

		/// <summary>Represents a script error that occurred during parsing.</summary>
		class ScriptParseError : public ScriptError {
		public:
			ScriptParseError(std::string message, std::string file, int line) :
				ScriptError {message, file, line} {
			}
		};
		/// <summary>Represents a script error that occurred during lexical scanning.</summary>
		class ScriptLexicalError : public ScriptError {
		public:
			ScriptLexicalError(std::string message, std::string file, int line) :
				ScriptError {message, file, line} {
			}
		};
	}
}

#endif // TEXT_ADVENTURE_PARSER_SCRIPT_GLOBALS_HPP_INCLUDED