/**********************************
stop_ser.c: the source file of the server in udp transmission with stop-and-wait protocol where the ACK returned is always fixed to be 1
***********************************/

#include "headsock.h"

// Function to send ACKs and receive data packets (DUs)
void str_ser(int sockfd, struct sockaddr *addr, int addrlen);

int main(void)
{
	int sockfd;
	struct sockaddr_in my_addr;
	
	// Create udp socket
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockfd <0)
	{
		printf("error in socket!");
		exit(1);
	}
	
	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(MYUDP_PORT);
	my_addr.sin_addr.s_addr = htonl(INADDR_ANY);//inet_addr("172.0.0.1");
	bzero(&(my_addr.sin_zero), 8);
	if (bind(sockfd, (struct sockaddr *) &my_addr, sizeof(struct sockaddr)) <0)
	{
		printf("error in binding");
		exit(1);
	}

	while (1)
	{
		printf("waiting for data\n");
		str_ser(sockfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr_in));
	}
	close(sockfd);
	exit(0);
}

void str_ser(int sockfd, struct sockaddr *addr, int addrlen)
{
	char buf[BUFSIZE];
	FILE *fp;
	char recvs[DATALEN];
	struct ack_so ack;
	int end, n = 0;
	long lseek=0;
	end = 0;
	
	// Keep track of when to send ACK, 0 means send after 1DU, 2 means send after 2 DUs, 5 means send after 3DUs
	int x = 0;
	
	printf("receiving data!\n");

	while(!end)
	{
		// Receive the packet
		if ((n= recvfrom(sockfd, &recvs, DATALEN, 0, addr, (socklen_t *)&addrlen))==-1)
		{
			printf("error when receiving\n");
			exit(1);
		}
		
		// If it is the end of the file
		if (recvs[n-1] == '\0')
		{
			end = 1;
			n --;
		}
		memcpy((buf+lseek), recvs, n);
		lseek += n;
		
	        ack.num = 1;
	        ack.len = 0;
	        if ((n = sendto(sockfd, &ack, 2, 0, addr, addrlen))==-1)
	        {
		    printf("send error!");
		    exit(1);
	        }
	}

	if ((fp = fopen ("myUDPreceive.txt","wt")) == NULL)
	{
		printf("File doesn't exist\n");
		exit(0);
	}
	// Write data into file
	fwrite (buf , 1 , lseek , fp);
	fclose(fp);
	printf("A file has been successfully received!\nthe total data received is %d bytes\n", (int)lseek);
}
