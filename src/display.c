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

char slideNumberInputBuffer[10] = {'\0'}; // buffer for jumping to slides by number
int destination; // stores converted slide number for jumping

short curColor = 7; // current color pair (theme)

bool quitting = false; // if true, end display loop

short openingFile = 0; // 0 - end program, 1 - open file, 2 - tabopen file

char *nextFile; // next file name to open

int doubleSlideDisplayMode = 0; // used for displaying two slides at once

int lineColorMode = 1; // 1 - enable color tags, 0 - disable  changes with :nocolor

char lastCommand[1000] = {'\0'}; // stores last user inputed command

char lastSearchTerm[1000] = {'\0'}; // stores last used search for n/N searching
int FORWARD = 1; // n
int BACKWARD = 0; // N

int bookmarkReg; // holds key to assign bookmark to
short bookmarks[128] = {-1}; // each index represents an ascii key

// TODO: can this be handled better?
const int FGBGCOLORPAIRS[56][2] = {
    {1, 0},
    {2, 0},
    {3, 0},
    {4, 0},
    {5, 0},
    {6, 0},
    {7, 0},
    {0, 1},
    {2, 1},
    {3, 1},
    {4, 1},
    {5, 1},
    {6, 1},
    {7, 1},
    {0, 2},
    {1, 2},
    {3, 2},
    {4, 2},
    {5, 2},
    {6, 2},
    {7, 2},
    {0, 3},
    {1, 3},
    {2, 3},
    {4, 3},
    {5, 3},
    {6, 3},
    {7, 3},
    {0, 4},
    {1, 4},
    {2, 4},
    {3, 4},
    {5, 4},
    {6, 4},
    {7, 4},
    {0, 5},
    {1, 5},
    {2, 5},
    {3, 5},
    {4, 5},
    {6, 5},
    {7, 5},
    {0, 6},
    {1, 6},
    {2, 6},
    {3, 6},
    {4, 6},
    {5, 6},
    {7, 6},
    {0, 7},
    {1, 7},
    {2, 7},
    {3, 7},
    {4, 7},
    {5, 7},
    {6, 7}
};

// keep the same background color, change foreground
void incrememntFGColor() {
        int currentBG = FGBGCOLORPAIRS[curColor-1][1];
        do {
            curColor++;
            if (curColor == 57) {
                curColor = 1;
            }
        } while (FGBGCOLORPAIRS[curColor-1][1] != currentBG);
        // sets the screen to the new color pair 
        wbkgd(stdscr, COLOR_PAIR(curColor));
}

// keep the same foreground color, change background
void incrememntBGColor() {
        int currentFG = FGBGCOLORPAIRS[curColor-1][0];
        do {
            curColor++;
            if (curColor == 57) {
                curColor = 1;
            }
        } while (FGBGCOLORPAIRS[curColor-1][0] != currentFG);
        // sets the screen to the new color pair 
        wbkgd(stdscr, COLOR_PAIR(curColor));
}

void initColors() {
    int i;
    for (i=0;i<56;i++) {
        init_pair(i+1, FGBGCOLORPAIRS[i][0], FGBGCOLORPAIRS[i][1]);
    }
}

void initDisplay()
{
    // initialize ncurses
    initscr();
    // if term supports color, enable
    if (has_colors()) {
        start_color();
        initColors();
    }
    // disable line buffering
    cbreak();
    // don't echo characters typed
    noecho();
    // disables cursor
    curs_set(0);
    clear();
}

