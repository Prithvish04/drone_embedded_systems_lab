CC=gcc
CFLAGS = -g -Wextra -Wall -Werror  -lm
EXEC = ./pc-terminal 


default:
	$(CC) $(CFLAGS) send_msg.c js_data_read.c pc_terminal.c  -o $(EXEC)

clean:
	rm $(EXEC)

run: default
	$(EXEC) 
