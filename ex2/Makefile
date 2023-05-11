OBJ = src/main.o src/coder.o src/command.o

all: main

main: $(OBJ)
	gcc -o $@ $^

%.o: src/%.c
	gcc -Wall -o $@ $<

clean: 
	rm main $(OBJ)
