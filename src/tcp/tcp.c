/*
 * tcp.c
 * Author: Fabio Crestani
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>
#include <signal.h>

#include "mqtt_fc.h"
#include "logger.h"
#include "circular_buffer.h"

int sockfd;
struct sockaddr_in serv_addr;
struct hostent *server;

uint8_t connected = FALSE;

CircularBuffer *circular_buffer;

uint8_t tcp_connect(char server_address[], uint32_t server_port)
{
	fd_set fdset;
	struct timeval tv;
	char temp[512];

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        logger_log_tcp(TYPE_ERROR, "ERROR opening socket");
		connected = FALSE;
		return FALSE;
    }

    fcntl(sockfd, F_SETFL, O_NONBLOCK);
    server = gethostbyname(server_address);

    if (server == NULL)
    {
        logger_log_tcp(TYPE_ERROR, "ERROR: No such host");
		connected = FALSE;
        return FALSE;
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *) &serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(server_port);
    
	connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
        
	FD_ZERO(&fdset);
    FD_SET(sockfd, &fdset);
    tv.tv_sec = TCP_DEFAULT_CONNECT_TIMEOUT_S; // default: 10s
    tv.tv_usec = 0;

	// Disable SIGPIPE when the connection is lost
	// Reference: https://stackoverflow.com/questions/108183/how-to-prevent-sigpipes-or-handle-them-properly
	signal(SIGPIPE, SIG_IGN);

	if (select(sockfd + 1, NULL, &fdset, NULL, &tv) == 1)
    {
        int so_error;
        socklen_t len = sizeof(so_error);

        getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &so_error, &len);
        if (so_error == 0) 
		{
			connected = TRUE;
            return TRUE;
        }
    }

	sprintf(temp, "Error connecting to server %s:%d", 
		server_address, server_port);
	logger_log_tcp(TYPE_ERROR, temp);
	sprintf(temp, "TCP connect exceeded max timeout (%d seconds)",
		TCP_DEFAULT_CONNECT_TIMEOUT_S);
	logger_log_tcp(TYPE_ERROR, temp);

	connected = FALSE;
	return FALSE;
}

void tcp_set_circular_buffer(CircularBuffer *ptr_buffer)
{
	circular_buffer = ptr_buffer;
}

void tcp_set_socket_non_blocking(void)
{
	fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFL, 0) | O_NONBLOCK);
}

uint8_t tcp_is_connected(void)
{
	return connected;
}

uint8_t tcp_disconnect(void)
{
	int ret = shutdown(sockfd, 2);
	connected = FALSE;
	return (ret == 0);
}

uint8_t tcp_send(const char buffer[], const uint32_t len)
{
	int n = 0;
	uint32_t blocks = 0;

	while (n < (int) len)
	{
	    n += write(sockfd, buffer, len);
	    if (n < 0)
	    {
	        logger_log_tcp(TYPE_ERROR, "ERROR writing to socket");
			connected = FALSE;
			return FALSE;
	    }

		if (blocks++ > 2048)
		{
			logger_log_tcp(TYPE_ERROR, 
				"ERROR writing to socket: Possible infinite loop?");
			return FALSE;
		}
	}

	#ifdef TCP_VERBOSE
		printf("[tcp] %d bytes sent\n", len);
	#endif

	return TRUE;
}

uint8_t tcp_receive(char buffer[], uint32_t *len)
{
	int ret = read(sockfd, buffer, 255);
    if (*len <= 0)
    {
        //printf("ERROR reading from socket\n");
		return FALSE;
    }
    *len = ret;
    return TRUE;
}

void tcp_poll_rx(void)
{
	char buffer[256];
	int len = 0;

	if (circular_buffer == NULL)
	{
		return;
	}

	do {
		len = read(sockfd, buffer, 255);

		if (len > 0)
		{
			buffer_put_array(circular_buffer, buffer, len);
			#ifdef TCP_VERBOSE
				printf("[tcp] %d bytes received\n", len);
			#endif
		}

	} while (len > 0);
}

void tcp_poll(void)
{
	tcp_poll_rx();
}

