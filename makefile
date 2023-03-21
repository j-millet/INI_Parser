all: ini-parser

ini-parser:
	gcc -o ini-parser ini-parser.c strutils.c

clean: 
	rm ini-parser