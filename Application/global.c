/**
	Author		:	Timothy Black
	Date		:	26th December 2015
	Description	:	Requests serial data from a Motech inverter via Serial, and
					sends it via Wi-Fi/Socket connection.
	Version		:	v0.8
*/

// Include Files.
#include "global.h"
#include "settings.h"

/*
 * Global Variables
 */

// General
int verbose 			= 1;

// Serial Port Settings
int sp_baud_rate 		= SERIAL_BAUD_RATE;
char *sp_dev_name 		= SERIAL_PORT_LOCATION;

// Inverter Settings
int inv_address 		= INV_DEFAULT_ADDRESS;
int inv_look_for_addr 	= 0;
int inv_get_data		= 0;

// PVOutput Settings
int pvo_send_to 		= 0;
char *pvo_api_key 		= PVOUTPUT_API_KEY;
char *pvo_sys_id 		= PVOUTPUT_SYS_ID;

// Restart on failure Settings
int rof_flag 			= 0;
int rof_max_failures 	= FAILURE_COUNT_RESTART;
int rof_start_hour 		= FAILURE_START_TIME;
int rof_stop_hour 		= FAILURE_STOP_TIME;

/**
	Gets the current hour.
	
	Returns: The current hour of the device.
*/
int get_current_hour()
{
	time_t now;
	struct tm *localTime;

	now = time(0);
	localTime = localtime(&now);

	return localTime->tm_hour;
}

/**
	Attempts to reboot the device.
*/
void reboot_device()
{
	reboot(RB_AUTOBOOT);
}

/**
	Checks for constant failures(more than 3 times).
*/
void check_for_failures()
{
	int hour;
	int failCount;

	// Read failure count for Serial Port.
	failCount = read_fail_count();
	printf("The communication failure count has reached: %d\n", failCount);

	// If the failure rate is too high during operational times, restart device.
	if (failCount > rof_max_failures) {
		hour = get_current_hour();

		if ((hour > rof_start_hour) && (hour < rof_stop_hour))
		{
			printf("Rebooting device because of too many failures.");
			reboot_device();
		}
	} else {
		write_fail_count(failCount+1);
	}
}

/**
	Convert chars to value.
	
	Inputs: Higher byte, and lower byte.
	Returns: The calculated value.
*/
int convert_c2v(char v1, char v2)
{
	int valOut;

	valOut = (unsigned char) v1;
	valOut = valOut << 8;
	valOut = (valOut + ((unsigned char) v2));

	return valOut;
}

/**
	Calculates 10 to the power of x.
	
	Inputs: The exponential(x).
	Returns: The calculated value.
*/
int ten_power(int expon)
{
	int i;
	int valOut;

	valOut = 1;
	for (i=0; i<expon; i++) valOut = valOut * 10;

	return valOut;
}

/**
	Converts int to string.
	
	Inputs: The value to convert to string, the number of decimal places to shift.
	Returns: The calculated value.
*/
char *convert_i2s(int valIn, int decimalPlaces)
{
	char *valOut;

	// Allocate memory.
	valOut = malloc(STRING_SIZE);
	if (valOut == NULL) return NULL;
	
	if (decimalPlaces<=0) sprintf(valOut, "%d", valIn);
	else sprintf(valOut, "%d.%d", valIn / ten_power(decimalPlaces), valIn % ten_power(decimalPlaces));
	
	return valOut;
}

/**
	Prints inverter data to console.
	
	Inputs: The inverter info.
*/
void print_inverter_data(struct INVERTER_INFO *inv_info)
{
	int i;

	printf("Brand Name: %s\n", inv_info->idv->Brand_Name);
	printf("Type Name: %s\n", inv_info->idv->Type_Name);
	printf("Serial Number: %s\n", inv_info->idv->Sn_Name);

	printf("Time: %s\n", inv_info->dt.time);
	printf("Date: %s\n", inv_info->dt.date);

	for (i=0; i<3; i++) printf("Array Power(%d): %d W\n", i+1, inv_info->icv->Ppv[i]);
	for (i=0; i<3; i++) printf("Array Voltage(%d): %d V\n", i+1, inv_info->icv->Vpv[i]);
	
	printf("AC Frequency: %.2f Hz\n", inv_info->icv->Fac);
	printf("AC Voltage: %.2f V\n", inv_info->icv->Vac);
	printf("AC Power: %.2f W\n", inv_info->icv->Pac);
	printf("AC Current: %.2f A\n", inv_info->icv->Iac);
	
	printf("Heatsink Temp: %.2f degC\n", inv_info->icv->Heatsink_Temp);
	printf("Total time hours: %d hr\n", inv_info->itv->Time_Hr_Cnt);
	printf("Total time minutes: %d mins\n", inv_info->itv->Time_Min_Cnt);
	printf("Total Power: %ld kWh\n", inv_info->itv->Eac);
	printf("Time on today: %.2f hr\n", inv_info->icv->Ton_today);
}

/*
 * Cleans up a Read-Request item.
 */
void cleanup_read_req(struct READ_REQ *rr)
{
	free(rr->data);
	free(rr);
}

/**
 * Cleans up a Read-Request-Response item.
 */
void cleanup_read_req_response(struct READ_REQ_RESPONSE *rrr)
{
	free(rrr->data);
	free(rrr);
}

/**
	Blocks the application of microseconds
	
	Inputs: The microseconds to block for.
*/
void isleep(long lInterval) {
	struct timespec ts;

	ts.tv_sec = 0;
	ts.tv_nsec = (lInterval * 1000);

	nanosleep(&ts, NULL);
}
