main.o: src/main.c include/main.h include/parser.h
	gcc -c -I ./include src/main.c

parser.o: src/parser.c include/parser.h
	gcc -c -I ./include src/parser.c

dss: main.o parser.o
	gcc main.o parser.o -o dss -lncurses
