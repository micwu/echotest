#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <signal.h>

#define ERR_EXIT(m) \
	 do \
     {  \
           perror(m); \
	       exit(EXIT_FAILURE); \
	 } while(0)

void handler(int sig)
{
	printf("recv a sig = %d\n",sig);
	exit(EXIT_SUCCESS);
}


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
	pid_t pid;
	pid = fork();
	if(pid == -1)
		ERR_EXIT("fork");
	if(pid == 0)
	{
		while(1)
		{
			memset(recvbuf,0,sizeof(recvbuf));
			int ret = read(sockfd,recvbuf,sizeof(recvbuf));
			if(ret == -1)
				ERR_EXIT("read");
			else if (ret == 0)
			{
				printf("peer closed\n");
				break;
			}
			fputs(recvbuf,stdout);
		}

		printf("child close\n");
		kill(pid,SIGUSR1);
		exit(EXIT_SUCCESS);
	}
	else
	{		
		signal(SIGUSR1,handler);
		while(fgets(sendbuf,sizeof(sendbuf),stdin) != NULL)
        {
			write (sockfd,sendbuf,strlen(sendbuf));  
			memset(sendbuf,0,sizeof(sendbuf));
		}
		printf("parent close!\n");
	}
	close(sockfd);
	exit(0);
}
