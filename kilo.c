/* Includes */

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>


/* Defines */

#define CTRL_KEY(k) ((k) & 0x1f) // Bitwise AND char and 00011111


/* Data */

struct termios orig_termios;


/* Function Definitions */

void enableRawMode();
void disableRawMode();
void die(const char *s);
char editorReadKey();
void editorProcessKeypress();

/* Init */

int main() {
    enableRawMode();

    while (1) {
        editorProcessKeypress();
    }
        
    return 0;
}


/* Terminal */

void enableRawMode() {
    // Get original terminal flags
    if (tcgetattr(STDIN_FILENO, &orig_termios) == -1) {
        die("tcgetattr"); // Print error message and exit
    }

    atexit(disableRawMode); // Reset flags at program exit

    struct termios raw = orig_termios; // Copy flags

    // Bitwise AND and OR operations to clear or set flags
    raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON); // Disable ctrl-s and ctrl-q, fix ctrl-m
    raw.c_oflag &= ~(OPOST); // Disable output processing
    raw.c_cflag |= (CS8);    // Set char size to 8 bits
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG); // Disable echo, canonical, ctrl-c, ctrl-z, and ctrl-v
    raw.c_cc[VMIN] = 0;  // Set min time for read() to return
    raw.c_cc[VTIME] = 1; // Set max time before read() times out, 1/10 s

    // Set new terminal flags
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw)) {
        die("tcsetattr"); // Print error message and exit
    }
}

void disableRawMode() {
    // Reset original flags
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1) {
        die("tcsetattr"); // Print error message and exit
    }
}

void die(const char *s) {
    perror(s); // Print error message
    exit(1);
}

char editorReadKey() {
    int nread;
    char c;

    // Get input char
    while ((nread = read(STDIN_FILENO, &c, 1)) != 1) {
        if (nread == -1 && errno != EAGAIN) {
            die("read"); // Print error message and exit
        }
    }

    return c;
}


/* Input */

void editorProcessKeypress() {
    char c = editorReadKey(); // Get input

    switch(c) {
        case CTRL_KEY('q'):
            exit(0);
            break;
    }
}
