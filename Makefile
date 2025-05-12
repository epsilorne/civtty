CC = gcc

CFLAGS = -Wall -g

SRC = src/civtty.c
TARGET = build/civtty

all: $(SRC)
	@mkdir -p build
	@$(CC) -o $(TARGET) $(SRC)

clean:
	@rm -f $(TARGET)
