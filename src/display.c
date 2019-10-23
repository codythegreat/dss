#include <stdio.h>
#include "display.h"

// holds value of last pressed key
char prevPress[2] = "";
short numberInMem = 0;
char numberInput[4] = "";

void keyHandle()
{
    char keyInput = getch();
    switch(keyInput) {
    case 'q':
    case 'Q':
        break; // todo : find a way to break main function loop
    case KEY_DOWN:
    case 'j':
    case 'J':
        if (currentSlide != slideCount-1) {
            currentSlide++;
        }
    case KEY_UP:
    case 'k':
    case 'K':
        if (currentSlide != 0) {
            currentSlide--;
        }
    case 'g':
        if (numberInMem && prevPress == 'g') {
            int number = atoi(numberInput);
            if (!(number > slideCount)) {
                currentSlide = number;
                numberInput[0] = '\0'
            }
        } else if (prevPress[1] == 'g') {
            currentSlide = 1;
        }
    case 'G':
        if (numberInMem) {
            int number = atoi(numberInput);
            if (!(number > slideCount)) {
                currentSlide = number;
                numberInput[0] = '\0'
            }
        }
    }
}
