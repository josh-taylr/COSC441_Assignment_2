CC=gcc
FLAGS=-W -Wall -ansi -pedantic -O4 -I./inc -std=c11

SRC=./src
OBJ=./obj
PROGFILES=ctime (buff)

%: $(SRC)/ctime.c
	$(CC) $(FLAGS) $< -o ./$@

%: src/%.c 
	$(CC) $(FLAGS) $< -o ./$@

pipe1: $(SRC)/pipe1.c
	$(CC) $(FLAGS) $< -o ./$@

pipe100:
	$(CC) $(FLAGS) $< -o ./$@
bbuf1:
	$(CC) $(FLAGS) $< -o ./$@
bbuf100:
	$(CC) $(FLAGS) $< -o ./$@
mq1:
	$(CC) $(FLAGS) $< -o ./$@
mq100:
	$(CC) $(FLAGS) $< -o ./$@

clean:
	rm $(PROGFILES)