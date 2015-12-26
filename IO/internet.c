/**
	Author		:	Timothy Black
	Date		:	26th December 2015
	Description	:	Requests serial data from a Motech inverter via Serial, and
					sends it via Wi-Fi/Socket connection.
	Version		:	v0.8
*/

#include "../Application/global.h"

/**
	Resolves IP Address for a hostname.
	
	Inputs	: Hostname to connect to.
	Returns : An IP address.
*/
char *get_ip_for_hostname(char *hostname)
{
	struct hostent *hent;
	char *ip_addr;

	// Memory allocations.
	ip_addr = (char *)malloc(16);
	memset(ip_addr, 0, 16);

	// Get the IP address by the hostname.
	if((hent = gethostbyname(hostname)) == NULL)
	{
		fprintf(stderr, "Cannot resolve IP address for: %s\n", hostname);
		free(ip_addr);
		return NULL;
	}

	// Check the IP Address for the website.
	if(inet_ntop(AF_INET, (void *)hent->h_addr_list[0], ip_addr, 15) == NULL)
	{
		fprintf(stderr, "Invalid IP Address for %s.\n", hostname);
	}

	return ip_addr;
}

/**
	Send response via GET for HTTP.
	
	Inputs: The hostname and the HTTP request.
*/
void send_response_http(char *hostname, char *http_request)
{
	int retVal;
	int fd;
	int tmpip;
	char *ip_addr;
	struct sockaddr_in *sckaddr;

	printf("Connecting to %s.\n", hostname);
	
	// Create socket.
	fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (fd == -1)
	{
		fprintf(stderr, "Cannot connect to Internet socket.\n");
		return;
	}

	// Get the IP address for the hostname.
	ip_addr = get_ip_for_hostname(hostname);
	if (ip_addr == NULL)
	{
		close(fd);
		return;
	}

	// Initialise the socket address structure.
	sckaddr = (struct sockaddr_in *) malloc(sizeof(struct sockaddr_in *));
	sckaddr->sin_family = AF_INET;

	// Confirm the IP address is good, and set the destination port to port 80.
  	tmpip = inet_pton(AF_INET, ip_addr, (void *)(&(sckaddr->sin_addr.s_addr)));
	if(tmpip <= 0)  
	{
		fprintf(stderr, "Invalid IP address resolved.\n");
		close(fd);
		free(ip_addr);
		return;
	}
	sckaddr->sin_port = htons(80);

	// Connect to the socket.
	if (connect(fd, (struct sockaddr *)sckaddr, sizeof(struct sockaddr)) < 0)
	{
		perror("Cannot connect to Internet socket correctly.");
		close(fd);
		free(ip_addr);
		return;
	}

	printf("Connection opened successfully.\n");

	// Write the HTTP request to the socket.
	retVal = write(fd, http_request, strlen(http_request));

	// Close the connection.
	shutdown(fd, SHUT_RDWR);
	close(fd);
	free(ip_addr);

	printf("Connection closed successfully.\n");
}

/**
	Send the HTTP response to the pvoutput website.
	
	Inputs: The inverter info.
*/
void send_response_http_pvoutput(struct INVERTER_INFO *inv_info)
{
	char strRequest[BUFSIZ];

	// Prepare the GET request for pvoutput.
	sprintf(strRequest, "GET http://pvoutput.org/service/r2/addstatus.jsp?key=%s&sid=%s&d=%s&t=%s&v2=%d&v6=%f&c1=%ld HTTP/1.1\r\nHost: www.pvoutput.org\r\n\r\n", pvo_api_key, pvo_sys_id, inv_info->dt.date, inv_info->dt.time, (int) inv_info->icv->Pac, inv_info->icv->Vac, (long) inv_info->itv->Eac);
	
	// Send the response to pvoutput.
	send_response_http("www.pvoutput.org", strRequest);
}
