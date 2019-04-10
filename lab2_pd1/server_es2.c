#include "./test_server/sockwrap.h"
#include <stdlib.h>
#include <stdio.h>
#include "string.h"
#include <ctype.h>
#include <sys/socket.h>
#include <netdb.h>
#include ""
#include "./../lab1_pd1/test_server/errlib.h"
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <errno.h>
#include <rpc/xdr.h>
#include <sys/time.h>

int main(int argc, char* argv[])
{
  int id_socket,err=-1;
  struct sockaddr_in saddr;
  if(argc == 4)
    {
      id_socket = socket(PF_INET,SOCK_DGRAM,IPPROTO_UDP);
      printf("id socket:%d\n",id_socket);
      
      saddr.sin_family = AF_INET;
      saddr.sin_port = htons(atoi(argv[2])); //porta
      inet_pton(AF_INET, argv[1], &saddr.sin_addr);
      /*
      err = connect(id_socket,(struct sockaddr*)&saddr, sizeof(saddr));
      printf("risultato connessione: %d\n", err);
      */
      //int send_size = 0;
      if(strlen(argv[3]) <= 31)
      {
	char *buffer = malloc(32*sizeof(char));
	char *res = malloc(100*sizeof(char));
	strcpy(buffer,argv[3]);
	printf("prova: %s\n",buffer);
	int inviati = sendto(id_socket,buffer,strlen(buffer),0,(struct sockaddr*)&saddr,sizeof(saddr));
	printf("spediti %d caratteri\n",inviati);
	unsigned int ricevuti = recvfrom(id_socket,res,sizeof(res),0,(struct sockaddr*)&saddr,(socklen_t*)(sizeof(saddr)));
	printf("ricevuti %d\n",ricevuti);
	err = close(id_socket);
	printf("risultato chiusura %d\n",err);
      }
    }
  return 0;
}