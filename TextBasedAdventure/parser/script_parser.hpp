#ifndef TEXT_ADVENTURE_PARSER_SCRIPT_PARSER_HPP_INCLUDED
#define TEXT_ADVENTURE_PARSER_SCRIPT_PARSER_HPP_INCLUDED
// File Author: Isaiah Hoffman
// File Created: January 2, 2018
#include <string>
#include <iosfwd>
#include <memory>
#include "script_globals.hpp"

namespace hoffman::isaiah {
	namespace parser {
		/// <summary>Represents an object that can validate and interpret a script.</summary>
		class ScriptParser {
		public:
			ScriptParser(std::istream& script) :
				lexer {std::make_unique<ScriptLexer>(script)} {
			}
			ScriptParser(std::string script) :
				lexer {std::make_unique<ScriptLexer>(script)} {
			}
		private:
			/// <summary>The companion lexical scanner that the parser is using.</summary>
			std::unique_ptr<ScriptLexer> lexer;
		};
	}
}

#endif // TEXT_ADVENTURE_PARSER_SCRIPT_PARSER_HPP_INCLUDED