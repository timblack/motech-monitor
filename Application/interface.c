/**
	Author		:	Timothy Black
	Date		:	26th December 2015
	Description	:	Requests serial data from a Motech inverter via Serial, and
					sends it via Wi-Fi/Socket connection.
	Version		:	v0.8
*/

// Include files.
#include "global.h"
#include "protocol.h"
#include "../IO/serial.h"

/**
	Performs a serial read/write request.
*/
void perform_request(int sp, char *strAction, struct READ_REQ *rr, char *response, int *response_length)
{
	if (verbose) printf("Performing %s.\n", strAction);
	write_sp_command(sp, rr->data, rr->data_length, strAction);
	read_sp_response(sp, response, response_length, strAction);
}

/**
	Interprets the serial read request.
*/
struct READ_REQ_RESPONSE *process_response(char address, struct READ_REQ *rr, char *response, int response_len)
{
	struct READ_REQ_RESPONSE *rrr;

	// Free request.
	cleanup_read_req(rr);

	// Process response.
	rrr = read_response_header(address, response, response_len);
	if (rrr->success != 1) {
		if (verbose) fprintf(stderr, "The header was invalid(%d)\n", rrr->success);
		cleanup_read_req_response(rrr);
		return NULL;
	}

	return rrr;
}

/**
	Reads the Trip Settings #1.
*/
struct INV_TRIP_SETTINGS_1 *read_trip_settings1(char address, int sp)
{
	struct INV_TRIP_SETTINGS_1 	*its1;
	struct READ_REQ 			*rr;
	struct READ_REQ_RESPONSE 	*rrr;

	char response[10*2+7];
	int response_len = 10*2+7;

	its1 = malloc(sizeof(struct INV_TRIP_SETTINGS_1));
	if (its1 == NULL) return NULL;

	// Generate Inverter Trip Settings Request.
	rr = generate_read_request(address, 0x01, 10);
	if (rr == NULL) {
		free(its1);
		return NULL;
	}

	// Perform Serial Port Initialisation.
	perform_request(sp, "Serial Port Initialisation", rr, (char *) &response, &response_len);
	perform_request(sp, "Inverter Trip Settings", rr, (char *) &response, &response_len);

	// Process the read request response.
	rrr = process_response(address, rr, response, response_len);
	if (rrr == NULL) {
		free(its1);
		return NULL;
	}

	// Designate data into its variable.
	its1->FacH_Trip = convert_c2v(rrr->data[0], rrr->data[1]);
	its1->FacH_Cycle = convert_c2v(rrr->data[2], rrr->data[3]);
	its1->FacL_Trip = convert_c2v(rrr->data[4], rrr->data[5]);
	its1->FacL_Cycle = convert_c2v(rrr->data[6], rrr->data[7]);
	its1->VacH_Trip = convert_c2v(rrr->data[8], rrr->data[9]);
	its1->VacH_Cycle = convert_c2v(rrr->data[10], rrr->data[11]);
	its1->VacL_Trip = convert_c2v(rrr->data[12], rrr->data[13]);
	its1->VacL_Cycle = convert_c2v(rrr->data[14], rrr->data[15]);
	its1->Delta_Zac_Trip = convert_c2v(rrr->data[16], rrr->data[17]);
	its1->Zac_Trip = convert_c2v(rrr->data[18], rrr->data[19]);

	// Free memory.
	cleanup_read_req_response(rrr);

	return its1;
}

/**
	Reads the Trip Settings #2.
*/
struct INV_TRIP_SETTINGS_2 *read_trip_settings2(char address, int sp)
{
	struct INV_TRIP_SETTINGS_2 	*its2;
	struct READ_REQ 		*rr;
	struct READ_REQ_RESPONSE 	*rrr;

	char response[7*2+7];
	int response_len = 7*2+7;

	its2 = malloc(sizeof(struct INV_TRIP_SETTINGS_2));
	if (its2 == NULL) return NULL;

	// Generate Inverter Trip Settings Request.
	rr = generate_read_request(address, 0x0B, 7);
	if (rr == NULL) {
		free(its2);
		return NULL;
	}

	// Perform Request for ITS#2.
	perform_request(sp, "Inverter Trip Settings #2", rr, (char *) &response, &response_len);

	// Process the read request response.
	rrr = process_response(address, rr, response, response_len);
	if (rrr == NULL) {
		free(its2);
		return NULL;
	}

	// Designate data into its variable.
	its2->FastIEarth_Trip = convert_c2v(rrr->data[0], rrr->data[1]);
	its2->SlowIEarth_Trip = convert_c2v(rrr->data[2], rrr->data[3]);
	its2->Riso_Trip = convert_c2v(rrr->data[4], rrr->data[5]);
	its2->Vpv_Trip = convert_c2v(rrr->data[6], rrr->data[7]);
	its2->OnGrid_Delay = convert_c2v(rrr->data[8], rrr->data[9]);
	its2->VacH_Limit = convert_c2v(rrr->data[10], rrr->data[11]);
	its2->VacH_Limit_Cycle = convert_c2v(rrr->data[12], rrr->data[13]);

