default: build

build:
	gcc -o snake snake.c -std=c99 -lncurses

run: build
	./snake
