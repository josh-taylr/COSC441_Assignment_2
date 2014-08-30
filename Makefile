CC=gcc
FLAGS=-W -Wall -ansi -pedantic -O0 -I./inc -std=c11 -g

SRC=./src
PROGFILES=ctime pipe1 pipe100 bbuf1 bbuf100 mq1 mq100

build: $(PROGFILES)

%: $(SRC)/%.c
	$(CC) $(FLAGS) $< -o $@

bbuf1: $(SRC)/bbuf1.c $(SRC)/bbuf.c
	$(CC) $(FLAGS) -DT=double $^ -o $@

bbuf100: $(SRC)/bbuf100.c $(SRC)/bbuf.c
	$(CC) $(FLAGS) -DT=double* $^ -o $@

clean:
	rm $(PROGFILES)