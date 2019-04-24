// File Author: Isaiah Hoffman
// File Created: January 2, 2017
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
#include "./../parser/script_globals.hpp"
using namespace std::literals::string_literals;

namespace hoffman::isaiah {
	namespace game {
		Scenario::Scenario(std::string scen_name, std::string p_name) :
			scenario_name {scen_name},
			player_name {p_name},
			player_health {100},
			scen_flags {std::make_unique<std::array<std::array<std::int16_t, Scenario::max_y_flag>, Scenario::max_x_flag>>()},
			string_table {},
			number_buffer {0} {
			this->loadStrings();
		}

		void Scenario::loadStrings() {
			auto fileName = "./scenarios/"s + this->scenario_name + "/strings.txt"s;
			std::ifstream myFile {fileName};
			std::string buffer {};
			bool reached_end = myFile.fail() || myFile.bad();
			int line_no = 1;
			while (!reached_end) {
				reached_end = !std::getline(myFile, buffer, ' ');
				if (!reached_end) {
					size_t substring_start = 0;
					while (buffer.at(substring_start) == '\n' || buffer.at(substring_start) == '\r'
						|| buffer.at(substring_start) == '\t' || buffer.at(substring_start) == ' ') {
						++substring_start;
					}
					buffer = buffer.substr(substring_start);
					if (buffer[0] != '\'') {
						int str_no = 0;
						try {
							str_no = std::stoi(buffer);
							if (std::to_string(str_no) != buffer) {
								throw std::string {"Invalid input."};
							}
						}
						catch (...) {
							throw parser::ScriptError {"Invalid string number: "s + buffer + "."s,
								parser::ErrorSeverity::Error, fileName, line_no};
						}
						if (this->string_table.find(str_no) != this->string_table.end()) {
							throw parser::ScriptError {"Duplicate string number: "s + buffer + "."s,
								parser::ErrorSeverity::Error, fileName, line_no};
						}
						reached_end = !std::getline(myFile, buffer, '\n');
						this->string_table.emplace(str_no, buffer);
					}
					else {
						reached_end = !std::getline(myFile, buffer, '\n');
						continue;
					}
				}
				++line_no;
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