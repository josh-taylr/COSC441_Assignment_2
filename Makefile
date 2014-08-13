CC=gcc
FLAGS=-W -Wall -ansi -pedantic -O0 -I./inc

PROGFILES=bbuf ctime

build: $(PROGFILES)
	
%: src/%.c 
	$(CC) $(FLAGS) $< -o ./$@

clean:
	rm $(PROGFILES)