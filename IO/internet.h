/**
	Author		:	Timothy Black
	Date		:	26th December 2015
	Description	:	Requests serial data from a Motech inverter via Serial, and
					sends it via Wi-Fi/Socket connection.
	Version		:	v0.8
*/

// Include Files.
#include "../Application/global.h"

// External Declarations.
extern char *get_ip_for_hostname(char *);
extern void send_response_http(char *, char *);
extern void send_response_http_pvoutput(struct INVERTER_INFO *);
