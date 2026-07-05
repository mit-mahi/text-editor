#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <errno.h>


struct termios original_terminal;


/*
 Exit after printing system error
*/
void die(const char *message) {

    perror(message);
    exit(1);

}


/*
 Restore terminal back to normal mode
*/
void disableRawMode() {

    if (tcsetattr(
        STDIN_FILENO,
        TCSAFLUSH,
        &original_terminal
    ) == -1) {
        die("tcsetattr");
    }

}


/*
 Enable raw terminal mode
*/
void enableRawMode() {

    if (tcgetattr(
        STDIN_FILENO,
        &original_terminal
    ) == -1) {
        die("tcgetattr");
    }


    atexit(disableRawMode);


    struct termios raw = original_terminal;


    raw.c_iflag &= ~(IXON | ICRNL);

    raw.c_oflag &= ~(OPOST);

    raw.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN);


    if (tcsetattr(
        STDIN_FILENO,
        TCSAFLUSH,
        &raw
    ) == -1) {
        die("tcsetattr");
    }

}


/*
 Read exactly one key press
*/
char editorReadKey() {

    char c;

    int bytes_read = read(
        STDIN_FILENO,
        &c,
        1
    );


    if (bytes_read == -1) {
        die("read");
    }


    return c;
}



int main() {

    enableRawMode();


    while (1) {

        char c = editorReadKey();


        if (c == 'q') {
            break;
        }


        printf("%d\r\n", c);

    }


    return 0;
}
