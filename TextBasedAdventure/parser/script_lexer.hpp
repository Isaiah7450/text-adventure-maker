#ifndef TEXT_ADVENTURE_PARSER_SCRIPT_LEXER_HPP_INCLUDED
#define TEXT_ADVENTURE_PARSER_SCRIPT_LEXER_HPP_INCLUDED
// File Author: Isaiah Hoffman
// File Created: December 30, 2017
#include <string>
#include <iosfwd>
#include "script_globals.hpp"
namespace hoffman::isaiah {
	namespace parser {
		/// <summary>Represents the various types of tokens recognized by the lexical scanner.</summary>
		enum class ScriptTokenTypes {
			Unknown, Command, Number
		};

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
			void getNext();
			/// <summary>Scans the current token for commands.</summary>
			void scan() noexcept;
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
			/// <summary>This variable keeps track of the file (if relevant) being scanned.</summary>
			std::string file_name;
			/// <summary>This variable keeps track of the current line the lexical scanner is at in the script text.</summary>
			int line_number;
		};
	}
}

#endif // TEXT_ADVENTURE_PARSER_SCRIPT_LEXER_HPP_INCLUDED