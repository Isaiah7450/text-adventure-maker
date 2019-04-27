// File Author: Isaiah Hoffman
// File Created: January 2, 2017
#include <string>
#include <iostream>
#include <map>
#include <utility>
#include <memory>
#include <cinttypes>
#include "./../parser/script_globals.hpp"
#include "./../parser/script_parser.hpp"
#include "./../parser/script_lexer.hpp"
#include "./../game/scenario.hpp"

using namespace std::literals::string_literals;

namespace hoffman::isaiah {
	namespace parser {
		ScriptParser::ScriptParser(std::istream& script, std::string file, game::Scenario& scen_data) :
			lexer {std::make_unique<ScriptLexer>(script, file)},
			scenario_data {scen_data} {
		}
		ScriptParser::ScriptParser(std::string script, game::Scenario& scen_data) :
			lexer {std::make_unique<ScriptLexer>(script)},
			scenario_data {scen_data} {
		}
		ScriptParser::~ScriptParser() noexcept = default;

		void ScriptParser::start(int start_state) {
			// Create jump table
			while (!this->lexer->getNext()) {
				this->lexer->scan();
				this->lexer->skipComments();
				auto command = this->lexer->getCommandToken();
				if (command == ScriptCommands::State) {
					auto state_number = this->parseNumber();
					if (this->jumpTable.find(state_number) != this->jumpTable.end()) {
						// Repeated state number
						throw ScriptParseError {"State number "s + std::to_string(state_number) + " was repeated."s,
							ErrorSeverity::Error, this->lexer->getFileName(), this->lexer->getLineNumber()};
					}
					// Add to jump table
					this->jumpTable.emplace(state_number, std::make_pair(this->lexer->getLocation(),
						this->lexer->getLineNumber()));
				}
			}
			this->lexer->setLocation(0U, 1);
			this->parseScript(start_state);
		}