	// Free memory.
	cleanup_read_req_response(rrr);

	return its2;
}

/**
	Reads the device settings.
*/
struct INV_DEVICE_SETTINGS *read_device_settings(char address, int sp)
{
	struct INV_DEVICE_SETTINGS 	*ids;
	struct READ_REQ 		*rr;
	struct READ_REQ_RESPONSE 	*rrr;

	char response[4*2+7];
	int response_len = 4*2+7;

	ids = malloc(sizeof(struct INV_DEVICE_SETTINGS));
	if (ids == NULL) return NULL;

	// Generate Inverter Device Settings Request.
	rr = generate_read_request(address, 0x12, 4);
	if (rr == NULL) {
		free(ids);
		return NULL;
	}

	// Perform Request for IDS.
	perform_request(sp, "Inverter Device Settings", rr, (char *) &response, &response_len);

	// Process the read request response.
	rrr = process_response(address, rr, response, response_len);
	if (rrr == NULL) {
		free(ids);
		return NULL;
	}

	// Designate data into its variable.
	ids->Type_No = convert_c2v(rrr->data[0], rrr->data[1]);
	ids->Address = convert_c2v(rrr->data[2], rrr->data[3]);
	ids->Baudrate = convert_c2v(rrr->data[4], rrr->data[5]);
	ids->Language = convert_c2v(rrr->data[6], rrr->data[7]);

	// Free memory.
	cleanup_read_req_response(rrr);

	return ids;
}

/**
	Reads the Total Values.
*/
struct INV_TOTAL_VALUES *read_total_values(char address, int sp)
{
	struct INV_TOTAL_VALUES 	*itv;
	struct READ_REQ 			*rr;
	struct READ_REQ_RESPONSE 	*rrr;

	char response[15*2+7];
	int response_len = 15*2+7;
	int i;

	itv = malloc(sizeof(struct INV_TOTAL_VALUES));
	if (itv == NULL) return NULL;

	// Generate Inverter Total Values Request.
	rr = generate_read_request(address, 0x19, 15);
	if (rr == NULL) {
		free(itv);
		return NULL;
	}

	// Perform Inverter Total Values request.
	perform_request(sp, "Inverter Total Values", rr, (char *) &response, &response_len);

	// Process the read request response.
	rrr = process_response(address, rr, response, response_len);
	if (rrr == NULL) {
		free(itv);
		return NULL;
	}

	// Designate data into its variable.
	itv->BridgeRelay_On_Num = convert_c2v(rrr->data[0], rrr->data[1]);
	itv->BridgeRelay_On_Num = (itv->BridgeRelay_On_Num << 8) + convert_c2v(rrr->data[2], rrr->data[3]);
	itv->Time_Hr_Cnt = convert_c2v(rrr->data[4], rrr->data[5]);
	itv->Time_Min_Cnt = convert_c2v(rrr->data[6], rrr->data[7]);
	itv->Time_Sec_Cnt = convert_c2v(rrr->data[8], rrr->data[9]);
	itv->Eac = convert_c2v(rrr->data[10], rrr->data[11]);
	itv->Eac = (((double) itv->Eac) * 1000) + (((double)convert_c2v(rrr->data[12], rrr->data[13])) * 0.1);
	for (i=0; i<3; i++) {
		itv->Epv[i] = convert_c2v(rrr->data[16+i*6], rrr->data[17+i*6]);
		itv->Epv[i] = (((double) itv->Epv[i])*1000) + (((double) convert_c2v(rrr->data[18+i*6], rrr->data[19+i*6]))*0.1);
	}

	// Free memory.
	cleanup_read_req_response(rrr);

	return itv;
}

/**
	Reads a string value.
*/
char *read_string(char address, int sp, char ser_address, char ser_len, char *strDesc)
{
	char *strOut;
	struct READ_REQ 			*rr;
	struct READ_REQ_RESPONSE 	*rrr;
	int i, j;

	// Check that the string length is less than 15.
	if (ser_len > 15) ser_len = 15;
	else if (ser_len < 0) return NULL;

	char response[37];
	int response_len = (ser_len * 2) + 7;

	strOut = malloc(37);
	if (strOut == NULL) return NULL;
	for (i=0; i<37; i++) strOut[i] = 0;

	// Generate String Request.
	rr = generate_read_request(address, ser_address, ser_len);
	if (rr == NULL) {
		free(strOut);
		return NULL;
	}

	// Perform String Read.
	perform_request(sp, strDesc, rr, (char *) &response, &response_len);

	// Process the read request response.
	rrr = process_response(address, rr, response, response_len);
	if (rrr == NULL) {
		return NULL;
	}

	// Designate data into its variable.
	j = 0;
	for (i=0; i<ser_len; i++) {
		// Filter ASCII alphanumerics.
		if (((response[i] >= 65) && (response[i] <= 90)) || ((response[i] >= 97) && (response[i] <= 122)) || ((response[i] >= 48) && (response[i] <= 57))) {
			strOut[j] = response[i];
			j++;
		}
	}

	// Free memory.
	cleanup_read_req_response(rrr);

	return strOut;
}

