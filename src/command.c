#include <stdio.h>
#include <stdlib.h>
#include <curses.h>
#include <string.h>
#include "command.h"

char userInput[100];
int bottom;

command* parseCommand()
{
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

void printCommand()
{
    move(bottom, 1);
    clrtoeol();           // clear the line for printing
    printw(":", userInput);
}

command* commandLoop(int *y)
{
    bottom = *y-1;
    printCommand();
    char keyPress;
    int currentCharacter = 0;
    bool typing = true;
    while (typing)
    {
        keyPress = getch();
	switch(keyPress)
	{
            case '\n':
                typing = false;
                break;
            case 27:
                break;
            case 127:
                if (currentCharacter != 0) {
                    mvdelch(bottom, 1+currentCharacter);
                    currentCharacter--;
	        }
		break;
            default:
                addch(keyPress);
	        userInput[currentCharacter] = keyPress;
	        userInput[currentCharacter+1] = '\0';
	        currentCharacter++;
		break;
	}
    }
    curs_set(0);
    return parseCommand();
}
