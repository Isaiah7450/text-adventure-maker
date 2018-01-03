// File Author: Isaiah Hoffman
// File Created: December 30, 2017
#include <string>
#include <iostream>
#include <fstream>
#include <conio.h>
#include "./game/scenario.hpp"
#include "./parser/script_parser.hpp"
using namespace std::literals::string_literals;
namespace iyh = hoffman::isaiah;

int main(int argc, const char* argv[]) {
	// Validate number of arguments
	if (argc != 2) {
		std::cerr << "Error: Invalid number of arguments.\n";
		std::cerr << "Usage: " << argv[0] << " {scenario_name}\n"
			<< "\t{scenario_name} : The name of the scenario to load.\n\n"
			<< "Press any key to exit.\n";
		_getch();
		return 1;
	}
	// Get user's name.
	std::cout << "What is your name?\n";
	std::string yourName {};
	std::getline(std::cin, yourName, '\n');
	std::cout << "Welcome, "s << yourName << ", to the scenario `"s << argv[1] << "`.\n";
	_getch();
	// Load scenario
	try {
		auto scen_data = iyh::game::Scenario {argv[1], yourName};
		auto scen_script_file_name = "./scenarios/"s + argv[1] + "/script.txt"s;
		std::ifstream scen_script_file {scen_script_file_name};
		auto scen_parser = iyh::parser::ScriptParser {scen_script_file, scen_script_file_name, scen_data};
		scen_parser.start();
	}
	catch (const iyh::parser::ScriptError& e) {
		// Handle any errors that occurred
		std::cerr << e.what();
		_getch();
		return 1;
	}

	return 0;
}