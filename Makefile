# Created by: Isaiah Hoffman
# Created on: February 21, 2022
# This Makefile is meant for compiling on Windows using git bash and gcc.
# Other platforms or tools may require modifications. To compile with MSVC,
# just use the included solution file.
.RECIPEPREFIX := >
.PHONY := clean rebuild
CC := gcc
C++ := g++
My_Flags := -Wall -Wextra -pedantic-errors -std=c++17 -Wno-unknown-pragmas
Root_Dir := ./TextBasedAdventure
Obj_Dir := $(Root_Dir)/obj
All_Objects := $(Obj_Dir)/main.o $(Obj_Dir)/datafile_reader.o \
	$(Obj_Dir)/scenario.o $(Obj_Dir)/script_lexer.o \
	$(Obj_Dir)/script_parser.o
All_Headers := $(Root_Dir)/game/datafile_reader.hpp \
	$(Root_Dir)/game/scenario.hpp $(Root_Dir)/parser/script_lexer.hpp \
	$(Root_Dir)/parser/script_parser.hpp

release: $(All_Objects) $(All_Headers)
> $(C++) $(All_Objects) $(My_Flags) -o $(Root_Dir)/../bin/x64_Release_TextBasedAdventure.exe

$(Obj_Dir)/main.o: $(All_Headers) $(Root_Dir)/main.cpp
> mkdir -p $(Obj_Dir)
> $(CC) $(My_Flags) -c $(Root_Dir)/main.cpp -o $(Obj_Dir)/main.o

$(Obj_Dir)/datafile_reader.o: $(All_Headers) \
	$(Root_Dir)/game/datafile_reader.cpp
> mkdir -p $(Obj_Dir)
> $(CC) $(My_Flags) -c $(Root_Dir)/game/datafile_reader.cpp -o \
	$(Obj_Dir)/datafile_reader.o

$(Obj_Dir)/scenario.o: $(All_Headers) $(Root_Dir)/game/scenario.cpp
> mkdir -p $(Obj_Dir)
> $(CC) $(My_Flags) -c $(Root_Dir)/game/scenario.cpp -o $(Obj_Dir)/scenario.o

$(Obj_Dir)/script_lexer.o: $(All_Headers) $(Root_Dir)/parser/script_lexer.cpp
> mkdir -p $(Obj_Dir)
> $(CC) $(My_Flags) -c $(Root_Dir)/parser/script_lexer.cpp -o \
	$(Obj_Dir)/script_lexer.o

$(Obj_Dir)/script_parser.o: $(All_Headers) $(Root_Dir)/parser/script_parser.cpp
> mkdir -p $(Obj_Dir)
> $(CC) $(My_Flags) -c $(Root_Dir)/parser/script_parser.cpp -o \
	$(Obj_Dir)/script_parser.o

clean:
> rm -f $(All_Objects)

rebuild: clean release
