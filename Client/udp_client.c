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
#include <stdlib.h>
#include <memory.h>
#include <math.h>
#include <time.h>
#include <errno.h>

#define MAXBUFSIZE 100

/* You will have to modify the program below */

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

int main (int argc, char * argv[])
{
    FILE *f;
	int nbytes;                             // number of bytes send by sendto()
	int sock;                               //this will be our socket
	char buffer[MAXBUFSIZE];
    int count = 1;
    int i;
    int limit = 240;
    struct timespec spec;
    
    time_t s;
    long ms;

	struct sockaddr_in remote;              //"Internet socket address structure"

	if (argc < 3)
	{
		printf("USAGE:  <server_ip> <server_port>\n");
		exit(1);
	}

    bzero(&remote,sizeof(remote));               //zero the struct
    remote.sin_family = AF_INET;                 //address family
    remote.sin_port = htons(atoi(argv[2]));      //sets port to network byte order
    remote.sin_addr.s_addr = inet_addr(argv[1]); //sets remote IP address
    
    //Causes the system to create a generic socket of type UDP (datagram)
    if ((sock = socket(remote.sin_family, SOCK_DGRAM, 0)) < 0)
    {
        printf("unable to create socket");
    }

	/******************
	  Here we populate a sockaddr_in struct with
	  information regarding where we'd like to send our packet 
	  i.e the Server.
	 ******************/

    i = 1;
    for(i = 1; i <= 720; i++) {
        /*if(i >= limit) {
            count++;
            
            break;
        }*/
        
        /******************
          sendto() sends immediately.
          it will report an error if the message fails to leave the computer
          however, with UDP, there is no error if the message is lost in the network once it leaves the computer.
         ******************/
        char clienttime1[MAXBUFSIZE], clienttime2[MAXBUFSIZE];
        
        get_time(&spec, &s, &ms);
        
        sprintf(clienttime1, "%ld:%ld", s, ms);
        
        
        //sprintf(clienttime1, "%f", (float)clock()/CLOCKS_PER_SEC); // Send T0
        nbytes = sendto(sock, clienttime1, MAXBUFSIZE, 0, (struct sockaddr*)&remote, sizeof(remote));
        // Blocks till bytes are received
        struct sockaddr_in from_addr;
        unsigned int addr_length = sizeof(struct sockaddr);
        bzero(buffer,sizeof(buffer));
        nbytes = recvfrom(sock, buffer, MAXBUFSIZE,0, (struct sockaddr*)&from_addr, &addr_length); // Receive T2
        //sprintf(clienttime2, "%f", (float)clock()/CLOCKS_PER_SEC);
        get_time(&spec, &s, &ms);
        sprintf(clienttime2, "%ld:%ld", s, ms);
        printf("%d: Server says %s\n", i, buffer);
        char string[MAXBUFSIZE];
	char filename[MAXBUFSIZE];
	strcpy(filename, argv[3]);
	strcat(filename, "client.csv");
        f = fopen(filename, "a+");
        sprintf(string, "%s,%s", buffer, clienttime2); // T2 and T3
        fprintf(f, "%s\n", string);
        fclose(f);

        sleep(10);
    }
    

    nbytes = sendto(sock, "Done", MAXBUFSIZE, 0, (struct sockaddr*)&remote, sizeof(remote));

    close(sock);

}

