CC = gcc
CFLAGS = -Wall -Wextra -std=c99

TARGET = lumina

SRC = src/main.c

all:
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

clean:
	rm -f $(TARGET)

