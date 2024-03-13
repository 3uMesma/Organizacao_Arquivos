all:
	gcc -o programaTrab *.c -lm -Wall
run:
	./programaTrab
test:
	clear
	make all
	make run
valgrind:
	gcc -g -Wall -Werror *.c && valgrind ./programaTrab -s