// File Author: Isaiah Hoffman
// File Created: December 30, 2017
#include <string>
#include <iostream>
#include <conio.h>
using namespace std::literals::string_literals;

int main(int argc, const char* argv[]) {
	// Validate number of arguments
	if (argc != 2) {
		std::cerr << "Error: Invalid number of arguments.\n";
		std::cerr << "Usage: " << argv[0] << " {scenario_name}\n"
			<< "\t{scenario_name} : The name of the scenario to load.\n\n"
			<< "Press any key to exit.\n";
		getch();
		return 1;
	}
	// Load scenario
	return 0;
}