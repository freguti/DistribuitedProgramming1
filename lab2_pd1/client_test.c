#include "./../include/sockwrap.h"
#include <stdlib.h>
#include <stdio.h>
#include "string.h"
#include <ctype.h>
#include <sys/socket.h>
#include <netdb.h>
#include "./../include/errlib.h"
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <errno.h>
#include <rpc/xdr.h>
#include <sys/time.h>
#include <time.h>

#define SEND_BUFF_SIZE 32
#define RECV_BUFF_SIZE 100

char* prog_name;

int main(int argc, char* argv[])
{
  //char *buffer = malloc(SEND_BUFF_SIZE*sizeof(char));
  char *res = malloc(RECV_BUFF_SIZE*sizeof(char));
  int id_socket;
  int err = -1;
  int inviati;
  unsigned int ricevuti;
  int tentativi = 4;

  fd_set cset;

  struct sockaddr_in saddr;
  struct timeval tval; //gestione del tempo
  int t = 5; //numero di secondi di attesa
  char send_number[10]; //numero su 32 bit
  time_t ti;

  if(argc != 3)
    return -1;
  id_socket = socket(PF_INET,SOCK_DGRAM,IPPROTO_UDP);
  printf("id socket:%d\n",id_socket);
  
  saddr.sin_family = AF_INET;
  saddr.sin_port = htons(atoi(argv[2])); //porta
  inet_pton(AF_INET, argv[1], &saddr.sin_addr);
  time(&ti);
  uint32_t timestamp = htonl(ti); //non me lo prende unsigned
  sprintf(send_number,"%u",timestamp);
  printf("prova: %s\n",send_number);
  while(tentativi < 5)
  {
    inviati = sendto(id_socket,send_number,strlen(send_number),0,(struct sockaddr*)&saddr,sizeof(saddr));
    printf("spediti %d caratteri\n",inviati);
    FD_ZERO(&cset);
    FD_SET(id_socket,&cset);
    tval.tv_sec = t; //assegno i secondi
    tval.tv_usec = 0; //microsecondi

    if(select(FD_SETSIZE,&cset,NULL,NULL,&tval)) //attendo una ricezione
    {
      ricevuti = read(id_socket,res,RECV_BUFF_SIZE * sizeof(char));
      printf("ricevuti %d %s\n",ricevuti,res);
      tentativi += 10;
    }
    else
    {
      printf("TIMEOUT!\n");
      tentativi ++;
    }
  }
  if(tentativi >= 10)
    printf("SUCCESSO AL %d TENTATIVO\n",tentativi-10+1);
  else
    printf("ERRORE DI INVIO; ECCEDUTI I 5 TENTATIVI\n");    
        
  err = close(id_socket);
  printf("risultato chiusura %d\n",err);

  return 0;
}