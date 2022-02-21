#ifndef TEXT_ADVENTURE_PARSER_SCRIPT_GLOBALS_HPP_INCLUDED
#define TEXT_ADVENTURE_PARSER_SCRIPT_GLOBALS_HPP_INCLUDED
// File Author: Isaiah Hoffman
// File Created: January 2, 2018
#include <string>
#include <map>
#include <stdexcept>
namespace hoffman_isaiah {
	namespace parser {
		/// <summary>Represents the various types of tokens recognized by the lexical scanner.</summary>
		enum class ScriptTokenTypes {
			Unknown, Command, Number
		};

		/// <summary>Represents the commands recognized by the script parser.</summary>
		enum class ScriptCommands {
			Unknown, Number, Begin_Script, End_Script, State, Comment, End_Comment, Show_Text,
			Set_Flag, Set_Flag_Indirect, Retrieve_Flag, Store_Flag,
			Increment_Flag, Add_Flags, Subtract_Flags, Test_Flags,
			Jump_If_Zero, Jump_If_Not_Zero, Jump_If_Positive, Jump_If_Negative, Jump_On_Buffer,
			Reset_Buffer, Add_To_Buffer, Test_Buffer, Jump_To_State,
			End_Scenario, Store_Health, Change_Health, Kill_Player, Get_Input, Pause,
			Clear_String, Append_String, Append_String_Indirect, Append_Number, Display_String,
			Append_Space, Call_State, Return
		};

		// (Defined in script_lexer.cpp)
		/// <summary>Maps the textual representation of a command to its numerical representation.</summary>
		extern std::map<std::string, ScriptCommands> commandTable;

		// Forward declarations
		class ScriptLexer;
		class ScriptParser;

		enum class ErrorSeverity {
			Info, Warning, Error, Internal
		};

		// Exception classes
		class ScriptError : public std::runtime_error {
		public:
			/// <param name="message">The error message to display.</param>
			ScriptError(std::string message, ErrorSeverity severity, std::string file, int line) :
				runtime_error {message},
				my_message {"In file " + file + " on line " + std::to_string(line)
				+ ":\n" + (severity == ErrorSeverity::Error ? "Error: "
					: (severity == ErrorSeverity::Warning ? "Warning: "
					: (severity == ErrorSeverity::Internal ? "Internal Error: " : "Note: ")))
				+ message + "\n"},
				my_severity {severity} {
			}
			/// <returns>The error message.</returns>
			const char* what() const override {
				return this->my_message.c_str();
			}
			/// <returns>The severity level of the error.</returns>
			ErrorSeverity getLevel() const noexcept {
				return this->my_severity;
			}
		private:
			/// <summary>This variable represents the message to display when calling the what() function.</summary>
			std::string my_message;
			/// <summary>This variable represents the severity of the error.</summary>
			ErrorSeverity my_severity;
		};

		/// <summary>Represents a script error that occurred during parsing.</summary>
		class ScriptParseError : public ScriptError {
		public:
			ScriptParseError(std::string message, ErrorSeverity severity, std::string file, int line) :
				ScriptError {message, severity, file, line} {
			}
		};
		/// <summary>Represents a script error that occurred during lexical scanning.</summary>
		class ScriptLexicalError : public ScriptError {
		public:
			ScriptLexicalError(std::string message, ErrorSeverity severity, std::string file, int line) :
				ScriptError {message, severity, file, line} {
			}
		};
	}
}

#endif // TEXT_ADVENTURE_PARSER_SCRIPT_GLOBALS_HPP_INCLUDED