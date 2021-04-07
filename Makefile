CPPFLAGS:= -Wno-deprecated

all:
	bison -d nutshparser.y
	flex nutshscanner.l
	g++ -o nutshell nutshell.cpp $(CPPFLAGS) lex.yy.c nutshparser.tab.c 

clean:
	rm nutshparser.tab.c nutshparser.tab.h lex.yy.c nutshell