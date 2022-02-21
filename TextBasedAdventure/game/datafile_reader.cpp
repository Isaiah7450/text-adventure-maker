// File Author: Isaiah Hoffman
// File Created: December 18, 2018
#include "./datafile_reader.hpp"
#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>
using namespace std::literals::string_literals;
namespace hoffman_isaiah::util {
	DataFileReader::DataFileReader(std::istream& data_stream, std::string n) :
		my_data {},
		file_name {n},
		lookahead {} {
		if (!data_stream) {
			throw errors::DatafileException {"Could not open file `" + n + "`!", __FILE__, __func__, __LINE__,
				n, 0};
		}
		std::string my_buffer {};
		while (std::getline(data_stream, my_buffer, '\n')) {
			this->my_data += my_buffer + '\n';
		}
		if (this->my_data.size() == 0) {
			throw errors::DatafileException {"An empty datafile was provided.", __FILE__, __func__, __LINE__, n,
				this->getLineNumber()};
		}
		this->my_data += EOF;
		this->lookahead = &this->my_data[0];
	}

	DataFileReader::DataFileReader(std::string data_string, std::string n) :
		my_data {data_string + char{EOF}},
		file_name {n},
		lookahead {&my_data.at(0)} /* Order dependency! */ {
	}

	bool DataFileReader::getNextToken() {
		while (this->skipWhite() || this->skipComments()) {
			// Empty body.
		}
		this->my_token = {};
		if ((this->getLookahead() >= 'a' && this->getLookahead() <= 'z')
			|| (this->getLookahead() >= 'A' && this->getLookahead() <= 'Z')) {
			this->readIdentifier();
		}
		else if (this->getLookahead() == '=') {
			this->my_token = this->getLookahead();
			this->advance();
			this->my_token_type = DatafileTokenTypes::Equals_Sign;
		}
		else if (this->getLookahead() == '+' || this->getLookahead() == '-'
			|| (this->getLookahead() >= '0' && this->getLookahead() <= '9')) {
			this->readNumber();
		}
		else if (this->getLookahead() == '"') {
			this->readString();
		}
		else if (this->getLookahead() == ',') {
			this->my_token = this->getLookahead();
			this->advance();
			this->my_token_type = DatafileTokenTypes::Comma;
		}
		else if (this->getLookahead() == '[') {
			this->readSection();
		}
		else if (this->getLookahead() == '{') {
			this->my_token = this->getLookahead();
			this->advance();
			this->my_token_type = DatafileTokenTypes::Left_Brace;
		}
		else if (this->getLookahead() == '}') {
			this->my_token = this->getLookahead();
			this->advance();
			this->my_token_type = DatafileTokenTypes::Right_Brace;
		}
		else if (this->getLookahead() == '<') {
			this->my_token = this->getLookahead();
			this->advance();
			this->my_token_type = DatafileTokenTypes::Left_Angle;
		}
		else if (this->getLookahead() == '>') {
			this->my_token = this->getLookahead();
			this->advance();
			this->my_token_type = DatafileTokenTypes::Right_Angle;
		}
		else if (this->getLookahead() == EOF) {
			this->my_token = EOF;
			this->my_token_type = DatafileTokenTypes::End_Of_File;
			return false;
		}
		return true;
	}

