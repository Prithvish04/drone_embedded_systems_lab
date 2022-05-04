CC=gcc
CFLAGS = -g -Wextra -Wall -Werror -lm
EXEC = ./pc-terminal

default:
	$(CC) $(CFLAGS) recieve_msg.c pc_terminal.c -o $(EXEC)

clean:
	rm $(EXEC)

run: default
	$(EXEC)
