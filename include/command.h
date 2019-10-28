#if !defined( COMMAND_H )
#define COMMAND_H

typedef struct command {
    short cmd;
    char arg[256];
} command;
// is this the best way to handle commands?
//           0 - bad input
// q/Q       1 - close program
// open      2 - open a new file
// tabopen   3 - open a new file on another tab
// blist     4 - bookmark list
// n         5 - jump to slide

command* commandLoop(int *y);

#endif
