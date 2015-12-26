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
extern unsigned short calculate_crc16(char *, unsigned char);
extern struct READ_REQ *generate_read_request(char, int, int);
extern char *generate_scan_request(char);
extern struct READ_REQ_RESPONSE *read_response_header(char, char *, int);

