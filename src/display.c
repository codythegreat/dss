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

int doubleSlideDisplayMode = 0; // used for displaying two slides at once

char lastCommand[1000] = {'\0'}; // stores last user inputed command

char lastSearchTerm[1000] = {'\0'}; // stores last used search for n/N searching
int FORWARD = 1; // n
int BACKWARD = 0; // N


int bookmarkReg; // holds key to assign bookmark to

short bookmarks[128] = {-1}; // each index represents an ascii key

void clearBookmarks() {
    int i;
    for (i = 0; i < 128; i++) {
        bookmarks[i] = -1;
    }
}

void initDisplay()
{
    // initialize ncurses
    initscr();
    // if term supports color, enable
    if (has_colors()) {
        start_color();
        init_pair(1, COLOR_WHITE, COLOR_BLACK);
        init_pair(2, COLOR_MAGENTA, COLOR_WHITE);
        init_pair(3, COLOR_WHITE, COLOR_BLUE);
        init_pair(4, COLOR_BLACK, COLOR_WHITE);
        init_pair(5, COLOR_BLACK, COLOR_CYAN);
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
    move(max_y-1, 0);
    clrtoeol();
    printw(message);
}

void printSlideNumberInputError() {
    char message[80];
    sprintf(message, "Number provided is not a slide. Expected 1 - %i", numOfSlides);
    printMessageBottomBar(message);
    getch();
}

void printSingleSlideView(slide *curSlide) {
    // for each line, move to the centered position and print the line
    int yPosition = (max_y-curSlide->y)/2;
    line *currentLine = curSlide->first;
    int i = 0;
    while(currentLine) {
        move(yPosition + i, (max_x - curSlide->maxX) / 2);
        printw(currentLine->content);
        line *temp = currentLine->next;
        currentLine = temp;
        i++;
   }
}

// prints two slides centered on each half of the screen
void printDoubleSlideView(slide *curSlide) {
    // get the starting y position
    int yPosition = (max_y-curSlide->y)/2;
    // start with the first line and print 
    line *currentLine = curSlide->first;
    int i = 0;
    while(currentLine) {
        move(yPosition + i, ((max_x/2) - curSlide->maxX) / 2);
        printw(currentLine->content);
        line *temp = currentLine->next;
        currentLine = temp;
        i++;
    }
    // reset y position, i and currentLine for next slide
    yPosition = (max_y - curSlide->next->y)/2;
    i = 0;
    currentLine = curSlide->next->first;
    // print the next slide
    while(currentLine) {
        move(yPosition + i, ((max_x/2) + ((max_x/2) - curSlide->next->maxX) / 2)); 
	printw(currentLine->content);
        line *temp = currentLine->next;
        currentLine = temp;
        i++;
    }
}

slide* handleCommand(slide *curSlide)
{
    // if lastCommand is empty, return
    if (lastCommand[0] == '\0') {
        return curSlide;
    }

    // parse lastCommand into an instance of command struct
    command *comm = parseCommand(lastCommand);
    int i; // for loops
    switch(comm->cmd) {
        case 0: // bad input
            printMessageBottomBar("Error: Not a Recognized command");
            getch();
            break;
        case 1: // quit
            quitting = true;
            break;
        case 2: // open file
	    if (strlen(comm->arg[1])>0) {
	        clearBookmarks();
                quitting = true;
                openingFile = 1;
                nextFile = comm->arg[1];
	    } else {
                printMessageBottomBar("Missing file argument");
		getch();
	    }
            break;
        case 3: // bookmark current slide
            if (strlen(comm->arg[1])==0) {
                printMessageBottomBar("Missing register: type a key after bmark command");
                getch();
                break;
            }
	    bookmarks[comm->arg[1][0]] = curSlide->number;
            break;
        case 4: // prints bookmarks
            move(2, 0);
            for (i = 0; i < 128; i++) {
                if (bookmarks[i] >= 1)
                {
                    printw("slide %i - register %c\n", bookmarks[i], i);
                }
            }
            getch();
            break;
        case 5: // clears bookmarks
	    clearBookmarks();
            printMessageBottomBar("Bookmarks cleared");
            getch();
            break;
        case 6: // jump to slide by number
            if (atoi(comm->arg[0]) > 0 && atoi(comm->arg[0]) <= numOfSlides)
            {
                while (curSlide->number!= atoi(comm->arg[0])) {
                    if (curSlide->number < atoi(comm->arg[0])) {
                        curSlide = curSlide->next;
                    } else {
                        curSlide = curSlide->prev;
                    }
                }
            }
            else
            {
                printSlideNumberInputError();
            }
            break;
	// todo: command to display meta information
	// todo: command to enable double mode
	// todo: command to enable markdown mode
        default:
            break;
    }
    return curSlide;
}

// prints input to the bottom of the screen, each
// char is appended to tmp, when enter is 
// pressed, tmp is assigned to the buffer
int parseUserInput(char *modeChar, char buffer[1000]) {
    printMessageBottomBar(modeChar);
    //enable cursor
    curs_set(1);
    
    // search criteria that the user inputs
    char tmp[1000];
    tmp[0] = '\0';
    // tracks character for keyPress to search
    int currentCharacter = 0;

    char keyPress;
    bool typing = true;
    while (typing)
    {
        keyPress = getch();
	    switch(keyPress)
	    {
            case '\n':  // enter
                typing = false;
                break;
            case 27:    // esc
		return 0;
                break;
            case 127:   // back space  todo: test key input on mult systems
                if (currentCharacter != 0) {
                    // move back and delete character in place
                    mvdelch(max_y-1, 1+currentCharacter);
                    currentCharacter--;
	            }
                break;
            default:
                // print the character to the screen
                addch(keyPress);
                // add the character to our userIn buffer
                tmp[currentCharacter] = keyPress;
                tmp[currentCharacter+1] = '\0';
                currentCharacter++;
    	        break;
	    }
    }
    // hide cursor
    curs_set(0);

    // if no text provided, return
    if (tmp[0]=='\0') {
        printMessageBottomBar("Missing Argument");
        getch();
        return 0;
    }
    // buffer is cleared and tmp is concatenated to it
    buffer[0] = '\0';
    strcat(buffer, tmp);
    return 1;
}

slide* searchLastInput(int direction, slide* curSlide) {
    // if lastSearchTerm empty, print error and return
    if (lastSearchTerm[0] == '\0') {
        printMessageBottomBar("Error: initialize a search with the '/' key");
        getch();
        return curSlide;
    }
    //initialize variables if not on last slide
    slide *searching;
    line *toSearch;
    if (direction==1) {
        if (!curSlide->next==NULL) {
            searching = curSlide->next;
            toSearch = searching->first;
        } else {
            printMessageBottomBar("You are already at last slide.");
            getch();
            return curSlide;
        }
    } else {
        if (!curSlide->prev==NULL) {
            searching = curSlide->prev;
            toSearch = searching->first;
        } else {
            printMessageBottomBar("You are already at first slide.");
            getch();
            return curSlide;
        }
    }

    // tracks if search has been found
    bool found = false;
    while(!found) {
        // if on last line, get next slide and first line
        if (toSearch==NULL) {
            if (direction==1) {
                searching = searching->next;
            } else {
                searching = searching->prev;
            }
            if (searching == NULL) {
                printMessageBottomBar("No result found");
                getch();
                return curSlide;
            }
            toSearch = searching->first;
        }

        if (strstr(toSearch->content, lastSearchTerm)!=NULL) {
            found = true;
        } else {
            toSearch = toSearch->next;
        }
    }
    // return slide that match appears on
    return searching;
}

slide* handleKeyPress(slide *curSlide)
{
    // get the keypress from user
    int keyInput = getch();
    int i = 0; // for loops
    switch(keyInput) {
        case 'q':
        case 'Q': // end the session
            quitting = true;
            break;
        case 'j':
        case 'J':
        case ' ': // next slide
            if (!curSlide->next==NULL)
                curSlide = curSlide->next;
            break;
        case 'k':
        case 'K': // prev slide
            if (!curSlide->prev==NULL)
                curSlide = curSlide->prev;
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
        case '0': // digits are recorded and used later with 'G' to enable slide jumping
            if (keyDigit1 >= 1 && keyDigit2 >= 0) {
                keyDigit1 = keyInput - 48;
                keyDigit2 = -1;
            } else if (keyDigit1 == -1 && keyInput != '0') {
                keyDigit1 = keyInput - 48;
            } else if (keyDigit2 == -1) {
                keyDigit2 = keyInput - 48;
            }
            break;
        case 'g': // go to first slide
            while (curSlide->number!=1) {
                curSlide = curSlide->prev;
            }
            break;
        case 'G': // go to slide specified from prior input, or end
            if (keyDigit1 >= 0) {
                if (keyDigit2 >= 0) {
                    char dest[3];
                    sprintf(dest, "%i%i", keyDigit1, keyDigit2);  // compiler will warn here due to length of string. safe to ignore.
                    if (atoi(dest)<=numOfSlides) {
                        while (curSlide->number!=atoi(dest)) {
                            if (curSlide->number < atoi(dest)) {
                                curSlide = curSlide->next;
                            } else {
                                curSlide = curSlide->prev;
                            }
                        }
                        keyDigit1 = -1;
                        keyDigit2 = -1;
                    } else {
                        printSlideNumberInputError();
		    }
                } else {
                    if (keyDigit1<=numOfSlides) {
                        while (curSlide->number!=keyDigit1) {
                            if (curSlide->number < keyDigit1) {
                                curSlide = curSlide->next;
                            } else {
                                curSlide = curSlide->prev;
                            }
                        }
                        keyDigit1 = -1;
                    } else {
                        printSlideNumberInputError();
		    }
                }
            } else {
                while (curSlide->number!=(numOfSlides)) {
                    curSlide = curSlide->next;
                }
            }
            break;
	    case 't': // change color if term has support
                if(has_colors()) {
                    curColor++;
                    if (curColor == 6)
                        curColor = 1;
                    wbkgd(stdscr, COLOR_PAIR(curColor));
                }
		break;
        case 'b': // set a bookmark on cur slide
	    bookmarkReg = getch();
	    bookmarks[bookmarkReg] = curSlide->number;
            break;
        case 'B': // go to bookmark at following register
            bookmarkReg = getch();
	    if (bookmarks[bookmarkReg] >= 1) {
                while (curSlide->number != bookmarks[bookmarkReg]) {
                    if (curSlide->number < bookmarks[bookmarkReg]) {
                        curSlide = curSlide->next;
                    } else {
                        curSlide = curSlide->prev;
                    }
		}
	    } else {
		printMessageBottomBar("Not a registered bookmark");
		getch();
	    }
            break;
        case 'd':
	    if (doubleSlideDisplayMode==1) {
                doubleSlideDisplayMode = 0;
	    } else {
                doubleSlideDisplayMode = 1;
	    }
	    break;
        case ':': // parse user input and execute inputted command
	    if (parseUserInput(":", lastCommand)) {
                curSlide = handleCommand(curSlide);
	    }
            break;
        case '/': // parse user input and perform a forward search
            if (parseUserInput("/", lastSearchTerm)) {
                curSlide = searchLastInput(FORWARD, curSlide);
	    }
            break;
        case 'n': // forward search last input
            curSlide = searchLastInput(FORWARD, curSlide);
            break;
        case 'N': // backward search last input
            curSlide = searchLastInput(BACKWARD, curSlide);
            break;
        default:
            break;
    }
    return curSlide;
}

int displayLoop(slide *curSlide, int* slideCount, char* title, char* fileName)
{
    while(quitting == false) {
        numOfSlides = *slideCount;

        // assigns screen x/y length continually (incase of screen resize)
        getmaxyx(stdscr, max_y, max_x);

	// print the title if terminal is big enough
	if (strlen(title)>max_x) {
            printw("Terminal too small for title");
	} else {
            printw(title);
            printw("\n");
	}

	// if the screen is too small/zoomed in, dispay a soft error
	// otherwise, print the slide(s)
	if (doubleSlideDisplayMode==0) {
	    if (curSlide->maxX> max_x-1 || curSlide->y > max_y-3) {
                printw("terminal size/zoom error: Please resize or zoom out the terminal to display the slide.");
	    } else {
                printSingleSlideView(curSlide);
	    }
	} else {
            // always stay one slide less than numOfSlides so that two slides can be printed
            if (curSlide->number == numOfSlides) {
                curSlide = curSlide->prev;
	    }
	    // todo: if statement is ugle
	    if (curSlide->maxX + curSlide->next->maxX > max_x-2 || curSlide->y + curSlide->next->y > (max_y*2)-3) {
                printw("terminal size/zoom error: Please resize or zoom out the terminal to display the slide.");
	    } else {
                printDoubleSlideView(curSlide);
	    }
	}
	
	// print bottom bar to screen
        mvprintw(max_y-1, 0, fileName);
        char bottomRightCounter[20];
	if (!doubleSlideDisplayMode) {
            sprintf(bottomRightCounter, "%i / %i", curSlide->number, numOfSlides);
	} else {
            sprintf(bottomRightCounter, "%i-%i / %i", curSlide->number,curSlide->number+1, numOfSlides);
	}
        mvprintw(max_y-1, max_x-strlen(bottomRightCounter), bottomRightCounter);

        // handle key presses
        curSlide = handleKeyPress(curSlide);

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
