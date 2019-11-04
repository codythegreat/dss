#if !defined( PARSECOMMAND_H )
#define PARSECOMMAND_H

typedef struct command {
    short cmd;
    // todo: better way to do the below? array or string pointers?
    char arg[10][256];
} command;
// is this the best way to handle commands?
//           0 - bad input
// q/Q       1 - close program
// open      2 - open a new file
// tabopen   3 - bookmark current slide
// blist     4 - bookmark list
// bclear    5 - bookmark clear
// n         7 - jump to slide

command* parseCommand(char commandInput[1000]);

#endif
