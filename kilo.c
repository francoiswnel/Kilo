#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

struct termios orig_termios;

void enableRawMode();
void disableRawMode();
void die(const char *s);

int main() {
    enableRawMode();

    while (1) {
        char c = '\0';

        // Get input
        if (read(STDIN_FILENO, &c, 1) == -1 && errno != EAGAIN) {
            die("read"); // Print error message and exit
        }

        if (iscntrl(c)) {
            printf("%d\r\n", c);
        } else {
            printf("%d ('%c')\r\n", c, c);
        }

        if (c == 'q') {
            break;
        }
    }
        
    return 0;
}

void enableRawMode() {
    // Get original terminal flags
    if (tcgetattr(STDIN_FILENO, &orig_termios) == -1) {
        die("tcgetattr"); // Print error message and exit
    }

    atexit(disableRawMode); // Reset flags at program exit

    struct termios raw = orig_termios; // Copy flags
    raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON); // Disable ctrl-s and ctrl-q, fix ctrl-m
    raw.c_oflag &= ~(OPOST); // Disable output processing
    raw.c_cflag |= (CS8); // Set char size to 8 bits
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG); // Disable echo, canonical, ctrl-c, ctrl-z, and ctrl-v
    raw.c_cc[VMIN] = 0; // Set min time for read() to return
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
