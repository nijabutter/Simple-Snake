default: build

build:
	gcc -o snake snake.c -lncurses

run: build
	./snake
