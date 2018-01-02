// File Author: Isaiah Hoffman
// File Created: January 2, 2017
#include <string>
#include <iostream>
#include <memory>
#include "script_globals.hpp"
#include "script_parser.hpp"
#include "script_lexer.hpp"

namespace hoffman::isaiah {
	namespace parser {
		ScriptParser::ScriptParser(std::istream& script, std::string file) :
			lexer {std::make_unique<ScriptLexer>(script, file)} {
		}
		ScriptParser::ScriptParser(std::string script) :
			lexer {std::make_unique<ScriptLexer>(script)} {
		}
		ScriptParser::~ScriptParser() noexcept = default;

		void ScriptParser::start() {
			// Create jump table
			bool inComment = false;

		}
	}
}