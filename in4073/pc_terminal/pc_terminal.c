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
#include "send_msg.h"
#include "js_data_read.h"

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

static int fd_serial_port;
char current_mode;
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
	// int8_t result;
	uint8_t c = '\0';
	// do {
	read(fd_serial_port, &c, 1);
	// } while (result != 1);
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

int serial_port_buffer(char* buf,int  size_buf)
{
	int result;
	do {
		result = (int) write(fd_serial_port, buf, size_buf);
	} while (result ==0);
	// printf("Written (port: %d) bytes: %d -- ", fd_serial_port, result);
	assert (result == size_buf);
	return result;
}


int open_serial(int argc,char ** argv){
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
	return 0;
}

/*----------------------------------------------------------------
 * main -- execute terminal
 *----------------------------------------------------------------
 */
int main(int argc, char **argv) {
	char c;
	struct message mlog;
	js_data_type jsdata;

	term_initio();
	js_init();
	term_puts("\nTerminal program - Embedded Real-Time Systems\n");
	if(open_serial(argc, argv)==-1){
		return -1;
	}

	char start_find[2] = {0, 0};

	for (;;) {
		start_find[0] = start_find[1];
		start_find[1] = c;

		jsdata = js_values_read();
		mlog.start = (uint8_t)0xAA;
		mlog.roll_H = (jsdata.roll >> 8) & MASK; 
		mlog.roll_L = jsdata.roll & MASK;
		mlog.pitch_H = (jsdata.pitch >> 8) & MASK; 
		mlog.pitch_L = jsdata.pitch & MASK;
		mlog.yaw_H = (jsdata.yaw >> 8) & MASK; 
		mlog.yaw_L = jsdata.yaw & MASK;
		mlog.lift_H = (jsdata.lift >> 8) & MASK; 
		mlog.lift_L = jsdata.lift & MASK;
		mlog.mode = term_getchar_nb();
		mlog.stop = (uint8_t)'\n';
		char* buf = (char *)&mlog; 

		if (jsdata.panic)
			mlog.mode = '1';

		serial_port_buffer(buf, sizeof(struct message));

		c = serial_port_getchar();
		if (c != '\0') {
			term_putchar(c);		
		}

		//print_message(mlog);
	}

	term_exitio();
	serial_port_close();
	js_close();
	term_puts("\n<exit>\n");

	return 0;
}

