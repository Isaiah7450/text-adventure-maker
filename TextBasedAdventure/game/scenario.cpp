// File Author: Isaiah Hoffman
// File Created: January 2, 2017
#include <cassert>
#include <array>
#include <string>
#include <map>
#include <iostream>
#include <fstream>
#include <conio.h>
#include "scenario.hpp"
using namespace std::literals::string_literals;

namespace hoffman::isaiah {
	namespace game {
		// Commands
		void Scenario::showText(int number) {
			assert(number >= 0);
			std::cout << this->string_table.at(number) << '\n';
		}

		void Scenario::setFlag(int x, int y, int v) {
			// Warning: unsafe without script validation!
			assert(x >= 0 && x < Scenario::max_x_flag);
			assert(y >= 0 && y < Scenario::max_y_flag);
			this->scen_flags[x][y] = v;
		}

		void Scenario::setFlagIndirect(int x_set, int y_set, int x_get, int y_get) {
			// Warning: unsafe without script validation!
			assert(x_set >= 0 && x_set < Scenario::max_x_flag);
			assert(y_set >= 0 && y_set < Scenario::max_y_flag);
			assert(x_get >= 0 && x_get < Scenario::max_x_flag);
			assert(y_get >= 0 && y_get < Scenario::max_y_flag);
			this->scen_flags[x_set][y_set] = this->scen_flags[x_get][y_get];
		}

		void Scenario::retrieveFlag(int x_set, int y_set) {
			// Warning: unsafe without script validation!
			assert(x_set >= 0 && x_set < Scenario::max_x_flag);
			assert(y_set >= 0 && y_set < Scenario::max_y_flag);
			this->scen_flags[x_set][y_set] = this->number_buffer;
		}

		void Scenario::storeFlag(int x_get, int y_get) {
			// Warning: unsafe without script validation!
			assert(x_get >= 0 && x_get < Scenario::max_x_flag);
			assert(y_get >= 0 && y_get < Scenario::max_y_flag);
			this->number_buffer = this->scen_flags[x_get][y_get];
		}

		void Scenario::incrementFlag(int x, int y, int amt) {
			// Warning: unsafe without script validation!
			assert(x >= 0 && x < Scenario::max_x_flag);
			assert(y >= 0 && y < Scenario::max_y_flag);
			this->scen_flags[x][y] += amt;
		}

		void Scenario::addFlags(int x_set, int y_set, int x_get, int y_get) {
			// Warning: unsafe without script validation!
			assert(x_set >= 0 && x_set < Scenario::max_x_flag);
			assert(y_set >= 0 && y_set < Scenario::max_y_flag);
			assert(x_get >= 0 && x_get < Scenario::max_x_flag);
			assert(y_get >= 0 && y_get < Scenario::max_y_flag);
			this->scen_flags[x_set][y_set] += this->scen_flags[x_get][y_get];
		}

		void Scenario::subtractFlags(int x_set, int y_set, int x_get, int y_get) {
			// Warning: unsafe without script validation!
			assert(x_set >= 0 && x_set < Scenario::max_x_flag);
			assert(y_set >= 0 && y_set < Scenario::max_y_flag);
			assert(x_get >= 0 && x_get < Scenario::max_x_flag);
			assert(y_get >= 0 && y_get < Scenario::max_y_flag);
			this->scen_flags[x_set][y_set] -= this->scen_flags[x_get][y_get];
		}

		void Scenario::testFlags(int x_a, int y_a, int x_b, int y_b) {
			// Warning: unsafe without script validation!
			assert(x_a >= 0 && x_a < Scenario::max_x_flag);
			assert(y_a >= 0 && y_a < Scenario::max_y_flag);
			assert(x_b >= 0 && x_b < Scenario::max_x_flag);
			assert(y_b >= 0 && y_b < Scenario::max_y_flag);
			this->number_buffer = this->scen_flags[x_a][y_a] - this->scen_flags[x_b][y_b];
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

		void Scenario::addToBuffer(int v) noexcept {
			this->number_buffer += v;
		}

		void Scenario::testBuffer(int v) noexcept {
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

		bool Scenario::changeHealth(int amt) noexcept {
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
			int userInput = -1;
			do {
				std::string buffer {};
				std::getline(std::cin, buffer, '\n');
				userInput = std::stoi(buffer);
				// If an invalid input is given (as in not a number)
				if (std::to_string(userInput) != buffer) {
					std::cout << '\r';
					std::cout.flush();
				}
				else {
					validInput = true;
				}
			} while (!validInput);
			this->scen_flags[x][y] = userInput;
		}

		void Scenario::pause() noexcept {
			_getch();
		}

		void Scenario::doDeathSequence() const noexcept {
			std::cout << "Alas, you have died, and your adventure ends here.\n";
			_getch();
		}
	}
}