	void DataFileReader::expectTokenType(DatafileTokenTypes expected) const {
		if (!this->matchesTokenType(expected)) {
			switch (expected) {
			case DatafileTokenTypes::Unknown:
				// Throw exception--bad code path.
				break;
			case DatafileTokenTypes::End_Of_File:
				throw errors::DatafileException {"Expected end-of-file.", __FILE__, __func__, __LINE__,
					this->getFileName(), this->getLineNumber()};
			case DatafileTokenTypes::Left_Brace:
				throw errors::DatafileException {"Expected `{`.", __FILE__, __func__, __LINE__,
					this->getFileName(), this->getLineNumber()};
			case DatafileTokenTypes::Right_Brace:
				throw errors::DatafileException {"Expected `}`.", __FILE__, __func__, __LINE__,
					this->getFileName(), this->getLineNumber()};
			case DatafileTokenTypes::Left_Angle:
				throw errors::DatafileException {"Expected `<`.", __FILE__, __func__, __LINE__,
					this->getFileName(), this->getLineNumber()};
			case DatafileTokenTypes::Right_Angle:
				throw errors::DatafileException {"Expected `>`.", __FILE__, __func__, __LINE__,
					this->getFileName(), this->getLineNumber()};
			case DatafileTokenTypes::Section:
				throw errors::DatafileException {"Expected a section header. (Sections are enclosed within square brackets.)",
					__FILE__, __func__, __LINE__, this->getFileName(), this->getLineNumber()};
			case DatafileTokenTypes::Identifier:
				throw errors::DatafileException {"Expected an identifier. (Identifiers are not enclosed within anything.)",
					__FILE__, __func__, __LINE__, this->getFileName(), this->getLineNumber()};
			case DatafileTokenTypes::String:
				throw errors::DatafileException {"Expected a string. (Strings are enclosed within double quotes.)",
					__FILE__, __func__, __LINE__, this->getFileName(), this->getLineNumber()};
			case DatafileTokenTypes::Number:
				throw errors::DatafileException {"Expected a number.", __FILE__, __func__, __LINE__,
					this->getFileName(), this->getLineNumber()};
			case DatafileTokenTypes::Equals_Sign:
				throw errors::DatafileException {"Expected `=`.", __FILE__, __func__, __LINE__,
					this->getFileName(), this->getLineNumber()};
			case DatafileTokenTypes::Comma:
				throw errors::DatafileException {"Expected `,`.", __FILE__, __func__, __LINE__,
					this->getFileName(), this->getLineNumber()};
			case DatafileTokenTypes::List:
				throw errors::DatafileException {"Expected a list of items enclosed within angle brackets.",
					__FILE__, __func__, __LINE__, this->getFileName(), this->getLineNumber()};
			}
		}
	}

	void DataFileReader::expectToken(DatafileTokenTypes expected_type, std::string expected_value) const {
		this->expectTokenType(expected_type);
		if (!this->matchesTokenValue(expected_value)) {
			throw errors::DatafileException {"Expected token to be `"s + expected_value + "`.\n"
				"(Received `" + this->my_token + "` instead.)", __FILE__, __func__, __LINE__,
				this->getFileName(), this->getLineNumber()};
		}
	}

	int DataFileReader::parseInteger() const {
		this->expectTokenType(DatafileTokenTypes::Number);
		try {
			return std::stoi(this->my_token);
		}
		catch (const std::invalid_argument&) {
			throw errors::DatafileException {"Expected an integer value.", __FILE__, __func__, __LINE__,
				this->getFileName(), this->getLineNumber()};
		}
		catch (const std::out_of_range&) {
			throw errors::DatafileException {"Expected an integer less than four billion.", __FILE__, __func__, __LINE__,
				this->getFileName(), this->getLineNumber()};
		}
	}

	long long DataFileReader::parseLargeInteger() const {
		this->expectTokenType(DatafileTokenTypes::Number);
		try {
			return std::stoll(this->my_token);
		}
		catch (const std::invalid_argument&) {
			throw errors::DatafileException {"Expected an integer value.", __FILE__, __func__, __LINE__,
				this->getFileName(), this->getLineNumber()};
		}
		catch (const std::out_of_range&) {
			throw errors::DatafileException {"Expected an integer less than 9 quintillion.", __FILE__, __func__, __LINE__,
				this->getFileName(), this->getLineNumber()};
		}
	}

	double DataFileReader::parseFloat() const {
		this->expectTokenType(DatafileTokenTypes::Number);
		try {
			return std::stod(this->my_token);
		}
		catch (const std::invalid_argument&) {
			throw errors::DatafileException {"Expected a decimal value.", __FILE__, __func__, __LINE__,
				this->getFileName(), this->getLineNumber()};
		}
		catch (const std::out_of_range&) {
			throw errors::DatafileException {"Expected a reasonable decimal value.", __FILE__, __func__, __LINE__,
				this->getFileName(), this->getLineNumber()};
		}
	}

	bool DataFileReader::parseBoolean() const {
		this->expectTokenType(DatafileTokenTypes::Identifier);
		// If I wasn't lazy, I'd make this work for any capitalization.
		if (this->matchesTokenValue("True") || this->matchesTokenValue("true") || this->matchesTokenValue("TRUE")) {
			return true;
		}
		else if (this->matchesTokenValue("False") || this->matchesTokenValue("false") || this->matchesTokenValue("FALSE")) {
			return false;
		}
		throw errors::DatafileException {"Expected a boolean value. (One of `True` or `False`.)", __FILE__, __func__, __LINE__,
			this->getFileName(), this->getLineNumber()};
	}

