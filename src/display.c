#include <curses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "display.h"

int slideCount;
int max_x;
int max_y;
short keyDigit1 = -1;
short keyDigit2 = -1;

void setSlideCount(int* slides)
{
    slideCount = *slides;
}

void initDisplay()
{
    // initialize ncurses
    initscr();
    // if term supports color, enable
    if (has_color()) {
        start_color();
    }
    cbreak();
    noecho();
    // disables cursor
    curs_set(0);
    clear();
}

void closeDisplay()
{
    // end ncurses session
    endwin();
    exit(0);
}


void displayLoop(Slide *slide, int* slideNumber, char* title, char* fileName)
{
    // assigns screen x/y length continually (incase of screen resize)
    getmaxyx(stdscr, max_y, max_x);
    printw(title);
    printw("\n");
    printw(slide->content); //to be used when parser.c implemented
    mvprintw(max_y-1, 1, fileName);
    char bottomRightCounter[15];
    sprintf(bottomRightCounter, "slide %i / %i", slide->number, slideCount);
    mvprintw(max_y-1, max_x-15, bottomRightCounter);
    int keyInput = getch();
    switch(keyInput) {
        case 'q':
        case 'Q':
            closeDisplay();
            break;
        case 'j':
        case 'J':
        case ' ':
            if (*slideNumber != slideCount-1) {
                *slideNumber = *slideNumber + 1;
            }
            break;
        case 'k':
        case 'K':
            if (*slideNumber != 0) {
                *slideNumber = *slideNumber - 1;
            }
            break;
        case '9':
        case '8':
        case '7':
        case '6':
        case '5':
        case '4':
        case '3':
        case '2':
        case '1':
        case '0':
            if (keyDigit1 >= 1 && keyDigit2 >= 0) {
                keyDigit1 = keyInput - 48;
                keyDigit2 = -1;
            } else if (keyDigit1 == -1 && keyInput != '0') {
                keyDigit1 = keyInput - 48;
            } else if (keyDigit2 == -1) {
                keyDigit2 = keyInput - 48;
            }
            break;
        case 'g':
            *slideNumber = 0;
            break;
        case 'G':
            if (keyDigit1 >= 0) {
                if (keyDigit2 >= 0) { // todo: look into making sure 2 digits slides are found correctly
                    char dest[3];
                    if (keyDigit2 == 0) {
                        sprintf(dest, "%i%i", keyDigit1-1, 9);
                    } else {
                        sprintf(dest, "%i%i", keyDigit1, keyDigit2-1);
                    }
                    if (atoi(dest)<=slideCount) {
                        *slideNumber = atoi(dest);
                        keyDigit1 = -1;
                        keyDigit2 = -1;
                    }
                } else {
                    if (keyDigit1<=slideCount) {
                        *slideNumber = keyDigit1-1;
                        keyDigit1 = -1;
                    }
                }
            } else {
                *slideNumber = slideCount - 1;
            }
            break;
	// case 't':
	    // if (curColTheme == 2) {
		// curColTheme = 0;
		// if (COLORS == 256) {
		    // int black = 16;
		    // init_pair(curColTheme, foreGCols[curColTheme], black);
		// } else {
                    // int black = 0;
		    // init_pair();
		// }	
		// curColTheme++;
	    // }
	    // break;
        default:
            break;
    }
    clear();
}
