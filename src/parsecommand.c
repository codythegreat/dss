#include <stdio.h>
#include <stdlib.h>
#include <curses.h>
#include <string.h>
#include "parsecommand.h"

// holds user input while in command mode
char userInput[1000];
// y value of bottom of screen
int bottomLine;

command* parseCommand()
{
    // create a new command struct (to be returned)
    command* comm = malloc(sizeof(struct command));

    // *p holds current arg sep by spaces
    char *p;
    p = strtok(userInput, " ");

    // iterate over each argument and add to comm->arg array
    int i = 0;
    while (p!=NULL)
    {
        comm->arg[i][0] = '\0';
        strcat(comm->arg[i], p);
        p = strtok(NULL, " ");
        i++;
    }
    
    // todo: add more if statements for commands
    // todo: can this be done better?
    if (strcmp(comm->arg[0], "q")==0) 
    {
         comm->cmd = 1;
    } else if (strcmp(comm->arg[0], "open")==0)
    {
         comm->cmd = 2;
    } else if (strcmp(comm->arg[0], "bmark")==0)
    {
         comm->cmd = 3;
    } else if (strcmp(comm->arg[0], "blist")==0)
    {
         comm->cmd = 4;
    } else if (strcmp(comm->arg[0], "bclear")==0)
    {
         comm->cmd = 5;
    } else if (atoi(comm->arg[0])!=NULL)
    {
         comm->cmd = 6;
    } else 
    {
         comm->cmd = 0;
    }

    return comm;
}


command* commandLoop(int *y)
{
    // move to bottom of screen, clear line and print :
    bottomLine = *y-1;
    move(bottomLine, 1);
    // clear to end of line for printing
    clrtoeol();
    printw(":");
    
    // enable cursor
    curs_set(1);

    // bool becomes false after enter is pressed
    bool typing = true;

    // tracks character for keyPress to userInput
    int currentCharacter = 0;

    char keyPress;
    while (typing)
    {
        keyPress = getch();
	switch(keyPress)
	{
        case '\n':  // enter
            typing = false;
            break;
        case 27:    // esc
            break;
        case 127:   // back space  todo: test key input on mult systems
            if (currentCharacter != 0) {
                // move back and delete character in place
                mvdelch(bottomLine, 1+currentCharacter);
                currentCharacter--;
	        }
            break;
        default:
            // print the character to the screen
            addch(keyPress);
            // add the character to our userIn buffer
            userInput[currentCharacter] = keyPress;
            userInput[currentCharacter+1] = '\0';
            currentCharacter++;
    	    break;
	}
    }

    // hide cursor
    curs_set(0);

    // parse input into a command struct and return it
    return parseCommand();
}
