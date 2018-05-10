#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/time.h>
#include <time.h>
#include <stdlib.h>
#include <memory.h>
#include <math.h>
#include <string.h>
/* You will have to modify the program below */

#define MAXBUFSIZE 100

void get_time(struct timespec *spec, time_t *s0, long *ms0) {
    clock_gettime(CLOCK_REALTIME, spec);
    
    time_t s  = spec->tv_sec;
    long ms = round(spec->tv_nsec / 1.0e6); // Convert nanoseconds to milliseconds
    if (ms > 999) {
        s++;
        ms = 0;
    }
    *s0 = s;
    *ms0 = ms;
}

int main (int argc, char * argv[] )
{

    FILE *f;
	int sock;                           //This will be our socket
	struct sockaddr_in sin, remote;     //"Internet socket address structure"
	unsigned int remote_length;         //length of the sockaddr_in structure
	int nbytes;                        //number of bytes we receive in our message
	char buffer[MAXBUFSIZE];             //a buffer to store our received message
	if (argc < 2)
	{
		printf ("USAGE:  <port>\n");
		exit(1);
	}
    struct timespec spec;
    
    time_t s;
    long ms;
	/******************
	  This code populates the sockaddr_in struct with
	  the information about our socket
	 ******************/
	bzero(&sin,sizeof(sin));                    //zero the struct
	sin.sin_family = AF_INET;                   //address family
	sin.sin_port = htons(atoi(argv[1]));        //htons() sets the port # to network byte order
	sin.sin_addr.s_addr = INADDR_ANY;           //supplies the IP address of the local machine
    char servertime1[MAXBUFSIZE], servertime2[MAXBUFSIZE];
    
	//Causes the system to create a generic socket of type UDP (datagram)
	if ((sock = socket(sin.sin_family, SOCK_DGRAM, 0)) < 0)
	{
		printf("unable to create socket");
	}


	/******************
	  Once we've created a socket, we must bind that socket to the 
	  local address and port we've supplied in the sockaddr_in struct
	 ******************/
	if (bind(sock, (struct sockaddr *)&sin, sizeof(sin)) < 0)
	{
		printf("unable to bind socket\n");
	}

	remote_length = sizeof(remote);
    while(1) {
	//waits for an incoming message
        bzero(buffer,sizeof(buffer));
        nbytes = recvfrom(sock, buffer, MAXBUFSIZE, 0, (struct sockaddr*) &remote, &remote_length);
        if(strstr(buffer, "Done") != NULL) {
            break;
        }
        printf("Client says %s\n", buffer); // Receive T0 at T1
        get_time(&spec, &s, &ms);
        sprintf(servertime1, "%ld:%ld", s, ms);
        char temp[MAXBUFSIZE];
        strcpy(temp, servertime1);
        get_time(&spec, &s, &ms);
        sprintf(servertime2, "%ld:%ld", s, ms);
        //sprintf(servertime1, "%f", (float)clock()/CLOCKS_PER_SEC); // Record T1
        //sprintf(servertime2, "%f", (float)clock()/CLOCKS_PER_SEC); // Send T2
        nbytes = sendto(sock, servertime2, MAXBUFSIZE, 0, (struct sockaddr*)&remote, remote_length);
        char string[MAXBUFSIZE];
	char filename[MAXBUFSIZE];
	strcpy(filename, argv[2]);
	strcat(filename, "server.csv");
        f = fopen(filename, "a+");
        sprintf(string, "%s,%s", buffer, temp); // T0 and T1
        fprintf(f, "%s\n", string);
        fclose(f);
    }
    close(sock);
}

