#include <stdio.h>
#include <stdlib.h>
#include <curses.h>
#include <string.h>
#include "parsecommand.h"

char userInput[1000];
int bottomLine;

command* parseCommand()
{
    // todo: add more if statements for commands
    command* comm = malloc(sizeof(struct command));
    char *p;
    p = strtok(userInput, " ");
    int i = 0;
    while (p!=NULL)
    {
        comm->arg[i][0] = '\0';
        strcat(comm->arg[i], p);
        p = strtok(NULL, " ");
        i++;
    }
    
    if (strcmp(comm->arg[0], "q")==0) 
    {
         comm->cmd = 1;
    } else if (strcmp(comm->arg[0], "open")==0)
    {
         comm->cmd = 2;
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
    bottomLine = *y-1;
    move(bottomLine, 1);
    // clear to end of line for printing
    clrtoeol();
    printw(":");

    char keyPress;
    int currentCharacter = 0;
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