/**
	Reads the Device Values.
*/
struct INV_DEVICE_VALUES *read_device_values(char address, int sp)
{
	struct INV_DEVICE_VALUES *idv;

	// Initialise memory.
	idv = malloc(sizeof(struct INV_DEVICE_VALUES));
	if (idv == NULL) return NULL;

	// Read the strings in from memory.
	idv->Brand_Name = read_string(address, sp, 0x67, 0x0F, "Brand Name");
	idv->Type_Name = read_string(address, sp, 0x6F, 0x0F, "Type Name");
	idv->Sn_Name = read_string(address, sp, 0x77, 0x0F, "SN Name");

	return idv;
}

/**
	Reads the current state.
*/
struct INV_CUR_STATE *read_current_state(char address, int sp)
{
	struct INV_CUR_STATE 		*ics;
	struct READ_REQ 			*rr;
	struct READ_REQ_RESPONSE 	*rrr;

	char response[5*2+7];
	int response_len = 5*2+7;
	int i;

	ics = malloc(sizeof(struct INV_CUR_STATE));
	if (ics == NULL) return NULL;

	// Generate Inverter Current State Request.
	rr = generate_read_request(address, 0xB5, 5);
	if (rr == NULL) {
		free(ics);
		return NULL;
	}

	// Perform Request for ICS.
	perform_request(sp, "Inverter Current State", rr, (char *) &response, &response_len);

	// Process the read request response.
	rrr = process_response(address, rr, response, response_len);
	if (rrr == NULL) {
		free(ics);
		return NULL;
	}

	// Designate data into its variable.
	ics->State = convert_c2v(rrr->data[0], rrr->data[1]);
	for (i=0; i<4; i++) {
		ics->Error_Code[i] = convert_c2v(rrr->data[2+(2*i)], rrr->data[3+(2*i)]);
	}

	// Free memory.
	cleanup_read_req_response(rrr);

	return ics;
}

/**
	Reads the extended current values of the Inverter.
*/
int read_current_values_ext(char address, int sp, struct INV_CUR_VALUES *icv)
{
	struct READ_REQ 			*rr;
	struct READ_REQ_RESPONSE 	*rrr;

	char response[3*2+7];
	int response_len = 3*2+7;

	if (icv == NULL) return -1;

	// Generate current values Request.
	rr = generate_read_request(address, 0xCC, 3);
	if (rr == NULL) return -2;

	// Perform current inverter values readings.
	perform_request(sp, "Current Inverter Values (Extended)", rr, (char *) &response, &response_len);

	// Process the read request response.
	rrr = process_response(address, rr, response, response_len);
	if (rrr == NULL) return -3;

	// Assign data into its variable.
	icv->Ton_today = ((double)convert_c2v(rrr->data[0], rrr->data[1]) / 2048);
	icv->Heatsink_Temp = ((double)convert_c2v(rrr->data[4], rrr->data[5])) / 10;

	// Free memory.
	cleanup_read_req_response(rrr);

	return 1;
}

/**
	Reads the current values of the Inverter.
*/
struct INV_CUR_VALUES *read_current_values(char address, int sp)
{
	struct INV_CUR_VALUES 		*icv;
	struct READ_REQ 			*rr;
	struct READ_REQ_RESPONSE 	*rrr;

	char response[15*2+7];
	int response_len = 15*2+7;
	int i;

	icv = malloc(sizeof(struct INV_CUR_VALUES));
	if (icv == NULL) return NULL;

	// Generate current values Request.
	rr = generate_read_request(address, 0xBA, 15);
	if (rr == NULL) {
		free(icv);
		return NULL;
	}

	// Perform current inverter values readings.
	perform_request(sp, "Current Inverter Values", rr, (char *) &response, &response_len);

	// Process the read request response.
	rrr = process_response(address, rr, response, response_len);
	if (rrr == NULL) {
		free(icv);
		return NULL;
	}

	// Assign data into its variable.
	for (i=0; i<3; i++) {
		icv->Vpv[i] = ((double) convert_c2v(rrr->data[i*2], rrr->data[1+(i*2)])) / 10;
		icv->Ppv[i] = convert_c2v(rrr->data[6+i*2], rrr->data[7+(i*2)]);
	}
	icv->Vac = ((double)convert_c2v(rrr->data[12], rrr->data[13])) / 10;
	icv->Pac = convert_c2v(rrr->data[14], rrr->data[15]);
	icv->Iac = ((double)convert_c2v(rrr->data[16], rrr->data[17])) / 10;
	icv->Fac = ((double)convert_c2v(rrr->data[18], rrr->data[19])) / 100;
	icv->Eac = convert_c2v(rrr->data[20], rrr->data[21]);
	icv->Eac = ((double)(icv->Eac * 1000)) + (((double)convert_c2v(rrr->data[22], rrr->data[23])) * 0.1);

	// Free memory.
	cleanup_read_req_response(rrr);

	// Get the extended values.
	read_current_values_ext(address, sp, icv);

	return icv;
}
