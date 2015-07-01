#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#define ERR_EXIT(m) \
	do \
	{  \
		perror(m); \
		exit(EXIT_FAILURE); \
	} while(0)


void do_something(int conn)
{
     char recvbuf[1024];
     for(;;)
     {
	 memset(recvbuf,0,sizeof(recvbuf));
	 int ret = read(conn,recvbuf,sizeof(recvbuf));
	 if(ret == 0)
	 {
		 printf("client closed!\n");
		 break;
	 }
	 else if(ret == -1)
	 {
		 ERR_EXIT("read");
	 }
	 fputs(recvbuf,stdout);
	 write(conn,recvbuf,ret);
	 }
}
void handler(int sig)
{
	printf("recv a sig = %d\n",sig);
	exit(EXIT_SUCCESS);
}

int main()
{
	int listenfd;
	if((listenfd = socket(AF_INET,SOCK_STREAM,0)) < 0)
	  ERR_EXIT("socket");
	
	int on = 1;
	int ret = setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on) );
	 
	struct sockaddr_in servaddr;
	memset(&servaddr,0,sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(10001);
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

	if(bind(listenfd,(struct sockaddr*)&servaddr,sizeof(servaddr)) < 0)
	  ERR_EXIT("bind");
	
	if((listen(listenfd,SOMAXCONN)) < 0 )//主动套接字，变成被动套接字
	  ERR_EXIT("listen");
     
	struct sockaddr_in peeraddr;
	socklen_t socklen = sizeof(peeraddr);
	int conn;
	pid_t  pid;
	if((conn = accept(listenfd,(struct sockaddr*)&peeraddr,&socklen)) < 0)// 获得到是主动套接字
	    ERR_EXIT("accept");
     printf("ip:%s   port:%d\n",inet_ntoa(peeraddr.sin_addr),ntohs(peeraddr.sin_port));       
	 	
		pid = fork();
		char sendbuf[1024] = {0};
		if(pid == -1) ERR_EXIT("pid");
		if(pid == 0)
		{
			signal(SIGUSR1,handler);
			while(fgets(sendbuf,sizeof(sendbuf),stdin) != NULL)
			{
				write(conn,sendbuf,strlen(sendbuf));				
				memset(sendbuf,0,sizeof(sendbuf));
			}
			printf("child closed\n");
			exit(EXIT_SUCCESS);
		}
		else
		{

			char recvbuf[1024];
			while(1)
			{
				memset(recvbuf,0,sizeof(recvbuf));
				int ret = read(conn,recvbuf,sizeof(recvbuf));
				if(ret  == -1)
				  ERR_EXIT("read");
				else if(ret == 0)
				{ 
				  printf("peer close\n");
				  break;
				}
				fputs(recvbuf,stdout);
			}
			printf("kill parent!\n");
			kill(pid,SIGUSR1);
			exit(EXIT_SUCCESS);
			//do_something(conn);
		
		}
		close(conn);
		close(listenfd);
		exit(0);
}
