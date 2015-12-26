/**
	Author		:	Timothy Black
	Date		:	26th December 2015
	Description	:	Requests serial data from a Motech inverter via Serial, and
					sends it via Wi-Fi/Socket connection.
	Version		:	v0.8
*/

// Include Files.
#include "global.h"

// External declarations.
extern struct INV_TRIP_SETTINGS_1 *read_trip_settings1(char, int);
extern struct INV_TRIP_SETTINGS_2 *read_trip_settings2(char, int);
extern struct INV_DEVICE_SETTINGS *read_device_settings(char, int);
extern struct INV_TOTAL_VALUES *read_total_values(char, int);
extern struct INV_DEVICE_VALUES *read_device_values(char, int);
extern struct INV_CUR_STATE *read_current_state(char, int);
extern struct INV_CUR_VALUES *read_current_values(char, int);
