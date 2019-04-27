// File Author: Isaiah Hoffman
// File Created: January 2, 2017
#include <algorithm>
#include <array>
#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <cassert>
#include <cinttypes>
#include <conio.h>
#include "./../game/scenario.hpp"
#include "./../game/datafile_reader.hpp"
#include "./../parser/script_globals.hpp"
using namespace std::literals::string_literals;

namespace hoffman::isaiah {
	namespace game {
		Scenario::Scenario(std::string scen_name, std::string p_name) :
			scenario_internal_name {scen_name},
			player_name {p_name},
			player_health {100},
			scen_flags {std::make_unique<std::array<std::array<std::int16_t, Scenario::max_y_flag>, Scenario::max_x_flag>>()},
			string_table {},
			number_buffer {0} {
			this->loadSettings();
			this->loadStrings();
		}

		void Scenario::loadSettings() {
			const auto file_name = "./scenarios/"s + this->scenario_internal_name + "/settings.txt"s;
			std::ifstream my_file {file_name};
			util::DataFileReader my_parser {my_file, file_name};
			const int file_version = util::DataFileReader::readHeader(my_parser);
			if (file_version == 1) {
				my_parser.getNextToken();
				my_parser.expectToken(util::DatafileTokenTypes::Section, "[metadata]");
				my_parser.getNextToken();
				my_parser.readKeyValuePair("name");
				this->scenario_name = my_parser.parseString();
				my_parser.getNextToken();
				my_parser.readKeyValuePair("desc");
				this->scenario_desc = my_parser.parseString();
				my_parser.getNextToken();
				my_parser.readKeyValuePair("author");
				this->scenario_author = my_parser.parseString();
				my_parser.getNextToken();
				my_parser.readKeyValuePair("version");
				this->scenario_update_version = my_parser.parseInteger();
				my_parser.getNextToken();
				my_parser.readKeyValuePair("languages");
				my_parser.readList();
				const auto my_langs = my_parser.parseList<std::string>();
				for (const auto lang : my_langs) {
					this->scenario_available_langs.emplace(lang);
				}
				// I should do this some other time or separate this into a separate function.
				if (this->scenario_available_langs.size() == 1) {
					this->scenario_lang = *this->scenario_available_langs.begin();
				}
				else if (this->scenario_available_langs.size() > 1) {
					bool is_valid = false;
					while (!is_valid) {
						std::cout << "Select a language for the scenario:\n";
						for (const auto lang : this->scenario_available_langs) {
							std::cout << lang << "\n";
						}
						std::getline(std::cin, this->scenario_lang, '\n');
						is_valid = std::binary_search(this->scenario_available_langs.cbegin(), this->scenario_available_langs.cend(),
							this->scenario_lang);
					}
				}
				my_parser.getNextToken();
				my_parser.expectToken(util::DatafileTokenTypes::Section, "[settings]");
				my_parser.getNextToken();
				my_parser.readKeyValuePair("start_state");
				this->starting_state = my_parser.parseInteger();
			}
			else {
				throw std::runtime_error {"Invalid settings file version specified."};
			}
		}

