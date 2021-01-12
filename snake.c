#include <unistd.h>
#include <ncurses.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#define FRUIT_CHAR ACS_PI

int main()
{
	const int FRAME_DELAY = 72000;
	int WIDTH = 20;
	int HEIGHT = 20;
	
	const char SNAKE_CHAR = '#';
	const char GRID_CHAR = '.';
	// const char FRUIT_CHAR = '𝜋';

	srand(time(NULL));
	initscr();
	noecho();
	cbreak();
  	curs_set(0);
	timeout(1);

	int snake_X = 10;
	int snake_Y= 10;
	int tail_Length = 1;
	int fruit_X = 15;
	int fruit_Y = 15;
	
	int tailX[100], tailY[100];
	tailX[0] = snake_X-1;
	tailY[0] = snake_Y;
	
	enum eDirection { STOP = 0, LEFT, RIGHT, UP, DOWN };
	enum eDirection direction;
	direction = STOP;
	
	bool running = true;
	bool showGrid = false;

	while (running) {

		/*
		 * Input
		 */
		switch(getch()) {
			case 'q':
				running = false;
				break;
			case 'w':
				if (direction != DOWN)
					direction = UP;
				break;
			case 'a':
				if (direction != RIGHT)
					direction = LEFT;
				break;
			case 's':
				if (direction != UP)
					direction = DOWN;
				break;
			case 'd':
				if (direction != LEFT)
					direction = RIGHT;
				break;
			case ' ':
				showGrid = !showGrid;
				break;
			default:
				break;
		}

		/*
		 * Logic
		 */
		if (snake_Y == fruit_Y && snake_X == fruit_X) {
			// eaten the fruit
			tail_Length++;
			fruit_X = rand() % WIDTH;
			fruit_Y = rand() % HEIGHT;
		}
		if (snake_Y < 0 || snake_Y >= HEIGHT) {
			// off screen top or bottom
			running = false;
		}
		if (snake_X < 0 || snake_X >= WIDTH) {
			// off screen left or right
			running = false;
		}
		int prevX = tailX[0];
		int prevY = tailY[0];
		int prev2X, prev2Y;
		tailX[0] = snake_X;
		tailY[0] = snake_Y;
		for (int i = 1; i < tail_Length; i++) {
			prev2X = tailX[i];
			prev2Y = tailY[i];
			tailX[i] = prevX;
			tailY[i] = prevY;
			prevX = prev2X;
			prevY = prev2Y;
		}
		switch (direction) {
			case UP:
				snake_Y--;
				break;
			case DOWN:
				snake_Y++;
				break;
			case LEFT:
				snake_X--;
				break;
			case RIGHT:
				snake_X++;
				break;
			default:
				break;
		}
		
		/*
		 * Draw
		 */
		erase();
		for (int y = -1; y <= HEIGHT; y++) {
			for (int x = -1; x <= WIDTH; x++) {
					if (y == -1) {
						if (x == -1) {
							addch(ACS_ULCORNER);
						}
						else if (x == WIDTH) {
							addch(ACS_URCORNER);
						}
						else {
							addch(ACS_HLINE); // need two because of adding space
							addch(ACS_HLINE);
						}
					}
					else if (y == HEIGHT) {
						if (x == -1) {
							addch(ACS_LLCORNER);
						}
						else if (x == WIDTH) {
							addch(ACS_LRCORNER);
						}
						else {
							addch(ACS_HLINE);
							addch(ACS_HLINE);
						}
					}
					else if (x == -1 || x == WIDTH) {
						addch(ACS_VLINE);
					}
					else {
						if (y == snake_Y && x == snake_X) {
							addch(SNAKE_CHAR);
						}
						else if (y == fruit_Y && x == fruit_X) {
							addch(FRUIT_CHAR);
						}
						else {
							bool printed = false;
							for (int t = 0; t < tail_Length; t++) {
								if (t != 0 && snake_Y == tailY[t] && snake_X == tailX[t]) {
									running = false;
								}
								if (tailY[t] == y && tailX[t] == x) {
									addch(SNAKE_CHAR);
									printed = true;
									break;
								}
							}
							if (!printed) {
								if (showGrid) {
									addch(GRID_CHAR);
								}
								else {
									addch(' ');
								}
							}
						}
						addch(' '); // to make the horizontal match vertical
					}
			}
			addstr("\n");
		}
		printw("Press q to quit. Score: %d\n", tail_Length);
		refresh();
		usleep(FRAME_DELAY);
	}	
	endwin();
	printf("Final score: %d\n", tail_Length);
	return 0;
}
