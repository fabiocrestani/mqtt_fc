/*
 * main.c
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

void error(char *msg)
{
    perror(msg);
    exit(0);
}

void dump(char *data, uint32_t len)
{
	char temp[9];
	uint32_t temp_idx = 0;
	uint32_t byte_counter = 0;

	printf("\n  ");

	for (uint32_t i = 0; i < 8; i++)
	{
		printf("    %d", i);
    }

	printf("\n%d: ", byte_counter);

	for (uint32_t i = 0; i < len; i++)
	{
		printf("0x%02x ", data[i]);

		temp[temp_idx++] = data[i];
	
		if ((i > 0) && ((i % 7) == 0))
		{
			printf("\t");
			for (uint32_t j = 0; j < temp_idx; j++)
			{
				if ((temp[j] > 33) && (temp[j] < 127))
				{
					printf("%c", temp[j]);
				}
				else
				{
					printf(".");
				}
			}
			byte_counter = byte_counter + 8;
			printf("\n%d: ", byte_counter);
			temp_idx = 0;
		}
	}
	
	for (uint32_t j = temp_idx; j < 8; j++)
	{
		printf(".... ");
	}

	printf("\t");
	for (uint32_t j = 0; j < temp_idx; j++)
	{
		if ((temp[j] > 33) && (temp[j] < 127))
		{
			printf("%c", temp[j]);
		}
		else
		{
			printf(".");
		}
	}
	printf("\n\n");

}


int main(int argc, char *argv[])
{
    int sockfd, portno, n;

    struct sockaddr_in serv_addr;
    struct hostent *server;
    char buffer[256] = {0x10, 10, 0x00, 0x04, 'M', 'Q', 'T', 'T', \
0x04, 0x00, 0x00, 0x00};
	unsigned int len = 12;

    portno = 1883;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        error("ERROR opening socket");
    }

    server = gethostbyname("iot.eclipse.org");
    if (server == NULL)
    {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *) &serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
    {
        error("ERROR connecting");
    }

	printf("TCP connected to %s:%d\n", "iot.eclipse.org", portno);

	printf("Trying to write %u bytes to socket\n", len);

	dump(buffer, len);

    n = write(sockfd, buffer, len);
    if (n < 0)
    {
         error("ERROR writing to socket");
    }

    n = read(sockfd, buffer, 255);
    if (n < 0)
    {
         error("ERROR reading from socket");
    }

    printf("n:%d %s\n", n, buffer);

    for (int i = 0; i < n; i++)
    {
    	printf("0x%02x ", buffer[i]);
    	if ((i % 10) == 0)
    	{
    		printf("\n");
    	}
    }

    return 0;
}
