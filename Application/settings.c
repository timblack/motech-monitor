/**
	Author		:	Timothy Black
	Date		:	26th December 2015
	Description	:	Requests serial data from a Motech inverter via Serial, and
					sends it via Wi-Fi/Socket connection.
	Version		:	v0.8
*/

// Include files.
#include "global.h"

/**
	Reads the failure count from the log file.

	Returns: The failure count on success, 0 otherwise.
*/
int read_fail_count()
{
	FILE *file;
	char line[LINE_LENGTH];
	int failCount;

	// Initialise the failure count to zero.
	failCount = 0;

	// Open the file, and read the failure count.
	file = fopen(FAILURE_FILE, "r");
	if (file != NULL)
	{
		if (fgets (line, sizeof(line), file) != NULL)
		{
			sscanf(line, "%d", &failCount);
		}
		fclose (file);
	}

	// Return the failure count.
	return failCount;
}

/**
	Writes the failure count to the log file.
	
	Inputs: The failure count.
*/
void write_fail_count(int failCount)
{
	FILE *file;
	char line[LINE_LENGTH];

	// Prepare assigning the failure count to string.
	sprintf((char*) &line, "%d", failCount);

	// Open the file and write the failure count to file.
	file = fopen(FAILURE_FILE, "w");
	if (file != NULL)
	{
		fputs((char *)&line, file);
		fclose (file);
	}
}

