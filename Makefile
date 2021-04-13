CPPFLAGS:= -Wno-deprecated -std=c++11

all:
	bison -d src/nutshparser.y
	flex src/nutshscanner.l
	g++ -o nutshell src/*.cpp src/shell_functions/*.cpp $(CPPFLAGS) lex.yy.c nutshparser.tab.c

clean:
	rm nutshparser.tab.c nutshparser.tab.h lex.yy.c nutshell