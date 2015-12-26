/**
	Author		:	Timothy Black
	Date		:	26th December 2015
	Description	:	Requests serial data from a Motech inverter via Serial, and
					sends it via Wi-Fi/Socket connection.
	Version		:	v0.8
*/

#include "global.h"

#define REQUEST_LENGTH		10	// Length of Motech requests.

char default_request[REQUEST_LENGTH] = {0x0A, 0x00, 0x03, 0x00, 0x17, 0x00, 0x02, 0x00, 0x00, 0x0D};

/**
	Calculates the CRC16 checksum required for all messages.

	Inputs: The message to send, number of characters to calculate for, and the offset
			to begin calculating for.
	Returns: The CRC16 Checksum.
*/
unsigned short calculate_crc16(unsigned char *message, unsigned char numChars, unsigned char offset)
{
	unsigned short crcOut;
	unsigned char bitPos;
	unsigned char flag;

	message += offset;
	crcOut = 0xFFFF;
	while (numChars > 0)
	{
		crcOut = crcOut ^ ((unsigned char)*message);
		bitPos = 0;
		while (bitPos <= 7)
		{
			flag = crcOut & 0x0001;

			crcOut = crcOut >> 1;
			crcOut = crcOut & 0x7FFF;

			if (flag != 0) crcOut = crcOut ^ 0xA001;

			bitPos++;
		}
		message++;
		numChars--;
	}

	// Reverse the two CRC bytes.
	crcOut = (crcOut >> 8) | (crcOut << 8);

	return (crcOut);
}

/**
	Generates a read request for the Motech protocol.
	
	Inputs: Generates the serial command for read registers.
	Returns: The serial sequence for a read request.
*/
struct READ_REQ *generate_read_request(char address, int start, int length)
{
	struct READ_REQ *read_req_out;
	char *request_out;
	unsigned short calcCRC;

	// Allocate memory for request.
	read_req_out = malloc(sizeof(struct READ_REQ));
	if (read_req_out == NULL) return NULL;
	
	request_out = malloc(REQUEST_LENGTH);
	if (request_out==NULL) {
		free(read_req_out);
		return NULL;
	}
	
	// Prepare request.
	memcpy(request_out, default_request, REQUEST_LENGTH);
	request_out[1] = address;

	// Set the start position and size of request.
	request_out[3] = ((start >> 8) & 0x00FF);
	request_out[4] = (start & 0x00FF);
	request_out[5] = ((length >> 8) & 0x00FF);
	request_out[6] = (length & 0x00FF);
	
	// Calculate checksum and include.
	calcCRC = calculate_crc16((unsigned char *)request_out, 6, 1);
	request_out[REQUEST_LENGTH-3] = ((calcCRC >> 8) & 0x00FF);
	request_out[REQUEST_LENGTH-2] = (calcCRC & 0x00FF);
	
	// Place inside structure.
	read_req_out->data_length = REQUEST_LENGTH;
	read_req_out->data = request_out;

	return read_req_out;
}

/**
	Generates a scan request for the Motech protocol.
	
	Inputs: Generates the serial command for search.
	Returns: The serial sequence for a search request.
*/
struct READ_REQ *generate_scan_request(char address)
{
	return generate_read_request(address, 0x17, 0x02);
}

/**
	Reads and validates a read request for the Motech protocol.
	
	Inputs: Validates the read request.
	Returns: The read request response.
*/
struct READ_REQ_RESPONSE *read_response_header(char address, unsigned char *response, int length)
{
	struct READ_REQ_RESPONSE *rrr;
	char data_len;
	unsigned short calc_CRC;

	// Error checking.
	if (response == NULL) return NULL;

	// Allocate memory space.
	rrr = malloc(sizeof(struct READ_REQ_RESPONSE));
	if (rrr == NULL) return NULL;

	// Initialise structure values.
	rrr->success = 0;
	rrr->data_length = 0;
	rrr->data = NULL;

	// Check for a valid header.
	if (response[0] != 0x0A) {
		if (verbose) fprintf(stderr, "The prefix(-1) was not found in the data returned.\n");
		rrr->success = -1;
		return rrr;
	}
	if (response[1] != address) {
		if (verbose) fprintf(stderr, "The response was not from the address queried.\n");
		rrr->success = -2;
		return rrr;
	}
	if (response[2] != 0x03) {
		if (verbose) fprintf(stderr, "The prefix(-3) was not found in the data returned.\n");
		rrr->success = -3;
		return rrr;
	}

	// Calculate length and CRC from header info.
	data_len = response[3];
	calc_CRC = calculate_crc16((unsigned char *)response, data_len+3, 1);

	// Verify length and CRC from header info.
	if (response[4+data_len+2] != 0x0D) {
		if (verbose) fprintf(stderr, "The end byte was not found in the data returned.\n");
		rrr->success = -4;
		return rrr;
	}
	if ( (((calc_CRC >> 8) & 0x00FF) != response[4+data_len]) || ((calc_CRC & 0x00FF) != response[5+data_len]) ) {
		if (verbose) fprintf(stderr, "The CRC received was invalid: %x != %x, %x != %x\n", ((calc_CRC >> 8) & 0x00FF), response[4+data_len], (calc_CRC & 0x00FF), response[5+data_len]);
		rrr->success = -5;
		return rrr;
	}

	// Set response, and return outcome.
	rrr->success = 1;
	rrr->data_length = data_len;

	rrr->data = malloc(data_len);
	if (rrr->data == NULL) {
		fprintf(stderr, "The memory could not be allocated for the response.\n");
		rrr->success = -6;
		return rrr;
	}
	memcpy(rrr->data, response+4, data_len);

	return rrr;
}

