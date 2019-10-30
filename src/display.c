#include <curses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "display.h"
#include "parsecommand.h"

int slideCount;
int max_x;
int max_y;
short keyDigit1 = -1;
short keyDigit2 = -1;
short curColor = 1;
bool quitting = false;
short exitCode = 0; // 0 - end program, 1 - open file, 2 - tabopen file
char *nextFile;

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

// todo: this should be a part of the main loop
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
    command *comm; // ':' case
    switch(keyInput) {
        case 'q':
        case 'Q':
            quitting = true;
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
                    if (keyDigit2 == 0) {                               // todo: more elegant solution for the below sprintf calls
                        sprintf(dest, "%i%i", keyDigit1-1, 9);          // compiler will warn here due to length of string. safe to ignore.
                    } else {
                        sprintf(dest, "%i%i", keyDigit1, keyDigit2-1);  // compiler will warn here due to length of string. safe to ignore.
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
        case ':':
	        comm = commandLoop(&max_y);
            switch(comm->cmd) {
                case 0: // bad input
                    move(max_y-1, 1);
                    clrtoeol();           // clear the line for printing
                    printw("Error: Not a Recognized command");
                    getch();
                    break;
                case 1: // quit
                    quitting = true;
                    break;
                case 2: // open file
                    move(max_y-1, 1);
                    clrtoeol();           // clear the line for printing
                    printw("command 'open' not yet available");
                    getch();
                    //quitting = true;
                    //exitCode = 1;
                    //nextFile = comm->arg[1];
                    break;
                case 3: // tab open file
                    quitting = true;
                    exitCode = 2;
                    // todo: add logic for tab opening new file
                    break;
                case 4: // prints bookmarks
                    move(max_y-6, 0);
                    for (i = 0; i < 5; i++) {
                        if (bookmarks[i][0] != -1)
                        {
                            printw(" slide %i - register %c\n", bookmarks[i][0]+1, bookmarks[i][1]);
                        } else 
                        {
                            printw(" empty\n", i+1);
                        }
                    }
                    getch();
                    break;
                case 5: // clears bookmarks
                    for (i = 0; i < 5; i++) {
                        bookmarks[i][0] = -1;
                        bookmarks[i][1] = 0;
                    }
                    move(max_y-1, 1);
                    // clear the line for printing
                    clrtoeol(); 
                    printw("Bookmarks cleared");
                    getch();
                    break;
                case 6: // jump to slide by number
		    if (atoi(comm->arg[0]) > 0 && atoi(comm->arg[0]) <= slideCount)
		    {
                        *slideNumber = atoi(comm->arg[0])-1;
		    }
		    else
		    {
                        move(max_y-1, 1);
                        // clear the line for printing
                        clrtoeol(); 
                        printw("Number provided is not a slide. Expected 1 - %i", slideCount);
                        getch();
		    }
                default:
                    break;
                }
	            break;
        default:
            break;
    }
}

int displayLoop(slide slides[], int* slideNumber, char* title, char* fileName)
{
    while(quitting == false) {
        // assigns screen x/y length continually (incase of screen resize)
        getmaxyx(stdscr, max_y, max_x);
	    // if the screen is too small/zoomed in, dispay a soft error
	    if (slides[*slideNumber].maxX> max_x-1 || slides[*slideNumber].y > max_y-3) {
            printw("terminal size/zoom error: Please resize or zoom out the terminal to display the slide.");
	    } else {
            move(0,1);
            printw(title);
            printw("\n");

            // for each line, move to the centered position and print the line
            int yPosition = (max_y-slides[*slideNumber].y)/2;
            line *currentLine = slides[*slideNumber].first;
            int i = 0;
            while(currentLine) {
                move(yPosition + i, (max_x - slides[*slideNumber].maxX) / 2);
                printw(currentLine->content);
                line *temp = currentLine->next;
                currentLine = temp;
                i++;
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

    // when quitting, end the ncurses session or set next file name
    if (exitCode == 1) {
        fileName[0] = '\0';
        strcat(fileName, nextFile);
    } else {
        endwin();
    }
    return exitCode;
}