void printSlideAtPosition(int x, int y, slide *printing) {
    line *currentLine = printing->first;
    int i = 0;
    int spaces; // TODO: should this be size_t? compiler gives warning
    while(currentLine) {
        // move to start position of printing
        move(y + i, x);

        // if line has a color value, switch to that color
        if (lineColorMode && currentLine->colorPair!=0) {
            attron(COLOR_PAIR(currentLine->colorPair));
        }

        // print the line
        printw(currentLine->content);

        // add spaces to EOL up to len of longest line
        for (spaces=0;spaces<(int)(printing->maxX-strlen(currentLine->content));spaces++) {
            printw(" ");
        }

        // switch back to global color
        attron(COLOR_PAIR(curColor));

        // increment to the next line
        line *temp = currentLine->next;
        currentLine = temp;

        i++;
    }
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

// todo: can this be more memory efficient?
void addKeyToNumericalBuffer(int key, char buffer[]) {
    char c[10];
    sprintf(c, "%d", key-48);
    strcat(buffer, c);
}

int parseSlideNumberInput(char buffer[]) {
    if (atoi(buffer) > 0 && atoi(buffer) <= numOfSlides) {
        return atoi(buffer);
    } else {
        return 0;
    }
}

slide* findSlideAtNumber(int number, slide *curSlide) {
    if (number > numOfSlides || number < 1) {
        return curSlide;
    }

    while (curSlide->number!= number) {
        if (curSlide->number < number) {
            curSlide = curSlide->next;
        } else {
            curSlide = curSlide->prev;
        }
    }
    return curSlide;
}

void clearBookmarks() {
    int i;
    for (i = 0; i < 128; i++) {
        bookmarks[i] = -1;
    }
}

slide* jumpToSlideAtBookmark(int key, slide *curSlide) {
    if (bookmarks[key] >= 1) {
        curSlide = findSlideAtNumber(bookmarks[key], curSlide);
    } else {
        printMessageBottomBar("Not a registered bookmark");
        getch();
    }
    return curSlide;
}

void printLinksOnSlide(slide *curSlide) {
    mdlink *l = curSlide->link;
    if (!l) {
        printMessageBottomBar("No links detected on current slide");
        return;
    }
    int i = 2; //start line to print at
    while (l) {
        move(i, 0);
        printw("%d - %s",l->index, l->url);
        l = l->next;
        i++;
    }
}

void openLinkAtIndex(int index, slide *curSlide) {
    mdlink *l = curSlide->link;
    while (l) {
        if (index == l->index) {
            // build a command that will open link in def browser
            char systemCommand[1000] = {0};
            strcat(systemCommand, "xdg-open ");
            strcat(systemCommand, l->url);
            // send any output to /dev/null (instead of stdout)
            strcat(systemCommand, " >/dev/null 2>&1");
            // if the command fails, display a soft error
            system(systemCommand);
            return;
        } else {
            l = l->next;
        }
    }
    printMessageBottomBar("index given does not contain a link");
    getch();
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
            bookmarks[(int)comm->arg[1][0]] = curSlide->number;
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
            destination = parseSlideNumberInput(comm->arg[0]);
            if (destination) {
                curSlide = findSlideAtNumber(destination, curSlide);
            }
            else
            {
                printSlideNumberInputError();
            }
            break;
        case 7:
            curSlide = jumpToSlideAtBookmark((int)comm->arg[0][0], curSlide);
            break;
        case 8: // toggle double slide mode
                if (!doubleSlideDisplayMode && numOfSlides>1)
                    doubleSlideDisplayMode = 1;
                else
                doubleSlideDisplayMode = 0;
                break;
        case 9: // list links on a slide
            if (doubleSlideDisplayMode) {
                printMessageBottomBar("Links not supported in double slide display mode");
            } else {
                printLinksOnSlide(curSlide);
            }
            getch();
            break;
        case 10: // open link at index
            if (doubleSlideDisplayMode) {
                printMessageBottomBar("Links not supported in double slide display mode");
                getch();
            } else {
                if (atoi(comm->arg[1]) && curSlide->link!=NULL) {
                    openLinkAtIndex(atoi(comm->arg[1]), curSlide);
                } else {
                    printMessageBottomBar("No links detected on this slide");
                    getch();
                }
            }
            break;
        case 11: // toggle line coloring via colortags
            if (lineColorMode) {
                lineColorMode=0;
            } else {
                lineColorMode=1;
            }
            break;
        // todo: command to display meta information
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
                curs_set(0);
                return 0;
                break;
            case 127:   // back space  todo: test key input on mult systems
                if (currentCharacter != 0) {
                    // move back and delete character in place
                    mvdelch(max_y-1, currentCharacter);
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
            addKeyToNumericalBuffer(keyInput, slideNumberInputBuffer);
            break;
        case 'g': // go to first slide
            while (curSlide->number!=1) {
                curSlide = curSlide->prev;
            }
            break;
        case 'G': // go to slide specified from prior input, or last slide
            if (strlen(slideNumberInputBuffer)>0) {
                destination = parseSlideNumberInput(slideNumberInputBuffer);
                if (destination) {
                    curSlide = findSlideAtNumber(destination, curSlide);
                    slideNumberInputBuffer[0] = '\0';
                } else {
                    printSlideNumberInputError();
                }
            } else {
                while (curSlide->number!=(numOfSlides)) {
                    curSlide = curSlide->next;
                }
            }
            break;
        case 't': // change color if term has support
            if(has_colors()) {
                incrememntFGColor();
            }
            break;
        case 'T': // next background color
            if(has_colors()) {
                incrememntBGColor();
            }
            break;
        case 'b': // set a bookmark on cur slide
            bookmarkReg = getch();
            bookmarks[bookmarkReg] = curSlide->number;
            break;
        case 'B': // go to bookmark at following register
            bookmarkReg = getch();
            curSlide = jumpToSlideAtBookmark(bookmarkReg, curSlide);
            break;
        case 'd':
                if (!doubleSlideDisplayMode && numOfSlides>1) {
                doubleSlideDisplayMode = 1;
                } else {
                doubleSlideDisplayMode = 0;
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
        case 'l':
            // TODO: handle double slide mode for links
            if (doubleSlideDisplayMode) {
                printMessageBottomBar("Links not supported in double slide display mode");
                getch();
                break;
            }
            printLinksOnSlide(curSlide);
            keyInput = getch();
            // on esc, break
            if (keyInput == 27) {
                break;
            }
            if (curSlide->link!=NULL) {
                openLinkAtIndex(keyInput-48, curSlide);
            }
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
            if (curSlide->maxX> max_x-1 || curSlide->y > max_y-2) {
            printw("terminal size/zoom error: Please resize or zoom out the terminal to display the slide.");
            } else {
            printSlideAtPosition((max_x - curSlide->maxX) / 2, (max_y-curSlide->y)/2, curSlide);
            }
        } else {
            // always stay one slide less than numOfSlides so that two slides can be printed
            if (curSlide->number == numOfSlides) {
                curSlide = curSlide->prev;
            }
            
            // find max y length of both slides
            int maxYLenBetweenSlides;
            if (curSlide->y > curSlide->next->y) {
                maxYLenBetweenSlides = curSlide->y;
            } else {
                maxYLenBetweenSlides = curSlide->next->y;
            }

            // todo: if statement is ugly
            if (curSlide->maxX + curSlide->next->maxX > max_x-2 || maxYLenBetweenSlides > max_y-2) {
                printw("terminal size/zoom error: Please resize or zoom out the terminal to display the slide.");
            } else {
                printSlideAtPosition(((max_x/2) - curSlide->maxX) / 2, (max_y-curSlide->y)/2, curSlide);
                printSlideAtPosition(((max_x/2) + ((max_x/2) - curSlide->next->maxX) / 2), (max_y-curSlide->next->y)/2, curSlide->next);
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
        // end the ncurses session, return 0 to close program
        endwin();
        return 0;
    }
}
