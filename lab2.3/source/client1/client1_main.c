#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <ctype.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <rpc/xdr.h>

#include <string.h>
#include <time.h>
#include <unistd.h>

#include "./../errlib.h"
#include "./../sockwrap.h"

#define BUFF_LENGTH 1500
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
	
	fd_set cset;

	char *res = malloc(1500*sizeof(char));
	struct timeval tval; //gestione del tempo
	int t = 15; //numero di secondi di attesa7

	FD_ZERO(&cset);
	FD_SET(sock,&cset);
	tval.tv_sec = t; //assegno i secondi
	tval.tv_usec = 0; //microsecondi

	//if(select(FD_SETSIZE,&cset,NULL,NULL,&tval)) //attendo una ricezione
	//{

		int ricevuti=0,somma = 0;
		while((ricevuti = recv(sock,res,1,0)) != -1)
		{
			somma ++;
			if((somma = somma % 1500) == 0)
				printf("yep\n");
		}
	//}
	//else
	//{
	//	printf("TIMEOUT!\n");
	//}
	
}