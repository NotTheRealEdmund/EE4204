/**********************************
tcp_ser.c: the source file of the server in tcp transmission 
***********************************/


#include "headsock.h"

void str_ser(int sockfd);                                                        // transmitting and receiving function

int main(void)
{
	int sockfd;
	struct sockaddr_in my_addr;

	sockfd = socket(AF_INET, SOCK_DGRAM, 0);          //create socket
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
		str_ser(sockfd);
	}
	close(sockfd);
	exit(0);
}

void str_ser(int sockfd)
{
	char buf[BUFSIZE];
	FILE *fp;
	char recvs[DATALEN];
	struct ack_so ack;
	int end, n = 0;
	long lseek=0;
	end = 0;
	int x = 0; // 0 after 1DU, 2 after 2 DUs, 5 after 3DUs
	
	printf("receiving data!\n");

	while(!end)
	{
		if ((n= recv(sockfd, &recvs, DATALEN, 0))==-1)                                   //receive the packet
		{
			printf("error when receiving\n");
			exit(1);
		}
		if (recvs[n-1] == '\0')									//if it is the end of the file
		{
			end = 1;
			n --;
		}
		memcpy((buf+lseek), recvs, n);
		lseek += n;
		
		if (x == 0 || x == 2 || x == 5) // send ack
		{
		    ack.num = 1;
		    ack.len = 0;
		    if ((n = send(sockfd, &ack, 2, 0))==-1)
		    {
			printf("send error!");
			exit(1);
		    }
		}
		x++;
		if (x == 5) 
		{
			x = 0;	
		}
	}
	
	if ((fp = fopen ("myTCPreceive.txt","wt")) == NULL)
	{
		printf("File doesn't exit\n");
		exit(0);
	}
	fwrite (buf , 1 , lseek , fp);					//write data into file
	fclose(fp);
	printf("a file has been successfully received!\nthe total data received is %d bytes\n", (int)lseek);
}
