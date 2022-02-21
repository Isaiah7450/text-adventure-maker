#pragma once
// File Author: Isaiah Hoffman
// File Created: December 18, 2018
#include <iosfwd>
#include <memory>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

namespace hoffman_isaiah::errors {
	class DatafileException : public std::runtime_error {
	public:
		// Note: The use of __FILE__, __func__, and __LINE__ are expected for the last three parameters.
		/// <param name="m">The error message.</param>
		/// <param name="pfile">The (program's) source file where the error occurred.</param>
		/// <param name="pfunc">The (program's) source function where the error occurred.</param>
		/// <param name="pline">The (program's) source line where the error occurred.</param>
		DatafileException(std::string m, const char* pfile, const char* pfunc, int pline,
			std::string dfile, int dline) :
			std::runtime_error {m},
			// file_str_owner {std::make_unique<const char*>(file)},
			// func_str_owner {std::make_unique<const char*>(func)},
			msg {m},
			program_file {pfile},
			program_function {pfunc},
			// program_file {file_str_owner.get()}, // Order dependency!
			// program_function {func_str_owner.get()}, // Order dependency!
			program_line {pline},
			data_file {dfile},
			data_line {dline} {
		}
		/// <returns>A full textual representation of the exception.</returns>
		const char* what() const noexcept override {
			using namespace std::literals::string_literals;
			std::string my_msg = "In file " + this->data_file + ":\n"
				"\tError: " + this->getMessage() + " (Line " + std::to_string(this->data_line) + ")."
#ifdef DEBUG
				+ "\nSee also:\n" + "In file "s + this->getProgramFile() + ", function "s + this->getProgramFunction() + ":\n"
				"\tError: " + this->getMessage() + " (Line " + std::to_string(this->getProgramLine()) + ").";
#else
				; // Looks a bit weird, but I'm just allowing the second part only to be displayed in debug builds.
#endif
			return my_msg.c_str();
		}
		// Getters
		std::string getMessage() const {
			return this->msg;
		}
		std::string getProgramFile() const {
			return this->program_file;
		}
		std::string getProgramFunction() const {
			return this->program_function;
		}
		int getProgramLine() const noexcept {
			return this->program_line;
		}
	private:
		/*
		/// <summary>Pointer that owns the actual (program) file name.</summary>
		std::unique_ptr<const char*> file_str_owner;
		/// <summary>Pointer that owns the actual (program) function name.</summary>
		std::unique_ptr<const char*> func_str_owner;
		*/
		/// <summary>The error message.</summary>
		std::string msg;
		/// <summary>The (program's) file where the error occurred.</summary>
		std::string program_file;
		/// <summary>The (program's) function where the error occurred.</summary>
		std::string program_function;
		/// <summary>The (program's) line where the error occurred.</summary>
		int program_line;
		/// <summary>The name of the datafile being loaded.</summary>
		std::string data_file;
		/// <summary>The line that the error occurred at in the datafile.</summary>
		int data_line;
	};
}

namespace hoffman_isaiah::util {
	// Note: I make the (probably unreasonable) assumption that every character used in the
	// datafile fits a single 2-byte character. (I.e.: I don't consider the case for multibyte
	// characters.) This is done to vastly simplify this program.
	enum class DatafileTokenTypes {
		Unknown, End_Of_File, Left_Brace, Right_Brace, Left_Angle, Right_Angle, Section,
		Identifier, String, Number, Equals_Sign, Comma, List
	};
	// Note: This custom datafile format resembles the format of INI files, but I have made some
	// extensions and changes.
	/// <summary>Reads a datafile in incremental bits.</summary>
	class DataFileReader {
	public:
		/// <param name="data_stream">An input stream for the datafile.</param>
		/// <param name="n">The name of the datafile.</param>
		DataFileReader(std::istream& data_stream, std::string n);
		// Precondition: data_string.size() > 0. (This is not explicitly checked.)
		/// <param name="data_string">The contents of the datafile as one long string.</param>
		/// <param name="n">The name of the datafile.</param>
		DataFileReader(std::string data_string, std::string n = "Unknown");

