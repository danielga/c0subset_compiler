all: parser

OBJS=	instruction.o	\
		symbol.o		\
		node.o			\
		parser.o		\
		main.o			\
		tokens.o		\

CPPFLAGS=-Wall -std=gnu++11

clean:
	$(RM) parser.cpp parser.hpp parser.output parser tokens.cpp $(OBJS)

parser.cpp: parser.y
	bison -v -d -o $@ $^

parser.hpp: parser.cpp

tokens.cpp: tokens.l parser.hpp
	flex -o $@ $^

%.o: %.cpp
	g++ -c $(CPPFLAGS) -o $@ $<

parser: $(OBJS)
	g++ -o $@ $(OBJS)

test: parser example.txt
	./parser < example.txt > example.asm
