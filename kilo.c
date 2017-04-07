#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

struct termios orig_termios;

void enableRawMode();
void disableRawMode();

int main() {
    enableRawMode();

    char c;
    
    while (read(STDIN_FILENO, &c, 1) == 1 && c != 'q') {
        if (isprint(c)) {
            printf("%d ('%c')\n", c, c);
        } else {
            printf("%d\n", c);
        }
    }
        
    return 0;
}

void enableRawMode() {
    tcgetattr(STDIN_FILENO, &orig_termios); // Get terminal flags
    atexit(disableRawMode); // Reset flags at program exit

    struct termios raw = orig_termios; // Copy flags
    raw.c_lflag &= ~(ECHO | ICANON | ISIG); // Disable echo, canonical mode, ctrl-c, and ctrl-z signals

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw); // Apply terminal flags
}

void disableRawMode() {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios); // Reset original flags
}
