/**
	Author		:	Timothy Black
	Date		:	26th December 2015
	Description	:	Requests serial data from a Motech inverter via Serial, and
					sends it via Wi-Fi/Socket connection.
	Version		:	v0.8
*/

// Include Files.
#include "../Application/global.h"

// External declarations.
extern int 		open_port();
extern void 	close_port(int);
extern void 	write_sp_command(int, char *, int, char *);
extern int 		read_sp_response(int, char *, int *, char *);
