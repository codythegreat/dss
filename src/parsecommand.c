#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parsecommand.h"

command* parseCommand(char commandInput[1000])
{
    // create a new command struct (to be returned)
    command* comm = malloc(sizeof(struct command));

    // *p holds current arg sep by spaces
    char *p;
    p = strtok(commandInput, " ");

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
    } else if (strcmp(comm->arg[0], "double")==0)
    {
         comm->cmd = 7;
    } else 
    {
         comm->cmd = 0;
    }

    return comm;
}
