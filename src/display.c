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
short curColor = 1;
short quitting = 0; // if 1, return to main

// register for bookmarks
int reg;
// holds slide and register
short bookmarks[5][2] = {
    {-1, 0},
    {-1, 0},
    {-1, 0},
    {-1, 0},
    {-1, 0}
};

void setSlideCount(int* slides)
{
    slideCount = *slides;
}

void initDisplay()
{
    // initialize ncurses
    initscr();
    // if term supports color, enable
    if (has_colors()) {
        start_color();
        init_pair(1, COLOR_WHITE, COLOR_BLACK);
        init_pair(2, COLOR_GREEN, COLOR_YELLOW);
        init_pair(3, COLOR_WHITE, COLOR_BLUE);
        init_pair(4, COLOR_BLACK, COLOR_WHITE);
    }
    cbreak();
    noecho();
    // disables cursor
    curs_set(0);
    clear();
}

void handleKeyPress(int *slideNumber)
{
    // get the keypress from user
    int keyInput = getch();
    int i = 0; // for loops
    switch(keyInput) {
        case 'q':
        case 'Q':
		    quitting = 1;
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
                if (keyDigit2 >= 0) {
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
	    case 't':
            if(has_colors()) {
                curColor++;
                if (curColor == 5) {
                    curColor = 1;
                }
                wbkgd(stdscr, COLOR_PAIR(curColor));
            }
        case 'b':
            for (i=0;i<5;i++) {
                if (bookmarks[i][0] == -1) {
                    bookmarks[i][0] = *slideNumber;
                    bookmarks[i][1] = getch();
                    break;
                }
            }
            break;
        case 'B':
            reg = getch();
            for (i=0;i<5;i++) {
                if (bookmarks[i][1] == reg) {
                    *slideNumber = bookmarks[i][0];
                    break;
                }
            }
            break;
        default:
            break;
    }
}

void displayLoop(slide slides[], int* slideNumber, char* title, char* fileName)
{
    initDisplay();
    while(quitting == 0) {
        // assigns screen x/y length continually (incase of screen resize)
        getmaxyx(stdscr, max_y, max_x);
	    // if the screen is too small/zoomed in, dispay a soft error
	    if (slides[*slideNumber].maxX> max_x-1 || slides[*slideNumber].y > max_y-3) {
            printw("terminal size/zoom error: Please resize or zoom out the terminal to display the slide.");
	    } else {
            move(0,1);
            printw(title);
            printw("\n");
            // move the cursor for vertical centering
            move(((max_y-slides[*slideNumber].y)/2), 0);
            // print all lines in slide
            line *curLine = slides[*slideNumber].first;
            // find the number of spaces required to center a slide, initialize and fill string of spaces
            int spaces = (max_x - slides[*slideNumber].maxX) / 2;
            char spaceStr[spaces];
            spaceStr[0] = '\0';
            sprintf(spaceStr, "%s%*s", spaceStr, spaces, "");
            // print spaces, line, and move to next line
            while(curLine) {
                printw(spaceStr);
                printw(curLine->content);
                line *temp = curLine->next;
                curLine = temp;
           }
	    }

	    // print bottom bar to screen
        mvprintw(max_y-1, 1, fileName);
        char bottomRightCounter[14];
        sprintf(bottomRightCounter, "slide %i / %i", slides[*slideNumber].number, slideCount);
        mvprintw(max_y-1, max_x-13, bottomRightCounter);

        // handle key presses
        handleKeyPress(slideNumber);

        // clear the screen for next loop to print
        clear();
    }

    // when quitting, end the ncurses session
    endwin();
}
