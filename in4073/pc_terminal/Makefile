CC=gcc
CFLAGS = -g -Wextra -Wall -Werror -lm
EXEC = ./pc-terminal


default:
	$(CC) $(CFLAGS)  pc_terminal.c js_data_read.c -o $(EXEC)

clean:
	rm $(EXEC)

run: default
	$(EXEC)
