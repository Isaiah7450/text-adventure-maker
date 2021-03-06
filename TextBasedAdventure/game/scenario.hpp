#ifndef TEXT_ADVENTURE_GAME_SCENARIO_HPP_INCLUDED
#define TEXT_ADVENTURE_GAME_SCENARIO_HPP_INCLUDED
// File Author: Isaiah Hoffman
// File Created: January 2, 2018
#include <array>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <cinttypes>

namespace hoffman_isaiah {
	namespace game {
		/// <summary>Class that represents a scenario.</summary> 
		class Scenario {
		public:
			/// <param name="scen_name">The name of the scenario.</param>
			/// <param name="p_name">The name of the player.</param>
			Scenario(std::string scen_name, std::string p_name);

			/// <summary>Prints some basic information about the scenario.</summary>
			void printScenarioInfo() const noexcept {
				std::cout << "Welcome, " << this->player_name << ", to the scenario `" << this->scenario_name << "`.\n\n";
				std::cout << "Description: " << this->scenario_desc << "\n";
				std::cout << "Author: " << this->scenario_author
					<< "\tVersion: " << this->scenario_update_version << "\n\n";
			}

			// Getters
			int getStartingState() const noexcept {
				return this->starting_state;
			}

			// Commands
			// Warning: None of the parameters for commands are validated here.
			// It is IMPERATIVE that the script parser validates command paramaters.
			// (The asserts exist to aid debugging only.)		
			/// <summary>Shows the string of text with the given number in the string table.</summary>
			/// <param name="number">The string's number in the string table.</param>
			void showText(int number);
			/// <summary>Sets a flag to the given value.</summary>
			/// <param name="x">The x-index of the flag to set.</param>
			/// <param name="y">The y-index of the flag to set.</param>
			/// <param name="v">The value to set the flag at (x, y) to.</param>
			void setFlag(int x, int y, std::int16_t v);
			/// <summary>Sets a flag to another flag's value.
			/// To put it another way, this function copies one flag to another flag.</summary>
			/// <param name="x_set">The x-index of the flag to set.</param>
			/// <param name="y_set">The y-index of the flag to set.</param>
			/// <param name="x_get">The x-index of the flag to copy.</param>
			/// <param name="y_get">The y-index of the flag to copy.</param>
			void setFlagIndirect(int x_set, int y_set, int x_get, int y_get);
			/// <summary>Sets a flag to the number buffer's value.
			/// It is called 'Retrieve' because one is retrieving a flag's
			/// value from the number buffer.</summary>
			/// <param name="x_set">The x-index of the flag to set.</param>
			/// <param name="y_set">The y-index of the flag to set.</param>
			void retrieveFlag(int x_set, int y_set);
			/// <summary>Sets the number buffer to a flag's value.
			/// It is called 'Store' because one is storing a flag's
			/// value in the number buffer.</summary>
			/// <param name="x_get">The x-index of the flag to store.</param>
			/// <param name="y_get">The y-index of the flag to store.</param>
			void storeFlag(int x_get, int y_get);
			/// <summary>Increases the value of a flag.
			/// One can provide a negative amount to decrease the value of a flag.</summary>
			/// <param name="x">The x-index of the flag to change.</param>
			/// <param name="y">The y-index of the flag to change.</param>
			/// <param name="amt">The amount by which to change the flag's value.</param>
			void incrementFlag(int x, int y, std::int16_t amt);
			/// <summary>Adds two flags' values and stores the result in the first flag.</summary>
			/// <param name="x_set">The x-index of the flag to store the result in.</param>
			/// <param name="y_set">The y-index of the flag to store the result in.</param>
			/// <param name="x_get">The x-index of the other flag to add.</param>
			/// <param name="y_get">The y-index of the other flag to add.</param>
			void addFlags(int x_set, int y_set, int x_get, int y_get);
			/// <summary>Subtracts the second flag's value from the first flag's value and stores
			/// the result in the first flag.</summary>
			/// <param name="x_set">The x-index of the flag to store the result in.</param>
			/// <param name="y_set">The y-index of the flag to store the result in.</param>
			/// <param name="x_get">The x-index of the flag to subtract from the first flag.</param>
			/// <param name="y_get">The y-index of the flag to subtract from the first flag.</param>
			void subtractFlags(int x_set, int y_set, int x_get, int y_get);
			/// <summary>Stores the difference between two flags' values in the number buffer.</summary>
			/// <param name="x_a">The x-index of the first flag.</param>
			/// <param name="y_a">The y-index of the first flag.</param>
			/// <param name="x_b">The x-index of the second flag.</param>
			/// <param name="y_b">The y-index of the second flag.</param>
			void testFlags(int x_a, int y_a, int x_b, int y_b);
			/// <summary>Checks if the number buffer's value equals zero.</summary>
			/// <returns>True if the number buffer's value equals zero.</returns>
			bool checkIfZero() const noexcept;
			/// <summary>Checks if the number buffer's value does not equal zero.</summary>
			/// <returns>True if the number buffer's value does not equal zero.</returns>
			bool checkIfNotZero() const noexcept;
			/// <summary>Checks if the number buffer's value is positive.</summary>
			/// <returns>True if the number buffer's value is positive.</returns>
			bool checkIfPositive() const noexcept;
			/// <summary>Checks if the number buffer's value is negative.</summary>
			/// <returns>True if the number buffer's value is negative.</returns>
			bool checkIfNegative() const noexcept;
			/// <returns>The current value of the number buffer.</returns>
			int getBuffer() const noexcept {
				return this->number_buffer;
			}
			/// <summary>Resets the number buffer's value to zero.</summary>
			void resetBuffer() noexcept;
			/// <summary>Adds a value directly to the number buffer.</summary>
			/// <param name="v">The value to add to the buffer.</param>
			void addToBuffer(std::int16_t v) noexcept;
			/// <summary>Subtracts a value from the number buffer.</summary>
			/// <param name="v">The value to subtract from the buffer.</param>
			void testBuffer(std::int16_t v) noexcept;
			/// <summary>Ends the scenario.</summary>
			/// <param name="status">The ending status of the scenario.
			/// Zero indicates premature exit, one indicates success,
			/// and two indicates failure.</param>
			void endScenario(int status) noexcept;
			/// <summary>Stores the player's health in the number buffer.</summary>
			void storeHealth() noexcept;
			/// <summary>Changes the health of the player.</summary>
			/// <param name="amt">The amount by which to change the player's health.</param>
			/// <returns>True if the player is still alive else false.</returns>
			bool changeHealth(std::int16_t amt) noexcept;
			/// <summary>Changes the health of the player using the value of a flag.</summary>
			/// <param name="x">The x-index of the flag to use.</param>
			/// <param name="y">The y-index of the flag to use.</param>
			bool changeHealthIndirect(int x, int y) noexcept;
			/// <summary>Kills the player triggering a death ending.</summary>
			void killPlayer() noexcept;
			/// <summary>Obtains a number from the player and stores it in a flag.</summary>
			/// <param name="x">The x-index of the flag to set.</param>
			/// <param name="y">The y-index of the flag to set.</param>
			void getInput(int x, int y);
			/// <summary>Pauses the scenario until the player provides an input.</summary>
			void pause() noexcept;
			/// <summary>Clears the string stored in the buffer currently.</summary>
			void clearString() noexcept;
			/// <summary>Appends a string to the end of the buffer.</summary>
			/// <param name="number">The number associated with the string.</param>
			void appendString(int number);
			/// <summary>Appends a string to the end of the buffer based on the number stored in a flag.</summary>
			/// <param name="x">The x-index of the flag that stores the number of the string to append.</param>
			/// <param name="y">The y-index of the flag that stores the number of the string to append.</param>
			void appendStringIndirect(int x, int y);
			/// <summary>Appends the number in the number buffer to the end of the string buffer.</summary>
			void appendNumber() noexcept;
			/// <summary>Appends a space to the string buffer.</summary>
			void appendSpace() noexcept;
			/// <summary>Displays the current string in the string buffer. Note: You must manually clear the string buffer.
			/// A newline is automatically attached at the end.</summary>
			void displayString() const noexcept;
			/// <summary>Checks if a string with the given number exist.</summary>
			/// <param name="number">The number associated with the string.</param>
			bool doesStringExist(int number) const noexcept {
				return this->string_table.find(number) != this->string_table.end();
			}
			// The maximum valid state number that can be used in a scenario.
			static constexpr const size_t max_state_number = 99'999U;
			// The maximum valid x and y indices for the scenario's flags.
			static constexpr const size_t max_x_flag = 100'000U;
			static constexpr const size_t max_y_flag = 1'000U;
		protected:
			/// <summary>Loads the scenario's metadata and settings.</summary>
			void loadSettings();
			/// <summary>Loads the scenario's strings from the string file.</summary>
			void loadStrings();
			/// <summary>Displays some texts when the player dies.</summary>
			void doDeathSequence() const noexcept;
		private:
			/// <summary>This variable represents the scenario's internal name (used to resolve file references).</summary>
			std::string scenario_internal_name;
			/// <summary>This variable represents the scenario's name.</summary>
			std::string scenario_name = "";
			/// <summary>This variable represents the scenario's description.</summary>
			std::string scenario_desc = "";
			/// <summary>This variable represents the scenario's author.</summary>
			std::string scenario_author = "";
			/// <summary>This variable represents the scenario's version (as specified by the
			/// scenario creator, not the script version).</summary>
			int scenario_update_version = 1;
			/// <summary>This variable represents the scenario's available languages.</summary>
			std::set<std::string> scenario_available_langs {};
			/// <summary>This variable represents the scenario's current language.</summary>
			std::string scenario_lang = "";
			/// <summary>This variable represents the scenario's starting state.</summary>
			int starting_state = 0;
			/// <summary>This variable represents the player's name.</summary>
			std::string player_name;
			/// <summary>The variable represents the amount of health the player has remaining.</summary>
			std::int16_t player_health;
			/// <summary>This variable is a two-dimensional array of flags that the scenario uses.</summary>
			std::unique_ptr<std::array<std::array<std::int16_t, Scenario::max_y_flag>,
				Scenario::max_x_flag>> scen_flags;
			/// <summary>This variable maps the scenario's string numbers to the proper string used by the scenario.</summary>
			std::map<int, std::string> string_table;
			/// <summary>This variable represents the number buffer utilized by the scenario.</summary>
			std::int16_t number_buffer;
			/// <summary>This variable represents the string buffer utilized by the scenario.</summary>
			std::string string_buffer {};
		};
	}
}

#endif // TEXT_ADVENTURE_GAME_SCENARIO_HPP_INCLUDED