		void ScriptParser::parseScript(int start_state) {
			this->lexer->getNext();
			this->lexer->scan();
			this->lexer->skipComments();
			this->lexer->matchCommand(ScriptCommands::Begin_Script);
			bool end_script = false;
			if (start_state == -1) {
				this->checkStateExists(this->scenario_data.getStartingState());
				const auto start_loc = this->jumpTable.at(this->scenario_data.getStartingState());
				this->lexer->setLocation(start_loc.first, start_loc.second);
			}
			else {
				this->checkStateExists(start_state);
				const auto start_loc = this->jumpTable.at(start_state);
				// Set jump location to the provided start state.
				this->lexer->setLocation(start_loc.first, start_loc.second);
			}
			while (!this->lexer->getNext() && !end_script) {
				this->lexer->scan();
				this->lexer->skipComments();
				const auto command = this->lexer->getCommandToken();
				switch (command) {
					case ScriptCommands::Show_Text:
					{
						auto text_no = this->parseNumber();
						if (!this->scenario_data.doesStringExist(text_no)) {
							throw ScriptParseError {"String number "s + std::to_string(text_no) + " does not exist."s,
								ErrorSeverity::Error, this->lexer->getFileName(), this->lexer->getLineNumber()};
						}
						this->scenario_data.showText(text_no);
						break;
					}
					case ScriptCommands::Set_Flag:
					{
						auto myFlag = this->parseFlag();
						auto value = static_cast<std::int16_t>(this->parseNumber());
						this->scenario_data.setFlag(myFlag.first, myFlag.second, value);
						break;
					}
					case ScriptCommands::Set_Flag_Indirect:
					{
						auto flagToSet = this->parseFlag();
						auto flagToCopy = this->parseFlag();
						this->scenario_data.setFlagIndirect(flagToSet.first, flagToSet.second,
							flagToCopy.first, flagToCopy.second);
						break;
					}
					case ScriptCommands::Retrieve_Flag:
					{
						auto myFlag = this->parseFlag();
						this->scenario_data.retrieveFlag(myFlag.first, myFlag.second);
						break;
					}
					case ScriptCommands::Store_Flag:
					{
						auto myFlag = this->parseFlag();
						this->scenario_data.storeFlag(myFlag.first, myFlag.second);
						break;
					}
					case ScriptCommands::Increment_Flag:
					{
						auto myFlag = this->parseFlag();
						auto amount = static_cast<std::int16_t>(this->parseNumber());
						this->scenario_data.incrementFlag(myFlag.first, myFlag.second, amount);
						break;
					}
					case ScriptCommands::Add_Flags:
					{
						auto flagToSet = this->parseFlag();
						auto flagToAdd = this->parseFlag();
						this->scenario_data.addFlags(flagToSet.first, flagToSet.second,
							flagToAdd.first, flagToAdd.second);
						break;
					}
					case ScriptCommands::Subtract_Flags:
					{
						auto flagToSet = this->parseFlag();
						auto flagToSubtract = this->parseFlag();
						this->scenario_data.subtractFlags(flagToSet.first, flagToSet.second,
							flagToSubtract.first, flagToSubtract.second);
						break;
					}
					case ScriptCommands::Test_Flags:
					{
						auto flagA = this->parseFlag();
						auto flagB = this->parseFlag();
						this->scenario_data.testFlags(flagA.first, flagA.second,
							flagB.first, flagB.second);
						break;
					}
					case ScriptCommands::Jump_If_Zero:
					case ScriptCommands::Jump_If_Not_Zero:
					case ScriptCommands::Jump_If_Positive:
					case ScriptCommands::Jump_If_Negative:
					case ScriptCommands::Jump_To_State:
					{
						auto state_no = this->parseNumber();
						this->checkStateExists(state_no);
						// Evaluate jump condition
						auto do_jump = true;
						switch (command) {
							case ScriptCommands::Jump_If_Zero:
								do_jump = this->scenario_data.checkIfZero();
								break;
							case ScriptCommands::Jump_If_Not_Zero:
								do_jump = this->scenario_data.checkIfNotZero();
								break;
							case ScriptCommands::Jump_If_Positive:
								do_jump = this->scenario_data.checkIfPositive();
								break;
							case ScriptCommands::Jump_If_Negative:
								do_jump = this->scenario_data.checkIfNegative();
							default:
								break;
						}
						// Do jump if appropriate
						if (do_jump) {
							auto jump_loc = this->jumpTable.at(state_no);
							this->lexer->setLocation(jump_loc.first, jump_loc.second);
						}
						break;
					}
					case ScriptCommands::Jump_On_Buffer:
					{
						auto min_state = this->parseNumber();
						auto max_state = this->parseNumber();
						if (max_state < min_state) {
							throw ScriptParseError {"Invalid bounds for `Jump_On_Buffer` command.",
								ErrorSeverity::Error, this->lexer->getFileName(), this->lexer->getLineNumber()};
						}
						auto jump_state = this->scenario_data.getBuffer();
						if (jump_state >= min_state && jump_state <= max_state) {
							auto jump_loc = this->jumpTable.at(jump_state);
							this->lexer->setLocation(jump_loc.first, jump_loc.second);
						}
						break;
					}
					case ScriptCommands::Reset_Buffer:
						this->scenario_data.resetBuffer();
						break;
					case ScriptCommands::Add_To_Buffer:
					{
						auto value = static_cast<std::int16_t>(this->parseNumber());
						this->scenario_data.addToBuffer(value);
						break;
					}
					case ScriptCommands::Test_Buffer:
					{
						auto value = static_cast<std::int16_t>(this->parseNumber());
						this->scenario_data.testBuffer(value);
						break;
					}
					case ScriptCommands::End_Scenario:
					{
						auto status = this->parseNumber();
						try {
							if (status < 0 || status > 2) {
								throw ScriptParseError {"The {Status} parameter for `End_Scenario` should be between 0 and 2.",
									ErrorSeverity::Warning, this->lexer->getFileName(), this->lexer->getLineNumber()};
							}
						}
						catch (const ScriptError& e) {
							if (static_cast<int>(e.getLevel()) < static_cast<int>(ErrorSeverity::Error)) {
								std::cerr << e.what();
							}
							else throw;
						}
						this->scenario_data.endScenario(status);
						end_script = true;
						break;
					}
					case ScriptCommands::Store_Health:
						this->scenario_data.storeHealth();
						break;
					case ScriptCommands::Change_Health:
					{
						auto amount = static_cast<std::int16_t>(this->parseNumber());
						if (!this->scenario_data.changeHealth(amount)) {
							end_script = true;
						}
						break;
					}
					case ScriptCommands::Kill_Player:
						this->scenario_data.killPlayer();
						end_script = true;
						break;
					case ScriptCommands::Get_Input:
					{
						auto myFlag = this->parseFlag();
						this->scenario_data.getInput(myFlag.first, myFlag.second);
						break;
					}
					case ScriptCommands::Pause:
						this->scenario_data.pause();
						break;
					case ScriptCommands::Clear_String:
						this->scenario_data.clearString();
						break;
					case ScriptCommands::Append_String:
					{
						const auto text_no = this->parseNumber();
						this->scenario_data.appendString(text_no);
						break;
					}
					case ScriptCommands::Append_String_Indirect:
					{
						const auto my_flag = this->parseFlag();
						this->scenario_data.appendStringIndirect(my_flag.first, my_flag.second);
						break;
					}
					case ScriptCommands::Append_Number:
						this->scenario_data.appendNumber();
						break;
					case ScriptCommands::Append_Space:
						this->scenario_data.appendSpace();
						break;
					case ScriptCommands::Display_String:
						this->scenario_data.displayString();
						break;
					case ScriptCommands::State:
						// Just skip over this command's one argument
						this->lexer->getNext();
						break;
					case ScriptCommands::Begin_Script:
						throw ScriptParseError {"Begin_Script should only appear at the beginning of a script.",
							ErrorSeverity::Error, this->lexer->getFileName(), this->lexer->getLineNumber()};
						break;
					case ScriptCommands::End_Script:
						throw ScriptParseError {"End_Script should only appear at the end of a script.",
							ErrorSeverity::Error, this->lexer->getFileName(), this->lexer->getLineNumber()};
						break;
					case ScriptCommands::Comment:
					case ScriptCommands::End_Comment:
						throw ScriptParseError {"Unexpected Comment or End_Comment command encountered!",
							ErrorSeverity::Internal, this->lexer->getFileName(), this->lexer->getLineNumber()};
						break;
					case ScriptCommands::Number:
						throw ScriptParseError {"Expected a command but received a number instead.",
							ErrorSeverity::Error, this->lexer->getFileName(), this->lexer->getLineNumber()};
						break;
					case ScriptCommands::Unknown:
					default:
						throw ScriptParseError {"Unrecognized command: "s + this->lexer->getToken() + "."s,
							ErrorSeverity::Error, this->lexer->getFileName(), this->lexer->getLineNumber()};
						break;
				}
			}
		}

