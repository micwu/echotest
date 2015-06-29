#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>

#define ERR_EXIT(m) \
	 do \
     {  \
           perror(m); \
	       exit(EXIT_FAILURE); \
	 } while(0)


int main(int argc,char *argv[])
{
	int sockfd;
	if((sockfd = socket(AF_INET,SOCK_STREAM,0)) < 0)
	  ERR_EXIT("socket");
	struct sockaddr_in servaddr;
	memset(&servaddr,0,sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(10001);
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	if(connect(sockfd,(struct sockaddr*)&servaddr,sizeof(servaddr)) < 0)
	  ERR_EXIT("connect");

	char sendbuf[1024] = {0};
	char recvbuf[1024] = {0};

	while(fgets(sendbuf,sizeof(sendbuf),stdin) != NULL)
	{
		write (sockfd,sendbuf,strlen(sendbuf));
		read(sockfd,recvbuf,sizeof(recvbuf));
		
		fputs(recvbuf,stdout);

		memset(sendbuf,0,sizeof(sendbuf));
		memset(recvbuf,0,sizeof(recvbuf));
	}
	close(sockfd);
	exit(0);
}
