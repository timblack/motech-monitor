/**
	Author		:	Timothy Black
	Date		:	26th December 2015
	Description	:	Requests serial data from a Motech inverter via Serial, and
					sends it via Wi-Fi/Socket connection.
	Version		:	v0.8
*/

// Include Files.
#include "../Application/global.h"

/**
	Attempts to open a new port.

	Returns: File descriptor of the serial port opened.
*/
int open_port()
{
	int sp;
	struct termios tio_settings;

	if (verbose) printf("Opening serial port: %s\n", sp_dev_name);

	// Open port, and check for error.
	sp = open(sp_dev_name, O_RDWR | O_NOCTTY | O_NDELAY);
	if (sp == -1) {
		perror("Unable to open serial port.");
		check_for_failures();
		exit(EXIT_FAILURE);
	} else {
		fcntl(sp, F_SETFL, O_NONBLOCK);
	}

	// Set the applicable serial port settings.
	tio_settings.c_cflag = sp_baud_rate | CS8 | CLOCAL | CREAD;
	tio_settings.c_iflag = IGNPAR;
	tio_settings.c_oflag = 0;
	tio_settings.c_lflag = 0;
	tcsetattr(sp, TCSANOW, &tio_settings);

	if (verbose) printf("Serial port opened.\n");

	return sp;
}

/**
	Closes the serial port.
*/
void close_port(int sp)
{
	if (verbose) printf("Closing serial port.\n");
	close(sp);
	if (verbose) printf("Serial port closed.\n");
}

/**
	Write command to serial port.
*/
void write_sp_command(int sp, unsigned char *command, int command_len, char *command_name)
{
	if (verbose) printf("Writing %d bytes to serial port.\n", command_len);
	if (write(sp, command, command_len) != command_len) {
		fprintf(stderr, "Error writing '%s' command to serial port.\n", command_name);
	}
}

/**
	Read response from serial port.

	Returns: Pointer to the response from serial port.
*/
int read_sp_response(int sp, char *buffer, int *buffer_len, char *command_name)
{
	int bufRead;
	int bufPos;
	char valIn;
	int counter;

	// Read one byte at a time, allow timeout, and report error if issue arises.
	bufPos = 0;
	counter = 0;
	while (bufPos < *buffer_len) {
		// Read one byte.
		bufRead = read(sp, &valIn, 1);

		// Wait/Block for required time.
		isleep(READ_SLEEP_USEC);
		
		// Check for successful read.
		if (bufRead > 0) {
			buffer[bufPos] = valIn;
			bufPos++;
			counter = 0;
		} else {
			counter++;

			// If the failure counter has reached the maximum limit, return error.
			if (counter > READ_COUNTER_MAX) {
				if (verbose) fprintf(stderr, "Did not receive expected response via Serial for '%s(%d, %d)'\n", command_name, bufPos, bufRead);
				*buffer_len = bufPos;
				return -1;
			}
		}
	}

	return 1;
}