		/// <summary>Updates the reader and extracts the next token.</summary>
		/// <returns>False if EOF was encountered; otherwise true.</returns>
		bool getNextToken();
		/// <param name="type">The expected type.</param>
		/// <returns>True if the current token's type matches the provided type.</returns>
		bool matchesTokenType(DatafileTokenTypes type) const noexcept {
			return this->getTokenType() == type;
		}
		/// <param name="value">The expected token string.</param>
		/// <returns>True if the current token's string matches the provided token's string.</returns>
		bool matchesTokenValue(std::string value) const noexcept {
			return this->my_token == value;
		}
		/// <param name="type">The expected type.</param>
		/// <param name="value">The expected token string.</param>
		/// <returns>True if the current token matches the provided token.</returns>
		bool matchesToken(DatafileTokenTypes type, std::string value) const {
			return this->matchesTokenType(type) && this->matchesTokenValue(value);
		}
		/// <summary>Throws an exception if the current token's type does not match the expected type.</summary>
		/// <param name="expected">The expected token type.</param>
		void expectTokenType(DatafileTokenTypes expected) const;
		/// <summary>Throws an exception if the current token does not completely match the provided arguments.</summary>
		/// <param name="expected_type">The expected token type.</param>
		/// <param name="expected_value">The expected token string.</param>
		void expectToken(DatafileTokenTypes expected_type, std::string expected_value) const;
		/// <returns>The current token as an identifier.</returns>
		std::string parseIdentifier() const {
			this->expectTokenType(DatafileTokenTypes::Identifier);
			return this->my_token;
		}
		/// <returns>The current token as a section header (with the surrounding square brackets.)</returns>
		std::string parseSection() const {
			this->expectTokenType(DatafileTokenTypes::Section);
			return this->my_token;
		}
		/// <returns>The current token as a string (with the surrounding quotes.)</returns>
		std::string parseString() const {
			this->expectTokenType(DatafileTokenTypes::String);
			// Returns the string without the surrounding quote marks.
			return this->my_token.substr(1, this->my_token.size() - 2);
		}
		/// <returns>The current token as an integer.</returns>
		int parseInteger() const;
		/// <returns>The current token as a large integer.</returns>
		long long parseLargeInteger() const;
		/// <returns>The current token as a floating-point value.</returns>
		double parseFloat() const;
		/// <returns>The current token as a boolean.</returns>
		bool parseBoolean() const;
		// <returns>The current token expressed as the type indicated by the type template.</returns>
		template <typename T>
		T parseAny() {
			if constexpr (std::is_same_v<T, int>) {
				return this->parseInteger();
			}
			else if constexpr (std::is_same_v<T, long long>) {
				return this->parseLargeInteger();
			}
			else if constexpr (std::is_same_v<T, double>) {
				return this->parseFloat();
			}
			else if constexpr (std::is_same_v<T, bool>) {
				return this->parseBoolean();
			}
			else if constexpr (std::is_same_v<T, std::string>) {
				if (this->matchesTokenType(DatafileTokenTypes::String)) {
					return this->parseString();
				}
				return this->parseIdentifier();
			}
		}
		// <returns>The current token as a vector of the list's items.</returns>
		template <typename T>
		std::vector<T> parseList() {
			this->expectTokenType(DatafileTokenTypes::List);
			std::string buffer {};
			std::vector<T> list_items {};
			const auto my_token_size = this->my_token.size();
			for (size_t i = 0; i < my_token_size; ++i) {
				if (this->my_token[i] != L' ') {
					buffer += this->my_token[i];
				}
				if (this->my_token[i] == L' ' || i == my_token_size - 1) {
					const auto temp_token = this->my_token;
					this->my_token_type = DataFileReader::convertCPPTypeToTokenType<T>(buffer);
					this->my_token = buffer;
					list_items.emplace_back(this->parseAny<T>());
					this->my_token = temp_token;
					buffer = {};
				}
			}
			return list_items;
		}
		/// <summary>Reads in a property from the input.</summary>
		/// <param name="expected_key">The expected key value.</param>
		void readKeyValuePair(std::string expected_key);
		/// <summary>Reads in a list of homogeneous values from the input.</summary>
		/// <returns>The type of the list's elements.</returns>
		DatafileTokenTypes readList();

		/// <summary>Reads the header of a datafile. (Everything up to and including the version info.)</summary>
		/// <param name="my_parser">The parser for the datafile.</param>
		/// <returns>The version number.</returns>
		static int readHeader(DataFileReader& my_parser);
		// Getters
		std::string getFileName() const {
			return this->file_name;
		}
		int getLineNumber() const noexcept {
			return this->line_number;
		}
		char getLookahead() const noexcept {
			return *this->lookahead;
		}
	protected:
#pragma warning(push)
#pragma warning(disable: 26481) // Code Analysis: CPP Core --> Pointer arithmetic.
		/// <summary>Advances the reader by one character.</summary>
		void advance() noexcept {
			++this->lookahead;
		}
#pragma warning(pop)
		/// <summary>Skips whitespace in the datafile.</summary>
		/// <returns>True if any whitespace was skipped.</returns>
		bool skipWhite() noexcept;
		/// <summary>Skips comments in the datafile.</summary>
		/// <returns>True if any comments were skipped.</returns>
		bool skipComments() noexcept;
		/// <summary>Reads in an identifier from the input.</summary>
		void readIdentifier();
		/// <summary>Reads in a section header from the input.</summary>
		void readSection();
		/// <summary>Reads in a string from the input.</summary>
		void readString();
		/// <summary>Reads in a number from the input.</summary>
		void readNumber();
		// Getters
		DatafileTokenTypes getTokenType() const noexcept {
			return this->my_token_type;
		}
	private:
		// Returns the token type constant that corresponds with certain C++ types.
		template <typename T>
		constexpr static DatafileTokenTypes convertCPPTypeToTokenType(std::string my_str) {
			if constexpr (std::is_same_v<T, int> || std::is_same_v<T, double>) {
				return DatafileTokenTypes::Number;
			}
			else if constexpr (std::is_same_v<T, bool>) {
				return DatafileTokenTypes::Identifier;
			}
			else if constexpr (std::is_same_v<T, std::string>) {
				return my_str[0] == L'"' ? DatafileTokenTypes::String : DatafileTokenTypes::Identifier;
			}
			else {
				// GCC doesn't seem to support constexpr if statements.
				#ifndef __GNUC__
				static_assert(false, "An invalid type was provided.");
				#endif
			}
		}

		/// <summary>The contents of the datafile.</summary>
		std::string my_data;
		/// <summary>The name of the file being read.</summary>
		std::string file_name;
		/// <summary>The current line number of the file being read.</summary>
		int line_number {1};
		/// <summary>Pointer to the current lookahead character.</summary>
		char* lookahead;
		/// <summary>The current token.</summary>
		std::string my_token {};
		/// <summary>The current token's type.</summary>
		DatafileTokenTypes my_token_type {DatafileTokenTypes::Unknown};
	};
}
