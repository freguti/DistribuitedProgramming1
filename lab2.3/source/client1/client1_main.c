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

    char send_buf[BUFF_LENGTH];

	struct sockaddr_in saddr;

  	if(argc < 4)
  	{
    	printf("non sono stati specificati abbastanza parametri\n");
       	return -1;
  	}
    prog_name = argv[0];
	
	int n_file = 3;
	while(n_file < argc)
	{
		sock = Socket(PF_INET,SOCK_STREAM,IPPROTO_TCP);
		saddr.sin_family = AF_INET;
		saddr.sin_port = htons(atoi(argv[2])); //porta
		saddr.sin_addr.s_addr = inet_addr(argv[1]); //indirizzo
		Connect(sock,(struct sockaddr*)&saddr, sizeof(saddr));

	printf("(%s) connected with %s:%u\n",prog_name,inet_ntoa(saddr.sin_addr),ntohs(saddr.sin_port));
	memset(send_buf,0,BUFF_LENGTH);
	strcpy(send_buf,"GET");
	strcat(send_buf," ");
	strcat(send_buf,argv[n_file]);
	strcat(send_buf,"\r\n");
	printf("%s %ld\n",send_buf,strlen(send_buf));
	int s = send(sock,send_buf,strlen(send_buf),0);
	printf("caratteri inviati:%s\n",send_buf);
	fd_set cset;
	

	char *res = malloc(BUFF_LENGTH);
	struct timeval tval; //gestione del tempo
	int t = 1; //numero di secondi di attesa7


	int somma = 0;
	int f1;
	char *namefile = malloc(100);
	sprintf(namefile,"./client1/%s",argv[n_file]); //per far passare i test togliere il path ./client1/
	f1 = open(namefile,O_CREAT|O_TRUNC|O_WRONLY,S_IRUSR|S_IWUSR);
	n_file++;
	printf("aperto file\n");
	fflush(stdout);
	char tmp = ' ';
	i = 0;
	int len = 0;
	while(tmp != '\n')
	{
			fflush(stdout);
			i = read(sock,(void *)&tmp,1);
			if(i != -1)
				res[len++] = tmp;
	}
	res[len++] = '\0';
	//CONTROLLO SU +OK\r\n
	printf("stringa iniziale %s\n",res);
	memset(res,0,BUFF_LENGTH);
	tmp = ' ';
	i = 0;
	len = 0;
	uint32_t dimensione;
		recv(sock,&dimensione,4,0);
		printf("dim %u\n",dimensione);
	
		dimensione = ntohl(dimensione);
		printf("dimensione conv. %u\n",dimensione);
	len = 0;
	while(somma < dimensione)
	{
		FD_ZERO(&cset);
		FD_SET(sock,&cset);
		tval.tv_sec = 15; //assegno i secondi
		tval.tv_usec = 0; //microsecondi
		if(select(FD_SETSIZE,&cset,NULL,NULL,&tval)) //attendo una ricezione
		{
			memset(res,0,BUFF_LENGTH);
			if((dimensione - somma) >= BUFF_LENGTH )
			{
				len = read(sock,res,BUFF_LENGTH);
				if(len != BUFF_LENGTH)
					return -1;
			}
			else
			{
				len = read(sock,res,dimensione - somma);
				if(len != dimensione - somma)
					return -1;
			}
			if(write(f1,res,len) != len)
				return -1;
			somma += len;
		}
		else
		{
			printf("TIMEOUT!\n");
			return -1;
		}
	}
	printf("%d\n",somma);
	uint32_t timestamp;
		recv(sock,&timestamp,4,0);
		timestamp = ntohl(timestamp);
		printf("timestamp %u\n",timestamp);
	close(f1);
	close(sock);
	}
	
	return 0; //28/04/2019
}