		void ScriptParser::checkStateExists(int state_no) const {
			if (this->jumpTable.find(state_no) == this->jumpTable.end()) {
				throw ScriptParseError {"Could not find state "s + std::to_string(state_no) + "."s,
					ErrorSeverity::Error, this->lexer->getFileName(), this->lexer->getLineNumber()};
			}
		}

		int ScriptParser::parseNumber() {
			this->lexer->getNext();
			this->lexer->matchCommand(ScriptCommands::Number);
			return std::stoi(this->lexer->getToken());
		}

		std::pair<int, int> ScriptParser::parseFlag() {
			auto flag_x = this->parseNumber();
			auto flag_y = this->parseNumber();
			if (flag_x < 0 || flag_x >= game::Scenario::max_x_flag) {
				throw ScriptParseError {"The x-index of a flag must be between 0 and "s
					+ std::to_string(game::Scenario::max_x_flag - 1) + " (inclusive)."s,
					ErrorSeverity::Error, this->lexer->getFileName(), this->lexer->getLineNumber()};
			}
			if (flag_y < 0 || flag_y >= game::Scenario::max_y_flag) {
				throw ScriptParseError {"The y-index of a flag must be between 0 and "s
					+ std::to_string(game::Scenario::max_y_flag - 1) + " (inclusive)."s,
					ErrorSeverity::Error, this->lexer->getFileName(), this->lexer->getLineNumber()};
			}
			return std::make_pair(flag_x, flag_y);
		}
	} // Inner namespace
} // Outer namespace