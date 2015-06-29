#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#define ERR_EXIT(m) \
	do \
	{  \
		perror(m); \
		exit(EXIT_FAILURE); \
	} while(0)

int main()
{
	int listenfd;
	if((listenfd = socket(AF_INET,SOCK_STREAM,0)) < 0)
	  ERR_EXIT("socket");
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
	if((conn = accept(listenfd,(struct sockaddr*)&peeraddr,&socklen)) < 0)// 获得到是主动套接字
	  ERR_EXIT("accept");
     
	char recvbuf[1024];
    for(;;)
	{
	
		memset(recvbuf,0,sizeof(recvbuf));
		int ret = read(conn,recvbuf,sizeof(recvbuf));
		fputs(recvbuf,stdout);
		write(conn,recvbuf,ret);
	}
    exit(0);
}
