/*------------------------------------------------------------
 * Simple pc terminal in C
 *
 * Arjan J.C. van Gemund (+ mods by Ioannis Protonotarios)
 *
 * read more: http://mirror.datenwolf.net/serial/
 *------------------------------------------------------------
 */

#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>
#include <inttypes.h>
#include <math.h>
#include <string.h>
#include "recieve_msg.h"
#include "parsing_drone.h"

/*------------------------------------------------------------
 * console I/O
 *------------------------------------------------------------
 */
struct termios 	savetty;

void	term_initio()
{
	struct termios tty;

	tcgetattr(0, &savetty);
	tcgetattr(0, &tty);

	tty.c_lflag &= ~(ECHO|ECHONL|ICANON|IEXTEN);
	tty.c_cc[VTIME] = 0;
	tty.c_cc[VMIN] = 0;

	tcsetattr(0, TCSADRAIN, &tty);
}

void	term_exitio()
{
	tcsetattr(0, TCSADRAIN, &savetty);
}

void	term_puts(char *s)
{
	fprintf(stderr,"%s",s);
}

void	term_putchar(char c)
{
	putc(c,stderr);
}

int	term_getchar_nb()
{
	static unsigned char 	line [2];

	if (read(0,line,1)) // note: destructive read
		return (int) line[0];

	return -1;
}

int	term_getchar()
{
	int    c;

	while ((c = term_getchar_nb()) == -1)
		;
	return c;
}

/*------------------------------------------------------------
 * Serial I/O
 * 8 bits, 1 stopbit, no parity,
 * 115,200 baud
 *------------------------------------------------------------
 */
#include <termios.h>
#include <ctype.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <assert.h>
#include <time.h>
#include "parsing_drone.h"

static int fd_serial_port;
/*
 * Open the terminal I/O interface to the serial/pseudo serial port.
 *
 */
void serial_port_open(const char *serial_device)
{
	char *name;
	int result;
	struct termios tty;

	fd_serial_port = open(serial_device, O_RDWR | O_NOCTTY);

	assert(fd_serial_port>=0);

	result = isatty(fd_serial_port);
	assert(result == 1);

	name = ttyname(fd_serial_port);
	assert(name != 0);

	result = tcgetattr(fd_serial_port, &tty);
	assert(result == 0);

	tty.c_iflag = IGNBRK; /* ignore break condition */
	tty.c_oflag = 0;
	tty.c_lflag = 0;

	tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8; /* 8 bits-per-character */
	tty.c_cflag |= CLOCAL | CREAD; /* Ignore model status + read input */

	cfsetospeed(&tty, B115200);
	cfsetispeed(&tty, B115200);

	tty.c_cc[VMIN]  = 0;
	tty.c_cc[VTIME] = 1; // TODO check cpu usage

	tty.c_iflag &= ~(IXON|IXOFF|IXANY);

	result = tcsetattr (fd_serial_port, TCSANOW, &tty); /* non-canonical */

	tcflush(fd_serial_port, TCIOFLUSH); /* flush I/O buffer */
}


void serial_port_close(void)
{
	int 	result;

	result = close(fd_serial_port);
	assert (result==0);
}


uint8_t serial_port_getchar()
{
	int8_t result;
	uint8_t c;

	do {
		result = read(fd_serial_port, &c, 1);
	} while (result != 1);

	//	assert(result == 1);
	return c;
}


int serial_port_putchar(char c)
{
	int result;

	do {
		result = (int) write(fd_serial_port, &c, 1);
	} while (result == 0);

	assert(result == 1);
	return result;
}


/*----------------------------------------------------------------
 * main -- execute terminal
 *----------------------------------------------------------------
 */
int main(int argc, char **argv)
{
	char c;

	term_initio();
	term_puts("\nTerminal program - Embedded Real-Time Systems\n");

	// if no argument is given at execution time, /dev/ttyUSB0 is assumed
	// asserts are in the function
	if (argc == 1) {
		serial_port_open("/dev/ttyUSB0");
	} else if (argc == 2) {
		serial_port_open(argv[1]);
	} else {
		printf("wrong number of arguments\n");
		return -1;
	}

	term_puts("Type ^C to exit\n");

	/* send & receive
	 */
	 
	int count = 0;
	int message[84];
	struct message m_rec;
	for (int i = 0; i<60;i++)
	{
		message[i] =0;
	}
	//int timestamp_start_find[8];
	//long long decimal = 0, base = 1;  
	char start_find[2] = {0, 0};
	for (;;) 
	{
		start_find[0] = start_find[1];
		start_find[1] = c;

		if ((c = term_getchar_nb()) != -1) {
			serial_port_putchar(c);
		}
		if ((c = serial_port_getchar()) != -1) 
		{
			message[count] = c;
			//term_putchar(start_find[0]);
			//term_putchar(c);
			count = count+1;
			//term_putchar((char)count);

			//printf("%d\n",count);

			//printf(" %d ", count);
			if (count == 73)
			{
				//term_putchar('M');
				m_rec.timestamp = message_block(message,0,7);
				m_rec.mot0 = message_block(message,7,4);
				m_rec.mot1 = message_block(message,11,4);
				m_rec.mot2 = message_block(message,15,4);
				m_rec.mot3 = message_block(message,19,4);

				count = 0;
			}
			if ((int) start_find[0] == 65 && ((int) start_find[1]) == 65)
			{
				count = 0;
			}
		//	}

			//term_putchar('a');
			
			/*if (((int) start_find[0]) == 65 && ((int) start_find[1]) == 65)// && message[count-2] == 65 && message[count-3] == 48 && message[count-1] == 65)
			{
				
				
				if (count == 84)
				{
					printf("OK");
					printf("%d ",count);
					print_message_block(message,8, 4); 
					

					char buffer[5]; 
					for (int i =12; i<16; i++)
					{
						buffer[i-12] = message[i];
					} 
					buffer[4] = '\n';
					printf("%lld |",to_dec(buffer,3));

					for (int i =16; i<20; i++)
					{
						buffer[i-16] = message[i];
					} 
					buffer[4] = '\n';
					printf("%lld |",to_dec(buffer,3));
				
					for (int i =20; i<24; i++)
					{
						buffer[i-20] = message[i];
					} 
					buffer[4] = '\n';
					printf("%lld |",to_dec(buffer,3));
					
					for (int i =24; i<28; i++)
					{
						buffer[i-24] = message[i];
					} 
					buffer[4] = '\n';
					printf("%lld |\n",to_dec(buffer,3));
					count = 0;
					count = 0;
				}
			}

				
				for (int i = 0; i<60;i++)
				{
					message[i] =0;
				}
				
			}*/
/*			
			else
			{
				message[count] = c; 
				count+=1;
			}*/
			//printf(".");
			//printf("\n");

			
		}
	}

	term_exitio();
	serial_port_close();
	term_puts("\n<exit>\n");

	return 0;
}

