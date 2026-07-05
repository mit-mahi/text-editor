#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>


struct termios original_terminal;


/*
 Restore terminal back to normal mode
*/
void disableRawMode() {

    tcsetattr(
        STDIN_FILENO,
        TCSAFLUSH,
        &original_terminal
    );

}


/*
 Enable raw terminal mode
*/
void enableRawMode() {

    // Save current terminal settings
    tcgetattr(
        STDIN_FILENO,
        &original_terminal
    );

    // Restore terminal when program exits
    atexit(disableRawMode);


    struct termios raw = original_terminal;


    // Disable input processing:
    // IXON  -> Ctrl-S / Ctrl-Q terminal flow control
    // ICRNL -> automatic carriage return conversion

    raw.c_iflag &= ~(IXON | ICRNL);


    // Disable output processing:
    // OPOST -> terminal output transformations

    raw.c_oflag &= ~(OPOST);


    // Disable local terminal features:
    // ECHO   -> automatic printing
    // ICANON -> line buffering
    // ISIG   -> Ctrl-C / Ctrl-Z signals
    // IEXTEN -> extended shortcuts

    raw.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN);


    tcsetattr(
        STDIN_FILENO,
        TCSAFLUSH,
        &raw
    );
}


int main() {

    enableRawMode();


    char c;

    while (read(STDIN_FILENO, &c, 1) == 1) {

        if (c == 'q') {
            break;
        }

    }


    return 0;
}
