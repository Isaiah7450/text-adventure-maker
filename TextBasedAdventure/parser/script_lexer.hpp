#ifndef TEXT_ADVENTURE_PARSER_SCRIPT_LEXER_HPP_INCLUDED
#define TEXT_ADVENTURE_PARSER_SCRIPT_LEXER_HPP_INCLUDED
// File Author: Isaiah Hoffman
// File Created: December 30, 2017
#include <string>
#include <map>
#include <iosfwd>
namespace hoffman::isaiah {
	namespace parser {
		/// <summary>Represents the various types of tokens recognized by the lexical scanner.</summary>
		enum class ScriptTokenTypes {
			Unknown, Command, Number
		};

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
		extern std::map<std::string, ScriptCommands> commandTable;

		/// <summary>Represents an object that performs lexical scanning and splits an input
		/// string into individual tokens.</summary>
		class ScriptLexer {
		public:
			/// <param name="script">A stream with the script to load.</param>
			ScriptLexer(std::istream& script);
			/// <param name="script">The script text to load.</param>
			ScriptLexer(std::string script) :
				script_text {script + '\0'},
				lookahead {&script.at(0)},
				token {""},
				ttoken {ScriptTokenTypes::Unknown},
				ctoken {ScriptCommands::Unknown},
				my_location {0U} {
				this->skipWhite();
			}
			/// <summary>Obtains the next token.</summary>
			void getNext();
			/// <summary>Scans the current token for commands.</summary>
			void scan() noexcept;

			/// <returns>The location that the lexer is at in the script.</returns>
			size_t getLocation() const noexcept {
				return this->my_location;
			}
			/// <param name="loc">The new location to jump to in the script.</param>
			void setLocation(size_t loc) {
				this->my_location = loc;
				this->lookahead = &this->script_text.at(loc);
			}
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
		};
	}
}

#endif // TEXT_ADVENTURE_PARSER_SCRIPT_LEXER_HPP_INCLUDED