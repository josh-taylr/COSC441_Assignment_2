CC=gcc
FLAGS=-W -Wall -ansi -pedantic -O4 -I./inc -std=c11

SRC=./src
OBJ=./obj
PROGFILES=ctime (buff)

%: $(SRC)/ctime.c
	$(CC) $(FLAGS) $< -o ./$@

%: src/%.c 
	$(CC) $(FLAGS) $< -o ./$@

clean:
	rm $(PROGFILES)