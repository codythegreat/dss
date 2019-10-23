#include <curses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "display.h"

// holds value of last pressed key
char prevPress[2] = "";
short numberInMem = 0;
char numberInput[4] = "";
int slideCount;

void setSlideCount(int* slides)
{
    slideCount = *slides;
}

void initDisplay()
{
    // initialize ncurses
    initscr();
    cbreak();
    noecho();
    clear();
}

void closeDisplay()
{
    // end ncurses session
    endwin();
    exit(0);
}


void displayLoop(Slide *slide, int* slideNumber, char* title)
{
    printw(title);
    printw("\n");
    printw(slide->content); //to be used when parser.c implemented
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
        //case '9':
        //case '8':
        //case '7':
        //case '6':
        //case '5':
        //case '4':
        //case '3':
        //case '2':
        //case '1':
	    //if (!(keyInput > slideCount)) {
                //*slideNumber = keyInput - 1;
	    //}
	    //break;
        case 'g':
	    *slideNumber = 0;
	    break;
        case 'G':
	    *slideNumber = slideCount - 1;
	    break;
        default:
	    break;
    }
    clear();
}