#include <curses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "display.h"
#include "parsecommand.h"

int numOfSlides; // total of slides

int max_x; // ncurses character "res" x axis
int max_y; // ncurses character "res" y axis

short keyDigit1 = -1; // holds value of numerical inputs for
short keyDigit2 = -1; // slide jumping

short curColor = 1; // current color pair (theme)

bool quitting = false; // if true, end display loop

short openingFile = 0; // 0 - end program, 1 - open file, 2 - tabopen file

char *nextFile; // next file name to open


int bookmarkReg; // holds key to assign bookmark to

short bookmarks[5][2] = { // holds slide and register
    {-1, 0},
    {-1, 0},
    {-1, 0},
    {-1, 0},
    {-1, 0}
};

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
    // disable line buffering
    cbreak();
    // don't echo characters typed
    noecho();
    // disables cursor
    curs_set(0);
    clear();
}

void printMessageBottomBar(char message[256])
{
    // move to bottom line, clear to EOL, print, wait for input before returning
    move(max_y-1, 1);
    clrtoeol();
    printw(message);
    getch();
}

void handleCommandInput(int *slideNumber)
{
    command *comm = commandLoop(&max_y); // get the user command
    int i; // for loops
    switch(comm->cmd) {
        case 0: // bad input
            printMessageBottomBar("Error: Not a Recognized command");
            break;
        case 1: // quit
            quitting = true;
            break;
        case 2: // open file
            // todo: add error handling for missing file arg
            printMessageBottomBar("command 'open' not yet available");
            //quitting = true;
            //openingFile = 1;
            //nextFile = comm->arg[1];
            break;
        case 3: // bookmark current slide
            if (strlen(comm->arg[1])==0) {
                printMessageBottomBar("Missing register: type a key after bmark command");
                break;
            }
            for (i=0;i<5;i++) {
                if (bookmarks[i][0] == -1) {
                    bookmarks[i][0] = *slideNumber;
                    bookmarks[i][1] = comm->arg[1][0];
                    break;
                }
            }
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
            printMessageBottomBar("Bookmarks cleared");
            break;
        case 6: // jump to slide by number
            if (atoi(comm->arg[0]) > 0 && atoi(comm->arg[0]) <= numOfSlides)
            {
                *slideNumber = atoi(comm->arg[0])-1;
            }
            else
            {
                char message[80];
                sprintf(message, "Number provided is not a slide. Expected 1 - %i", numOfSlides);
                printMessageBottomBar(message);
            }
        default:
            break;
    }
}

void handleKeyPress(int *slideNumber)
{
    // get the keypress from user
    int keyInput = getch();
    int i = 0; // for loops
    switch(keyInput) {
        case 'q':
        case 'Q':
            quitting = true;
            break;
        case 'j':
        case 'J':
        case ' ':
            if (*slideNumber != numOfSlides-1) {
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
                    if (atoi(dest)<=numOfSlides) {
                        *slideNumber = atoi(dest);
                        keyDigit1 = -1;
                        keyDigit2 = -1;
                    }
                } else {
                    if (keyDigit1<=numOfSlides) {
                        *slideNumber = keyDigit1-1;
                        keyDigit1 = -1;
                    }
                }
            } else {
                *slideNumber = numOfSlides - 1;
            }
            break;
	    case 't': // change color if term has support
                if(has_colors()) {
                    curColor++;
                    if (curColor == 5)
                        curColor = 1;
                    wbkgd(stdscr, COLOR_PAIR(curColor));
                }
		break;
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
            bookmarkReg = getch();
            for (i=0;i<5;i++) {
                if (bookmarks[i][1] == bookmarkReg) {
                    *slideNumber = bookmarks[i][0];
                    break;
                }
            }
            break;
        case ':':
            handleCommandInput(slideNumber);
	        break;
        default:
            break;
    }
}

int displayLoop(slide slides[], int* slideNumber, int* slideCount, char* title, char* fileName)
{
    while(quitting == false) {
        numOfSlides = *slideCount;
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
        sprintf(bottomRightCounter, "slide %i / %i", slides[*slideNumber].number, numOfSlides);
        mvprintw(max_y-1, max_x-13, bottomRightCounter);

        // handle key presses
        handleKeyPress(slideNumber);

        // clear the screen for next loop to print
        clear();
    }

    // reset quitting, open a new file or close program
    quitting = false;
    if (openingFile == 1) {
        // set new file name to read
        fileName[0] = '\0';
        strcat(fileName, nextFile);
        // reset value for next iteration, return 1 to open a new file
        openingFile = 0;
        return 1;
    } else {
        // end the ncurses session, return 0 to cloe program
        endwin();
        return 0;
    }
}
