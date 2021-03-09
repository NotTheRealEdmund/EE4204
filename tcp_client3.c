/*******************************
tcp_client.c: the source file of the client in tcp transmission 
********************************/

#include "headsock.h"

// Function to send data packets (DUs) and receive ACKs
float str_cli(FILE *fp, int sockfd, long *len);

// Calculate the time interval between out and in
void tv_sub(struct  timeval *out, struct timeval *in);

int main(int argc, char **argv)
{
	int sockfd;
	float ti, rt;
	long len;
	struct sockaddr_in ser_addr;
	char ** pptr;
	struct hostent *sh;
	struct in_addr **addrs;
	FILE *fp;

	if (argc != 2) {
		printf("parameters not match");
	}

	// Get host's information
	sh = gethostbyname(argv[1]);
	if (sh == NULL) {
		printf("error when gethostby name");
		exit(0);
	}

	// Print the remote host's information
	printf("canonical name: %s\n", sh->h_name);
	for (pptr=sh->h_aliases; *pptr != NULL; pptr++)
		printf("the aliases name is: %s\n", *pptr);
	switch(sh->h_addrtype)
	{
		case AF_INET:
			printf("AF_INET\n");
		break;
		default:
			printf("unknown addrtype\n");
		break;
	}
        
	addrs = (struct in_addr **)sh->h_addr_list;
	// Create the socket
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockfd <0)
	{
		printf("error in socket");
		exit(1);
	}
	ser_addr.sin_family = AF_INET;                                                      
	ser_addr.sin_port = htons(MYUDP_PORT);
	memcpy(&(ser_addr.sin_addr.s_addr), *addrs, sizeof(struct in_addr));
	bzero(&(ser_addr.sin_zero), 8);
	
	// Connect the socket with the host
	if (connect(sockfd, (struct sockaddr *)&ser_addr, sizeof(struct sockaddr)) != 0) {
		printf ("connection failed\n"); 
		close(sockfd); 
		exit(1);
	}
	
	if((fp = fopen ("myfile.txt","r+t")) == NULL)
	{
		printf("File doesn't exist\n");
		exit(0);
	}

	// Perform the transmission and receiving
	ti = str_cli(fp, sockfd, &len);
	// Caculate the average transmission rate
	rt = (len/(float)ti);
	printf("Time(ms) : %.3f, Data sent(byte): %d\nData rate: %f (Kbytes/s)\n", ti, (int)len, rt);

	close(sockfd);
	fclose(fp);
	exit(0);
}

float str_cli(FILE *fp, int sockfd, long *len)
{
	char *buf;
	long lsize, ci;
	char sends[DATALEN];
	struct ack_so ack;
	int n, slen;
	float time_inv = 0.0;
	struct timeval sendt, recvt;
	ci = 0;
	
	// Keep track of how many DUs to send, 0 means send 1DU, 1 means send 2DUs, 2 means send 3 DUs
	int x = 0;

	fseek (fp , 0 , SEEK_END);
	lsize = ftell (fp);
	rewind (fp);
	printf("The file length is %d bytes\n", (int)lsize);
	printf("the packet length is %d bytes\n",DATALEN);

	// Allocate memory to contain the whole file
	buf = (char *) malloc (lsize);
	if (buf == NULL) exit (2);

  	// Copy the file into the buffer
	fread (buf,1,lsize,fp);

  	/*** the whole file is loaded in the buffer. ***/
	// Append the end byte
	buf[lsize] ='\0';
	// Get the current time
	gettimeofday(&sendt, NULL);
	while(ci<= lsize)
	{
		// Send 1DU
		if (x == 0) {
			if ((lsize+1-ci) <= DATALEN)
				slen = lsize+1-ci;
			else 
				slen = DATALEN;
			memcpy(sends, (buf+ci), slen);
			// Send the data
			n = send(sockfd, &sends, slen, 0);
			if(n == -1) {
				printf("send error!");
				exit(1);
			}
			ci += slen;
		} 
		// Send 2DUs
		else if (x == 1) {
			for (int i = 0; i < 2; i++)
			{
				if ((lsize+1-ci) <= DATALEN)
					slen = lsize+1-ci;
				else 
					slen = DATALEN;
				memcpy(sends, (buf+ci), slen);
				// Send the data
				n = send(sockfd, &sends, slen, 0);
				if(n == -1) {
					printf("send error!");
					exit(1);
				}
				ci += slen;
			}
		} 
		// Send 3DUs
		else if (x == 2) {
			for (int i = 0; i < 3; i++)
            		{
				if ((lsize+1-ci) <= DATALEN)
					slen = lsize+1-ci;
				else 
					slen = DATALEN;
				memcpy(sends, (buf+ci), slen);
				// Send the data
				n = send(sockfd, &sends, slen, 0);
				if(n == -1) {
					printf("send error!");
					exit(1);
				}
				ci += slen;
			}
		}
		
		// Increment x every time an ACK is received, reset after 3 ACKs
		x++;
		if (x == 3) {
			x = 0;	
		}
		
		// Receive the ACK
		if ((n= recv(sockfd, &ack, 2, 0))==-1)
		{
			printf("error when receiving\n");
			exit(1);
		}
		if (ack.num != 1|| ack.len != 0)
			printf("error in transmission\n");
	}
	
	gettimeofday(&recvt, NULL);
	*len= ci;                                                         //get current time
	tv_sub(&recvt, &sendt);                                                                 // get the whole trans time
	time_inv += (recvt.tv_sec)*1000.0 + (recvt.tv_usec)/1000.0;
	return(time_inv);
}

void tv_sub(struct  timeval *out, struct timeval *in)
{
	if ((out->tv_usec -= in->tv_usec) <0)
	{
		--out ->tv_sec;
		out ->tv_usec += 1000000;
	}
	out->tv_sec -= in->tv_sec;
}
