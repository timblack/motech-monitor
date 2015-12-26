/**
	Author		:	Timothy Black
	Date		:	26th December 2015
	Description	:	Requests serial data from a Motech inverter via Serial, and
					sends it via Wi-Fi/Socket connection.
	Version		:	v0.8
*/

#ifndef GLOBAL_H

	// Header Guard.
	#define GLOBAL_H

	/*
	 * Include Files.
	 */

	#include <arpa/inet.h>
	#include <fcntl.h>
	#include <math.h>
	#include <netdb.h>
	#include <netinet/in.h>
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include <sys/socket.h>
	#include <sys/types.h>
	#include <time.h>
	#include <termios.h>
	#include <unistd.h>
	#include <sys/reboot.h>
	#include <getopt.h>

	/*
	* Definitions.
	*/

	#define	OPTLIST						"b:s:a:lgpi:k:rc:e:f:"						// Command Line Argument List

	#define SERIAL_PORT_LOCATION		"/dev/ttyUSB0"								// Default Serial Port
	#define SERIAL_BAUD_RATE			B9600										// Default Baud Rate

	#define INV_DEFAULT_ADDRESS			0x2D										// Default Inverter Address

	#define PVOUTPUT_API_KEY			""											// Default API Key for PVOutput
	#define PVOUTPUT_SYS_ID				""											// Default System ID for PVOutput

	#define FAILURE_COUNT_RESTART		300											// Number of read failures before restarting device
	#define FAILURE_START_TIME			8											// Hour to start monitoring read failures.
	#define FAILURE_STOP_TIME			16											// Hour to stop monitoring read failures.
	#define FAILURE_FILE				"/tmp/motech_log.txt"						// Log file to store read failures.

	#define	STRING_SIZE					10											// Default String Size
	#define LINE_LENGTH					20											// Default Line Size

	#define READ_SLEEP_USEC				20000										// Default Sleep time (in microseconds)
	#define READ_COUNTER_MAX			10											// Number of Serial read timeouts before failure

	/*
	 * Custom Structures
	 */

	// Inverter Trip Settings #1
	struct INV_TRIP_SETTINGS_1 {
		int FacH_Trip;
		int FacH_Cycle;
		int FacL_Trip;
		int FacL_Cycle;
		int VacH_Trip;
		int VacH_Cycle;
		int VacL_Trip;
		int VacL_Cycle;
		int Delta_Zac_Trip;
		int Zac_Trip;
	};

	// Inverter Trip Settings
	struct INV_TRIP_SETTINGS_2 {
		int FastIEarth_Trip;
		int SlowIEarth_Trip;
		int Riso_Trip;
		int Vpv_Trip;
		int OnGrid_Delay;
		int VacH_Limit;
		int VacH_Limit_Cycle;
	};

	// Inverter Device Settings
	struct INV_DEVICE_SETTINGS {
		int Type_No;
		int Address;
		int Baudrate;
		int Language;
	};

	// Inverter Total Values
	struct INV_TOTAL_VALUES {
		long BridgeRelay_On_Num;
		int Time_Hr_Cnt;
		int Time_Min_Cnt;
		int Time_Sec_Cnt;
		long Eac;
		long Epv[3];
	};

	// Inverter Device Values
	struct INV_DEVICE_VALUES {
		char *Brand_Name;
		char *Type_Name;
		char *Sn_Name;
	};

	// Inverter Current State
	struct INV_CUR_STATE {
		int State;
		int Error_Code[4];
	};

	struct INV_CUR_VALUES {
		int Vpv[3];
		int Ppv[3];
		
		double Vac;
		double Pac;
		double Iac;
		double Fac;

		double Eac;

		double Ton_today;
		double Heatsink_Temp;
	};

	// Read Request Response
	struct READ_REQ_RESPONSE {
		char 	success;		// 0 on failure, 1 on success.
		int 	data_length;	// Data length.
		char 	*data;			// Pointer to the data.
	};

	// Read Request
	struct READ_REQ {
		int 	data_length;	// Length of the request.
		char 	*data;			// The read request.
	};

	// Date and Time
	struct DATETIME {
		char time[STRING_SIZE];
		char date[STRING_SIZE];
	};

	// Inverter Info Structure
	struct INVERTER_INFO {
		struct INV_TRIP_SETTINGS_1 *its1;
		struct INV_TRIP_SETTINGS_2 *its2;
		struct INV_DEVICE_SETTINGS *ids;
		struct INV_TOTAL_VALUES *itv;
		struct INV_DEVICE_VALUES *idv;
		struct INV_CUR_STATE *ics;
		struct INV_CUR_VALUES *icv;
		struct DATETIME dt;
	};

	// Required for "check for failures".
	extern int 		get_current_hour();
	extern void 	reboot_device();
	extern void 	check_for_failures();

	// Conversion functions.
	extern int 		convert_c2v(char, char);
	extern int 		ten_power(int);
	extern char 	*convert_i2s(int, int);

	// General function.
	extern void 	print_inverter_data(struct INVERTER_INFO *);
	extern void		isleep(long);

	// Cleanup functions.
	extern void cleanup_read_req(struct READ_REQ *);
	extern void cleanup_read_req_response(struct READ_REQ_RESPONSE *);

	/*
	 * Global Variables
	 */

	// General
	extern int verbose;				// Verbose mode

	// Serial Port Settings
	extern int sp_baud_rate;		// Serial Port Baud Rate
	extern char *sp_dev_name;		// Serial Port Device Name

	// Inverter Settings
	extern int inv_address;			// Inverter Address
	extern int inv_look_for_addr;	// Look for Inverter Address
	extern int inv_get_data;		// Get data from inverter.

	// PVOutput Settings
	extern int pvo_send_to;			// Send To PVOutput flag
	extern char *pvo_api_key;		// PVOutput API Key
	extern char *pvo_sys_id;		// PVOutput System ID

	// Restart on failure Settings
	extern int rof_flag;			// Restart on failure flag
	extern int rof_max_failures;	// Maximum failures for restart
	extern int rof_start_hour;		// Start hour for monitoring failures
	extern int rof_stop_hour;		// Stop hour for monitoring failures

#endif
