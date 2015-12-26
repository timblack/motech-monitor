/**
	Author		:	Timothy Black
	Date		:	26th December 2015
	Description	:	Requests serial data from a Motech inverter via Serial, and
					sends it via Wi-Fi/Socket connection.
	Version		:	v0.8
*/

// Include files.
#include "Application/global.h"
#include "Application/interface.h"
#include "Application/settings.h"
#include "IO/internet.h"
#include "IO/serial.h"

/**
	Processes search/lookup request for Motech Inverter.
*/
void perform_scan_request(int sp)
{
	struct INV_DEVICE_VALUES *idv;
	int address;
	int pVerb;

	// Set verbose to OFF.
	pVerb = verbose;
	verbose = 0;

	// Scan for addresses 1-255.
	for (address=1; address<=255; address++) {
		printf("Searching on address %d\n", address);
		idv = read_device_values(address, sp);

		// If the address is found
		if (idv->Brand_Name != NULL) {
			// Print information on stdout.
			printf("Found inverter at address %d:\n", address);
			printf("\tBrand Name: %s\n", idv->Brand_Name);
			printf("\tType Name: %s\n", idv->Type_Name);
			printf("\tSerial Number: %s\n", idv->Sn_Name);

			// Set the address to the found address.
			inv_address = address;

			// Free memory.
			free(idv);
		}

		isleep(READ_SLEEP_USEC);
	}

	// Set verbose to the previous setting.
	verbose = pVerb;
}

/**
	Processes requests for Motech Inverter.
*/
void perform_main_requests(int sp)
{
	struct INVERTER_INFO ii;

	time_t rtime;
	struct tm *ti;

	ii.its1 = read_trip_settings1(inv_address, sp);
	ii.its2 = read_trip_settings2(inv_address, sp);
	ii.ids = read_device_settings(inv_address, sp);
	ii.itv = read_total_values(inv_address, sp);
	ii.idv = read_device_values(inv_address, sp);
	ii.ics = read_current_state(inv_address, sp);
	ii.icv = read_current_values(inv_address, sp);

	time (&rtime);
	ti = localtime(&rtime);

	strftime((char *) &ii.dt.date, 20, "%Y%m%d", ti);
	strftime((char *) &ii.dt.time, 20, "%H:%M", ti);

	if (ii.icv != NULL)
	{	
		print_inverter_data(&ii);
		if (pvo_send_to != 0) send_response_http_pvoutput(&ii);

		if (rof_flag != 0) write_fail_count(0);
	} else {
		fprintf(stderr, "Not publishing data to webservers as invalid responses from the inverter was received.\n");
		if (rof_flag != 0) check_for_failures();
	}
	
}

/**
 * Processes the options from the command line.
 */
int process_options(int argc, char *argv[]) {
    int opt, opterr;

    // Look for command line arguments.
    if (argc<=1) return -1;

    opterr = 1;
    while((opt = getopt(argc, argv, OPTLIST)) != -1)
    {
		switch (opt) {
			case 'b':	// Baud Rate
				if (atoi(optarg)==1) sp_baud_rate = B9600;
				else sp_baud_rate = B19200;
				break;
			case 's':	// Serial Port
				sp_dev_name = strdup(optarg);
				break;
			case 'a':	// Inverter Address
				inv_address = atoi(optarg);
				break;
			case 'l':	// Look for Inverter Address
				inv_look_for_addr = 1;
				break;
			case 'g':	// Get data from inverter.
				inv_get_data = 1;
				break;
			case 'p':	// Publish to PVOutput
				pvo_send_to = 1;
				break;
			case 'i':	// PVOutput System ID
				pvo_sys_id = strdup(optarg);
				break;
			case 'k':	// PVOutput API Key
				pvo_api_key = strdup(optarg);
				break;
			case 'r':	// Restart on Failure Flag
				rof_flag = 1;
				break;
			case 'c':	// Number of failures before restart
				rof_max_failures = atoi(optarg);
				break;
			case 'e':	// Hour to start monitoring failures
				rof_start_hour = atoi(optarg);
				break;
			case 'f':	// Hour to stop monitoring failures
				rof_stop_hour = atoi(optarg);
				break;
			default:	// Log error on invalid parameters.
				opterr = -1;
				break;
        }
    }

	return opterr;
}

/**
 * 	Prints the command line options to stdout.
 */
void print_options(char *strApp) {
	printf("Serial Port Arguments\n");
	printf("\t-b x\t\tBaud Rate(1=9600(Default), 2=19200)\n");
	printf("\t-s dev_name\tSerial Port Device Name(/dev/ttyUSB0 (Default))\n\n");

	printf("Inverter Arguments\n");
	printf("\t-a x\t\tInverter Address(45 (Default))\n");
	printf("\t-l\t\tLook/Scan for Inverter Address (0=Off(Default), 1=On)\n");
	printf("\t-g\t\tGet Inverter Data (0=Off(Default), 1=On)\n\n");

	printf("PVOutput Arguments\n");
	printf("\t-p\t\tPublish Data to PVOutput (0=Off(Default), 1=On)\n");
	printf("\t-i sys_id\tPVOutput System ID\n");
	printf("\t-k api_key\tPVOutput API Key\n\n");

	printf("Restart-On-Failure Arguments\n");
	printf("\t-c\t\tRestart on Failure Flag (0=Off(Default), 1=On)\n");
	printf("\t-c x\t\tMax Number of Failures before Restart(300 (Default))\n");
	printf("\t-e x\t\tHour to start monitoring failures (8 (Default))\n");
	printf("\t-f x\t\tHour to stop monitoring failures (16 (Default))\n\n");
}

/**
	Main function for the Motech application.

	Inputs: The arguments.
	Returns: integer for success or failure.
*/
int main(int argc, char *argv[])
{
	int sp;
	int err;

	printf("-----------------------------------------\n");
	printf("Motech Monitor v0.8\n");
	printf("http://github.com/timblack/motech-monitor\n");
	printf("-----------------------------------------\n\n");

	err = 1;
	if (process_options(argc, argv) == 1) {
		// Open the serial port, and check whether it was successful.
		sp = open_port();
		if (sp < 0) {
			check_for_failures();
			perror("Cannot connect on serial port. Something went seriously wrong.\n");
			exit(EXIT_FAILURE);
		}

		// Scan for Inverter Address.
		if (inv_look_for_addr) perform_scan_request(sp);

		// Perform the requests.
		if (inv_get_data) perform_main_requests(sp);

		// Close the serial port.
		close_port(sp);
	} else {
		err = 0;
		print_options(argv[0]);
	}

	return err;
}
