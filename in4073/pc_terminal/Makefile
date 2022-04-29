CC=gcc
CFLAGS = -g -Wextra -Wall -Werror -lm
EXEC = ./pc-terminal

default:
	$(CC) $(CFLAGS) joystick.c -o $(EXEC)
	$(CC) $(CFLAGS) communicaiton.c -o $(EXEC)
	$(CC) $(CFLAGS) pc_terminal.c -o $(EXEC)

clean:
	rm $(EXEC)

run: default
	$(EXEC)