	void DataFileReader::readKeyValuePair(std::string expected_key) {
		this->expectToken(DatafileTokenTypes::Identifier, expected_key);
		this->getNextToken();
		this->expectTokenType(DatafileTokenTypes::Equals_Sign);
		this->getNextToken();
	}

	DatafileTokenTypes DataFileReader::readList() {
		std::string list_string {};
		this->expectTokenType(DatafileTokenTypes::Left_Angle);
		this->getNextToken();
		const auto list_type = this->getTokenType();
		if (list_type != DatafileTokenTypes::Identifier && list_type != DatafileTokenTypes::Number
			&& list_type != DatafileTokenTypes::String) {
			throw errors::DatafileException {"Expected either an identifier, a number, or a string following `<`.",
				__FILE__, __func__, __LINE__, this->getFileName(), this->getLineNumber()};
		}
		list_string += this->my_token;
		while (this->getLookahead() != WEOF && this->getLookahead() != '>') {
			this->getNextToken();
			this->expectTokenType(DatafileTokenTypes::Comma);
			this->getNextToken();
			this->expectTokenType(list_type);
			list_string += ' ' + this->my_token;
		}
		this->getNextToken();
		this->expectTokenType(DatafileTokenTypes::Right_Angle);
		if (this->getLookahead() != ' ' && this->getLookahead() != '\t' && this->getLookahead() != '\r'
			&& this->getLookahead() != '\n' && this->getLookahead() != WEOF) {
			throw errors::DatafileException {"Expected either end-of-file or whitespace.", __FILE__, __func__, __LINE__,
				this->getFileName(), this->getLineNumber()};
		}
		this->advance();
		this->my_token_type = DatafileTokenTypes::List;
		this->my_token = list_string;
		return list_type;
	}

	int DataFileReader::readHeader(DataFileReader& my_parser) {
		my_parser.getNextToken();
		my_parser.expectToken(util::DatafileTokenTypes::Section, "[global]");
		my_parser.getNextToken();
		my_parser.readKeyValuePair("version");
		return my_parser.parseInteger();
	}

	bool DataFileReader::skipWhite() noexcept {
		bool did_skip = false;
		while (this->getLookahead() == ' ' || this->getLookahead() == '\r'
			|| this->getLookahead() == '\t' || this->getLookahead() == '\n') {
			did_skip = true;
			if (this->getLookahead() == '\n') {
				++this->line_number;
			}
			// Yes, pointer arithmetic is generally frowned, but this allows one
			// to forget having to track an index.
			this->advance();
		}
		return did_skip;
	}

	bool DataFileReader::skipComments() noexcept {
		bool did_skip = false;
		// Note: All comments last until the end of the line.
		while (this->getLookahead() == '#' || this->getLookahead() == ';') {
			did_skip = true;
			while (this->getLookahead() != '\n' && this->getLookahead() != EOF) {
				this->advance();
			}
			if (this->getLookahead() == '\n') {
				++this->line_number;
				this->advance();
			}
		}
		return did_skip;
	}

	void DataFileReader::readIdentifier() {
		// Identifiers: [a-zA-Z][\S]*
		this->my_token_type = DatafileTokenTypes::Identifier;
		do {
			this->my_token += this->getLookahead();
			this->advance();
		} while (this->getLookahead() != ' ' && this->getLookahead() != '\t' && this->getLookahead() != '\r'
			&& this->getLookahead() != '\n' && this->getLookahead() != ',' && this->getLookahead() != WEOF
			&& this->getLookahead() != '>');
	}

	void DataFileReader::readSection() {
		// Sections: \[[\w]+\]
		this->my_token_type = DatafileTokenTypes::Section;
		do {
			this->my_token += this->getLookahead();
			this->advance();
		} while ((this->getLookahead() >= 'a' && this->getLookahead() <= 'z')
			|| (this->getLookahead() >= 'A' && this->getLookahead() <= 'Z') || this->getLookahead() == '_'
			|| (this->getLookahead() >= '0' && this->getLookahead() <= '9'));
		if (this->getLookahead() != ']') {
			throw errors::DatafileException {"Expected: `]`.", __FILE__, __func__, __LINE__,
				this->getFileName(), this->getLineNumber()};
		}
		this->my_token += this->getLookahead();
		this->advance();
		if (this->getLookahead() != ' ' && this->getLookahead() != '\t' && this->getLookahead() != '\r'
			&& this->getLookahead() != '\n' && this->getLookahead() != WEOF) {
			throw errors::DatafileException {"Expected either end-of-file or whitespace.", __FILE__, __func__, __LINE__,
				this->getFileName(), this->getLineNumber()};
		}
	}