		void Scenario::loadStrings() {
			auto fileName = this->scenario_lang == "" ? "./scenarios/"s + this->scenario_internal_name + "/strings.txt"s
				: "./scenarios/" + this->scenario_internal_name + "/"s + this->scenario_lang + "/strings.txt"s;
			std::ifstream myFile {fileName};
			std::string my_string_file_text = "";
			my_string_file_text.reserve(5000);
			std::string buffer {};
			while (std::getline(myFile, buffer, '\n')) {
				my_string_file_text += buffer + '\n';
			}
			std::size_t current_loc = 0;
			auto get_next_line_lambda = [my_string_file_text, &buffer, &current_loc]() {
				buffer = "";
				while (current_loc < my_string_file_text.size()) {
					buffer += my_string_file_text.at(current_loc);
					if (my_string_file_text.at(current_loc++) == '\n') {
						buffer.pop_back();
						return false;
					}
				}
				return true;
			};
			auto normalize_line_string_lambda = [&buffer]() {
				std::size_t substring_start = 0;
				while (substring_start < buffer.size() && (buffer.at(substring_start) == '\t'
					|| buffer.at(substring_start) == ' ')) {
					++substring_start;
				}
				buffer = buffer.substr(substring_start);
			};
			bool reached_end = get_next_line_lambda();
			int line_no = 1;
			while (!reached_end) {
				normalize_line_string_lambda();
				if (buffer[0] != '\'' && buffer != "") {
					int str_no = 0;
					try {
						str_no = std::stoi(buffer.substr(0, buffer.find_first_of(' ')));
					}
					catch (...) {
						throw parser::ScriptError {"Invalid string number: "s + buffer + "."s,
							parser::ErrorSeverity::Error, fileName, line_no};
					}
					if (this->string_table.find(str_no) != this->string_table.end()) {
						throw parser::ScriptError {"Duplicate string number: "s + buffer + "."s,
							parser::ErrorSeverity::Error, fileName, line_no};
					}
					// Special string number.
					if (str_no == 0) {
						this->scenario_name = buffer.substr(buffer.find_first_of(' ') + 1);
					}
					else {
						this->string_table.emplace(str_no, buffer.substr(buffer.find_first_of(' ') + 1));
					}
				}
				++line_no;
				reached_end = get_next_line_lambda();
			}
		}

		// Commands
		void Scenario::showText(int number) {
			assert(number >= 0);
			std::cout << this->string_table.at(number) << '\n';
		}

		void Scenario::setFlag(int x, int y, std::int16_t v) {
			// Warning: unsafe without script validation!
			assert(x >= 0 && x < Scenario::max_x_flag);
			assert(y >= 0 && y < Scenario::max_y_flag);
			(*this->scen_flags)[x][y] = v;
		}

		void Scenario::setFlagIndirect(int x_set, int y_set, int x_get, int y_get) {
			// Warning: unsafe without script validation!
			assert(x_set >= 0 && x_set < Scenario::max_x_flag);
			assert(y_set >= 0 && y_set < Scenario::max_y_flag);
			assert(x_get >= 0 && x_get < Scenario::max_x_flag);
			assert(y_get >= 0 && y_get < Scenario::max_y_flag);
			(*this->scen_flags)[x_set][y_set] = (*this->scen_flags)[x_get][y_get];
		}

		void Scenario::retrieveFlag(int x_set, int y_set) {
			// Warning: unsafe without script validation!
			assert(x_set >= 0 && x_set < Scenario::max_x_flag);
			assert(y_set >= 0 && y_set < Scenario::max_y_flag);
			(*this->scen_flags)[x_set][y_set] = this->number_buffer;
		}

		void Scenario::storeFlag(int x_get, int y_get) {
			// Warning: unsafe without script validation!
			assert(x_get >= 0 && x_get < Scenario::max_x_flag);
			assert(y_get >= 0 && y_get < Scenario::max_y_flag);
			this->number_buffer = (*this->scen_flags)[x_get][y_get];
		}

		void Scenario::incrementFlag(int x, int y, std::int16_t amt) {
			// Warning: unsafe without script validation!
			assert(x >= 0 && x < Scenario::max_x_flag);
			assert(y >= 0 && y < Scenario::max_y_flag);
			(*this->scen_flags)[x][y] += amt;
		}

		void Scenario::addFlags(int x_set, int y_set, int x_get, int y_get) {
			// Warning: unsafe without script validation!
			assert(x_set >= 0 && x_set < Scenario::max_x_flag);
			assert(y_set >= 0 && y_set < Scenario::max_y_flag);
			assert(x_get >= 0 && x_get < Scenario::max_x_flag);
			assert(y_get >= 0 && y_get < Scenario::max_y_flag);
			(*this->scen_flags)[x_set][y_set] += (*this->scen_flags)[x_get][y_get];
		}

		void Scenario::subtractFlags(int x_set, int y_set, int x_get, int y_get) {
			// Warning: unsafe without script validation!
			assert(x_set >= 0 && x_set < Scenario::max_x_flag);
			assert(y_set >= 0 && y_set < Scenario::max_y_flag);
			assert(x_get >= 0 && x_get < Scenario::max_x_flag);
			assert(y_get >= 0 && y_get < Scenario::max_y_flag);
			(*this->scen_flags)[x_set][y_set] -= (*this->scen_flags)[x_get][y_get];
		}

