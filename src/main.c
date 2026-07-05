#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <errno.h>
#include <sys/ioctl.h>


struct termios original_terminal;



typedef struct editorRow {

    int size;

    char *chars;

} editorRow;



struct editorConfig {

    int cx;
    int cy;


    int rowOffset;

    int screenRows;
    int screenCols;


    int numberOfRows;

    editorRow *rows;

};


struct editorConfig editor;


enum editorKey {

    ARROW_LEFT = 1000,
    ARROW_RIGHT,
    ARROW_UP,
    ARROW_DOWN

};


/*
 Append buffer stores a complete terminal frame
 before writing it to the screen
*/
struct appendBuffer {

    char *buffer;
    int length;

};


#define APPEND_INIT {NULL, 0}


void append(
    struct appendBuffer *ab,
    const char *string,
    int length
) {

    char *new_buffer = realloc(
        ab->buffer,
        ab->length + length
    );


    if (new_buffer == NULL)
        return;


    memcpy(
        &new_buffer[ab->length],
        string,
        length
    );


    ab->buffer = new_buffer;

    ab->length += length;

}



void freeAppendBuffer(struct appendBuffer *ab) {

    free(ab->buffer);

}



void die(const char *message) {

    perror(message);
    exit(1);

}



void disableRawMode() {

    if (tcsetattr(
        STDIN_FILENO,
        TCSAFLUSH,
        &original_terminal
    ) == -1) {

        die("tcsetattr");

    }

}



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



int editorReadKey() {

    char c;


    if (read(STDIN_FILENO, &c, 1) == -1) {

        die("read");

    }


    if (c == '\x1b') {

        char sequence[2];


        if (read(STDIN_FILENO, &sequence[0], 1) != 1)
            return '\x1b';


        if (read(STDIN_FILENO, &sequence[1], 1) != 1)
            return '\x1b';



        if (sequence[0] == '[') {

            switch(sequence[1]) {

                case 'A': return ARROW_UP;

                case 'B': return ARROW_DOWN;

                case 'C': return ARROW_RIGHT;

                case 'D': return ARROW_LEFT;

            }

        }


        return '\x1b';

    }


    return c;

}





void editorInsertRow(char *text, int length) {


    editor.rows = realloc(
        editor.rows,
        sizeof(editorRow) * (editor.numberOfRows + 1)
    );


    int rowIndex = editor.numberOfRows;


    editor.rows[rowIndex].size = length;


    editor.rows[rowIndex].chars = malloc(length + 1);


    memcpy(
        editor.rows[rowIndex].chars,
        text,
        length
    );


    editor.rows[rowIndex].chars[length] = '\0';


    editor.numberOfRows++;

}




void editorOpen(char *filename) {


    FILE *file = fopen(filename, "r");


    if (!file) {

        die("fopen");

    }


    char *line = NULL;

    size_t capacity = 0;

    ssize_t length;



    while ((length = getline(&line, &capacity, file)) != -1) {


        while (
            length > 0 &&
            (
                line[length - 1] == '\n' ||
                line[length - 1] == '\r'
            )
        ) {

            length--;

        }


        editorInsertRow(line, length);

    }


    free(line);

    fclose(file);

}



void getWindowSize() {

    struct winsize size;


    if (ioctl(
        STDOUT_FILENO,
        TIOCGWINSZ,
        &size
    ) == -1) {

        die("ioctl");

    }


    editor.screenRows = size.ws_row;
    editor.screenCols = size.ws_col;

}




void moveCursor(int key) {

    switch(key) {

        case ARROW_LEFT:

            if (editor.cx != 0)
                editor.cx--;

            break;


        case ARROW_RIGHT:

            if (editor.cx != editor.screenCols - 1)
                editor.cx++;

            break;


        case ARROW_UP:

            if (editor.cy != 0)
                editor.cy--;

            break;


        case ARROW_DOWN:

            if (editor.cy < editor.numberOfRows - 1)
                editor.cy++;

            break;

    }

}




void editorScroll() {


    if (editor.cy < editor.rowOffset) {

        editor.rowOffset = editor.cy;

    }


    if (editor.cy >= editor.rowOffset + editor.screenRows) {

        editor.rowOffset = editor.cy - editor.screenRows + 1;

    }

}



void editorRefreshScreen() {


    editorScroll();


    struct appendBuffer ab = APPEND_INIT;


    append(&ab, "\x1b[2J", 4);

    append(&ab, "\x1b[H", 3);



    for (int i = 0; i < editor.screenRows; i++) {


        int fileRow = i + editor.rowOffset;


        if (fileRow >= editor.numberOfRows) {

            append(&ab, "~", 1);

        }

        else {

            append(
                &ab,
                editor.rows[fileRow].chars,
                editor.rows[fileRow].size
            );

        }


        append(&ab, "\r\n", 2);

    }



    char cursorPosition[32];


    snprintf(
        cursorPosition,
        sizeof(cursorPosition),
        "\x1b[%d;%dH",
        (editor.cy - editor.rowOffset) + 1,
        editor.cx + 1
    );


    append(
        &ab,
        cursorPosition,
        strlen(cursorPosition)
    );



    write(
        STDOUT_FILENO,
        ab.buffer,
        ab.length
    );


    freeAppendBuffer(&ab);

}



int main(int argc, char *argv[]) {


    enableRawMode();


    getWindowSize();


    editor.cx = 0;
    editor.cy = 0;


    editor.rowOffset = 0;


    editor.numberOfRows = 0;

    editor.rows = NULL;


    if (argc >= 2) {

        editorOpen(argv[1]);

    }



    while (1) {


        editorRefreshScreen();


        int c = editorReadKey();


        if (c == 'q') {

            break;

        }


        switch(c) {

            case ARROW_LEFT:
            case ARROW_RIGHT:
            case ARROW_UP:
            case ARROW_DOWN:

                moveCursor(c);
                break;

        }

    }


    return 0;

}