	void DataFileReader::readString() {
		// Strings: "[^"]*"
		// Note also that the escape sequences \t, \n, \", and \\
		// are recognized and will be replaced with a tab, a newline, a double
		// quote, and a single backslash respectively.
		this->my_token_type = DatafileTokenTypes::String;
		bool previous_was_slash = true;
		do {
			const auto current_look = this->getLookahead();
			if (previous_was_slash) {
				// Do escape sequences replacements.
				switch (current_look) {
				case 't':
					this->my_token += '\t';
					break;
				case 'n':
					this->my_token += '\n';
					break;
				case '"':
					this->my_token += '"';
					break;
				case '\\':
					this->my_token += '\\';
					break;
				default:
					throw errors::DatafileException {"Expected a valid escape sequence: \\t, \\n, \\\", or \\\\.",
						__FILE__, __func__, __LINE__, this->getFileName(), this->getLineNumber()};
				}
				previous_was_slash = false;
			}
			else if (this->getLookahead() == '\\') {
				previous_was_slash = true;
			}
			else {
				this->my_token += this->getLookahead();
			}
			if (this->getLookahead() == '\n') {
				++this->line_number;
			}
			this->advance();
		} while ((this->getLookahead() != '"' || previous_was_slash) && this->getLookahead() != WEOF);
		if (this->getLookahead() != '"') {
			throw errors::DatafileException {"Expected `\"`.", __FILE__, __func__, __LINE__,
				this->getFileName(), this->getLineNumber()};
		}
		this->my_token += this->getLookahead();
		this->advance();
		if (this->getLookahead() != ' ' && this->getLookahead() != '\t' && this->getLookahead() != '\r'
			&& this->getLookahead() != '\n' && this->getLookahead() != ',' && this->getLookahead() != WEOF
			&& this->getLookahead() != '>') {
			throw errors::DatafileException {"Expected either end-of-file or whitespace.", __FILE__, __func__, __LINE__,
				this->getFileName(), this->getLineNumber()};
		}
	}

	void DataFileReader::readNumber() {
		// Numbers: [\+-] ? [\d_]+[\.[\d_]*]?
		this->my_token_type = DatafileTokenTypes::Number;
		bool has_decimal_point = false;
		const bool starting_plus_minus = this->getLookahead() == '+' || this->getLookahead() == '-';
		do {
			if (this->getLookahead() == '.' && !has_decimal_point) {
				has_decimal_point = true;
				this->my_token += this->getLookahead();
				if (starting_plus_minus && this->my_token.size() == 1) {
					throw errors::DatafileException {"Expected a digit before the decimal point.",
						__FILE__, __func__, __LINE__, this->getFileName(), this->getLineNumber()};
				}
			}
			else if (this->getLookahead() != '_') {
				this->my_token += this->getLookahead();
			}
			if (this->getLookahead() == '.') {
				has_decimal_point = true;
			}
			this->advance();
		} while ((this->getLookahead() >= '0' && this->getLookahead() <= '9')
			|| (this->getLookahead() == '.' && !has_decimal_point) || this->getLookahead() == '_');
		if (starting_plus_minus && this->my_token.size() == 1) {
			throw errors::DatafileException {"Expected at least one digit in the number.", __FILE__,
				__func__, __LINE__, this->getFileName(), this->getLineNumber()};
		}
		if (this->getLookahead() != ' ' && this->getLookahead() != '\t' && this->getLookahead() != '\r'
			&& this->getLookahead() != '\n' && this->getLookahead() != ',' && this->getLookahead() != WEOF
			&& this->getLookahead() != '>') {
			throw errors::DatafileException {"Expected either end-of-file or whitespace.", __FILE__, __func__, __LINE__,
				this->getFileName(), this->getLineNumber()};
		}
	}
}
