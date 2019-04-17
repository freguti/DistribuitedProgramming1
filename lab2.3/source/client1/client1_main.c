#include "./../sockwrap.h"
#include <stdlib.h>
#include <stdio.h>
#include "string.h"
#include <ctype.h>
#include <sys/socket.h>
#include <netdb.h>
#include "./../errlib.h"
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <errno.h>
#include <rpc/xdr.h>
#include <sys/time.h>


char* prog_name;

int main(int argc, char* argv[])
{
	int sock;
	int i;

	char send_buf[5];
	strcpy(send_buf,"GET");

  	struct sockaddr_in saddr;

  	if(argc < 4)
  	{
		printf("non sono stati specificati abbastanza parametri\n");
	  	return -1;
  	}
	prog_name = argv[0];
	for(i = 3;i<argc;i++)
	{
		strcat(send_buf," ");
		strcat(send_buf,argv[i]);
	}
	sock = Socket(PF_INET,SOCK_STREAM,IPPROTO_TCP);
	
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(atoi(argv[2])); //porta
	saddr.sin_addr.s_addr = inet_addr(argv[1]); //indirizzo
    Connect(sock,(struct sockaddr*)&saddr, sizeof(saddr));

	printf("(%s) connected with %s:%u\n",prog_name,inet_ntoa(saddr.sin_addr),ntohs(saddr.sin_port));
	Send(sock,send_buf,strlen(send_buf)*sizeof(char),0);
	//filename = strtok(recv_buffer," ");
}