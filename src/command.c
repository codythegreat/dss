#include <stdio.h>
#include <stdlib.h>
#include <curses.h>
#include <string.h>
#include "command.h"

char userInput[100];
int bottomLine;

command* parseCommand()
{
    // todo: change into switch with more cases/logic
    // todo: text should be split by spaces, first 
    // item is the verb, then subsequent to the verb
    // is the subject(s)
    command* cmd = malloc(sizeof(struct command));
    if (strcmp(userInput, "q")==0) 
    {
        cmd->cmd = 1;
    } else 
    {
        cmd->cmd = 0;
    }
    return cmd;
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