		void Scenario::testFlags(int x_a, int y_a, int x_b, int y_b) {
			// Warning: unsafe without script validation!
			assert(x_a >= 0 && x_a < Scenario::max_x_flag);
			assert(y_a >= 0 && y_a < Scenario::max_y_flag);
			assert(x_b >= 0 && x_b < Scenario::max_x_flag);
			assert(y_b >= 0 && y_b < Scenario::max_y_flag);
			this->number_buffer = (*this->scen_flags)[x_a][y_a] - (*this->scen_flags)[x_b][y_b];
		}

		bool Scenario::checkIfZero() const noexcept {
			return this->number_buffer == 0;
		}

		bool Scenario::checkIfNotZero() const noexcept {
			return this->number_buffer != 0;
		}

		bool Scenario::checkIfPositive() const noexcept {
			return this->number_buffer > 0;
		}

		bool Scenario::checkIfNegative() const noexcept {
			return this->number_buffer < 0;
		}

		void Scenario::resetBuffer() noexcept {
			this->number_buffer = 0;
		}

		void Scenario::addToBuffer(std::int16_t v) noexcept {
			this->number_buffer += v;
		}

		void Scenario::testBuffer(std::int16_t v) noexcept {
			this->number_buffer -= v;
		}

		void Scenario::endScenario(int status) noexcept {
			assert(status >= 0 && status <= 2);
			if (status == 0) {
				std::cout << "You ended the scenario early!\n";
			}
			else if (status == 1) {
				std::cout << "You won the scenario!\n";
			}
			else {
				std::cout << "You lost the scenario!\n";
			}
			_getch();
		}

		void Scenario::storeHealth() noexcept {
			this->number_buffer = this->player_health;
		}

		bool Scenario::changeHealth(std::int16_t amt) noexcept {
			if (amt > 0) {
				std::cout << "You gained " << amt << " health.\n";
			}
			else if (amt < 0) {
				std::cout << "You lost " << -amt << " health.\n";
			}
			this->player_health += amt;
			if (this->player_health > 0) {
				return true;
			}
			this->doDeathSequence();
			return false;
		}

		void Scenario::killPlayer() noexcept {
			this->doDeathSequence();
			this->player_health = 0;
		}

		void Scenario::getInput(int x, int y) {
			// Warning: unsafe without script validation!
			assert(x >= 0 && x < Scenario::max_x_flag);
			assert(y >= 0 && y < Scenario::max_y_flag);
			bool validInput = false;
			std::int16_t userInput = -1;
			auto startPos = std::cout.tellp();
			do {
				std::string buffer {};
				std::getline(std::cin, buffer, '\n');
				try {
					userInput = static_cast<std::int16_t>(std::stoi(buffer));
					// If an invalid input is given (as in not a number)
					if (std::to_string(userInput) != buffer) {
						throw std::string {"Invalid input."};
					}
					else {
						validInput = true;
					}
				}
				catch (...) {
					std::cout << "Try again: ";
					std::cout.flush();
				}
			} while (!validInput);
			(*this->scen_flags)[x][y] = userInput;
		}

		void Scenario::pause() noexcept {
			_getch();
		}

		void Scenario::clearString() noexcept {
			this->string_buffer = "";
		}

		void Scenario::appendString(int number) {
			assert(number >= 0);
			this->string_buffer += this->string_table[number];
		}

		void Scenario::appendStringIndirect(int x, int y) {
			// Warning: unsafe without script validation!
			assert(x >= 0 && x < Scenario::max_x_flag);
			assert(y >= 0 && y < Scenario::max_y_flag);
			assert((*this->scen_flags)[x][y] >= 0);
			this->appendString((*this->scen_flags)[x][y]);
		}

		void Scenario::appendNumber() noexcept {
			this->string_buffer += std::to_string(this->getBuffer());
		}

		void Scenario::appendSpace() noexcept {
			this->string_buffer += " ";
		}

		void Scenario::displayString() const noexcept {
			std::cout << this->string_buffer << "\n";
		}

		void Scenario::doDeathSequence() const noexcept {
			std::cout << "Alas, you have died, and your adventure ends here.\n";
			_getch();
		}
	}
}