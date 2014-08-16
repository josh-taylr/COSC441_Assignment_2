CC=gcc
FLAGS=-W -Wall -ansi -pedantic -O2 -I./inc -Wno-long-long

PROGFILES=bbuf ctime pipe

build: $(PROGFILES)
	
%: src/%.c 
	$(CC) $(FLAGS) $< -o ./$@

clean:
	rm $(PROGFILES)