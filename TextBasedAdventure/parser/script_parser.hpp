#ifndef TEXT_ADVENTURE_PARSER_SCRIPT_PARSER_HPP_INCLUDED
#define TEXT_ADVENTURE_PARSER_SCRIPT_PARSER_HPP_INCLUDED
// File Author: Isaiah Hoffman
// File Created: January 2, 2018
#include <string>
#include <iosfwd>
#include <memory>
#include <map>
#include <utility>
#include "./../parser/script_globals.hpp"
#include "./../game/scenario.hpp"

namespace hoffman::isaiah {
	namespace parser {
		/// <summary>Represents an object that can validate and interpret a script.</summary>
		class ScriptParser {
		public:
			// Constructors; parameters almost identical to lexical scanner's parameters
			/// <param name="scen">Reference to the scenario data.</param>
			ScriptParser(std::istream& script, std::string file, game::Scenario& scen_data);
			ScriptParser(std::string script, game::Scenario& scen_data);
			// Destructor defined in source file because of forward declaration
			~ScriptParser() noexcept;
			// Rule of 5 should apply
			ScriptParser(const ScriptParser& rhs) = delete;
			ScriptParser(ScriptParser&& rhs) = default;
			ScriptParser& operator=(const ScriptParser& rhs) = delete;
			ScriptParser& operator=(ScriptParser&& rhs) = default;
			/// <summary>Starts the parser.</summary>
			/// <param name="start_state">The state to start at. If -1, the default state
			/// (generally state 0) is used.</param>
			void start(int start_state = -1);
		protected:
			/// <summary>Causes the parser to parse the script.</summary>
			void parseScript();
			/// <summary>Checks if the given state exists in the jump table, and throws an exception
			/// if it does not.</summary>
			/// <param name="state_no">The state number to look for.</param>
			void checkStateExists(int state_no) const;
			/// <summary>Interprets the following input as a number and validates it.</summary>
			/// <returns>The parsed number.</returns>
			int parseNumber();
			/// <summary>Interprets the following input as a pair of flag indices
			/// and validates them.</summary>
			/// <returns>The x and y indices of the parsed flag.</returns>
			std::pair<int, int> parseFlag();
		private:
			/// <summary>The companion lexical scanner that the parser is using.</summary>
			std::unique_ptr<ScriptLexer> lexer;
			/// <summary>A mapping of state numbers to their location and line number in the script file.</summary>
			std::map<int, std::pair<size_t, int>> jumpTable;
			/// <summary>This variable is a reference to the scenario data.</summary>
			game::Scenario& scenario_data;
		};
	}
}

#endif // TEXT_ADVENTURE_PARSER_SCRIPT_PARSER_HPP_INCLUDED