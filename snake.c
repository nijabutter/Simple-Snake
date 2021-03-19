#include <ncurses.h>
#include <stdlib.h>
#include <time.h>

#define FRUIT_CHAR ACS_PI
#define SNAKE_CHAR '0'
#define GRID_CHAR ACS_BULLET

int main()
{
	const int WIDTH = 20;
	const int HEIGHT = 20;

	// const int FRAME_DELAY = 72000; // microsecond, use usleep

    struct timespec delay;
    delay.tv_sec = 0;
    delay.tv_nsec = 72000000L; // nanoseconds

	srand(time(NULL));
	initscr();
	nodelay(stdscr, TRUE);
    keypad(stdscr, TRUE);
    curs_set(0);
	
	int SCREEN_WIDTH, SCREEN_HEIGHT;
	getmaxyx(stdscr, SCREEN_HEIGHT, SCREEN_WIDTH);

    int indent_left, indent_top;
	indent_left = (SCREEN_WIDTH / 2) - WIDTH; // width is technically doubled when printing
	indent_top = (SCREEN_HEIGHT - HEIGHT) / 2 - 1;

	if (has_colors() == FALSE) {
		endwin();
		printf("Sorry, your terminal does not support color.\n");
		exit(1);
	}

	use_default_colors();
	start_color();
	init_pair(1, COLOR_GREEN, -1);
	init_pair(2, COLOR_RED, -1);
	init_pair(3, COLOR_WHITE, -1);
	init_pair(4, COLOR_BLACK, -1);

  	int snake_X = WIDTH/2;
	int snake_Y= HEIGHT/2;
	int tail_Length = 3;

	int fruit_X = rand() % WIDTH;
	int fruit_Y = rand() % HEIGHT;
	
	int tailX[WIDTH*HEIGHT], tailY[WIDTH*HEIGHT];
  
  // set the position of the rest of the body to be behind the head
	for (int t = 0; t < tail_Length; t++) { 
		tailX[t] = snake_X-t-1;
		tailY[t] = snake_Y;
	}

	enum eDirection { STOP = 0, LEFT, RIGHT, UP, DOWN };
	enum eDirection direction = STOP;
	
	bool running = true;
	bool showGrid = false;

	while (running) {

		switch(getch()) {
			case 'q':
				running = false;
				break;
			case KEY_UP:
			case 'w':
				if (direction != DOWN)
					direction = UP;
				break;
			case KEY_LEFT:
			case 'a':
				if (direction != RIGHT && direction != STOP)
					direction = LEFT;
				break;
			case KEY_DOWN:
			case 's':
				if (direction != UP)
					direction = DOWN;
				break;
			case KEY_RIGHT:
			case 'd':
				if (direction != LEFT)
					direction = RIGHT;
				break;
			case ' ':
				showGrid = !showGrid;
				break;
            case KEY_RESIZE: 
                getmaxyx(stdscr, SCREEN_HEIGHT, SCREEN_WIDTH);
                indent_left = (SCREEN_WIDTH / 2) - WIDTH;
                indent_top = (SCREEN_HEIGHT - HEIGHT) / 2 -1;
                break;
			default:
				break;
		}

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
		if (direction != STOP) {
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
		
		erase();
        
		move(indent_top ,indent_left);

		// top border
        addch(ACS_ULCORNER);
        for (int i = 0; i < WIDTH; i++) {
            addch(ACS_HLINE); 
            addch(ACS_HLINE); // need two because of adding spaces after each char
        }
        addch(ACS_URCORNER);


		for (int y = 0; y < HEIGHT; y++) {

			move(indent_top+y+1, indent_left);
            addch(ACS_VLINE); // left border
            for (int x = 0; x < WIDTH; x++) {
                if (y == snake_Y && x == snake_X) {
                    // the snake head
                    switch (direction) {
                        case UP:
                        addch('^');
                        break;
                        case DOWN:
                        addch('v');
                        break;
                        case LEFT:
                        addch('<');
                        break;
                        case RIGHT:
                        addch('>');
                        break;
                        default:
                        addch('>'); // stopped
                        break;
                    }
                }
                else if (y == fruit_Y && x == fruit_X) {
                    // fruit
                    attron(COLOR_PAIR(2));
                    addch(FRUIT_CHAR);
                    attroff(COLOR_PAIR(2));
                }
                else {
                    bool printed = false; // to know if we printed a snake body or if it is empty
                    for (int t = 0; t < tail_Length; t++) {
                        // check each part of the snake
                        if (snake_Y == tailY[t] && snake_X == tailX[t]) {
                            // snake body has hit head - game over
                            running = false;
                            break;
                        }
                        if (tailY[t] == y && tailX[t] == x) {
                    		attron(COLOR_PAIR(1));
                            addch(SNAKE_CHAR);
                    		attroff(COLOR_PAIR(1));
                            printed = true; // printed a body part - dont print a gap or dot
                            break;
                        }
                    }
                    if (!printed) { // gap/dot
                        if (showGrid) {
                            attron(COLOR_PAIR(4));
                            addch(GRID_CHAR);
                            attroff(COLOR_PAIR(4));
                        }
                        else {
                            addch(' ');
                        }
                    }
                }
                addch(' '); // to stop things looking squished
            }
			addch(ACS_VLINE);
		}

		move(indent_top+HEIGHT+1, indent_left);
		// bottom border
		addch(ACS_LLCORNER);
		for (int i = 0; i < WIDTH; i++) {
			addch(ACS_HLINE);
			addch(ACS_HLINE);
		}
		addch(ACS_LRCORNER);

		move(indent_top+HEIGHT+2, indent_left);
		printw("Press q to quit");
		refresh();

		/* usleep(FRAME_DELAY); */
        nanosleep(&delay, NULL);
	}	
	endwin();
	printf("Final score: %d\n", tail_Length);
	return 0;
}
