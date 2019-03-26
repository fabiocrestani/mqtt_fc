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

#include "mqtt_fc.h"
#include "logger.h"

int sockfd, portno = 1883, n;
struct sockaddr_in serv_addr;
struct hostent *server;

uint8_t tcp_connect()
{
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        printf("ERROR opening socket\n");
		return FALSE;
    }

    server = gethostbyname("iot.eclipse.org");

    if (server == NULL)
    {
        printf("ERROR, no such host\n");
        return FALSE;
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *) &serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portno);
    
	if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
    {
        printf("ERROR connecting");
		return FALSE;
    }

	return TRUE;
}

uint8_t tcp_send(char buffer[], uint32_t len)
{
	int n;
    n = write(sockfd, buffer, len);
    if (n < 0)
    {
        printf("ERROR writing to socket");
		return FALSE;
    }

	return TRUE;
}

uint8_t tcp_receive(char buffer[], uint32_t *len)
{
    int n = read(sockfd, buffer, 255);
    if (n < 0)
    {
        printf("ERROR reading from socket");
		return FALSE;
    }

	*len = n;

    return TRUE;
}
