#include "./test_server/sockwrap.h"
#include <stdlib.h>
#include <stdio.h>
#include "string.h"
//#include <ctype.h>
//#include <sys/socket.h>
//#include <netdb.h>
//#include "./test_server/errlib.h"
//#include <netinet/in.h>
//#include <arpa/inet.h>
//#include <sys/types.h>
//#include <errno.h>

int main(int argc, char* argv[])
{
  int id_socket,err=-1,addr;
  struct sockaddr_in saddr;
  if(argc == 3)
    {
      id_socket = socket(PF_INET,SOCK_STREAM,IPPROTO_TCP);
      //if(id_socket == INVALID_SOCKET)
      //return -1;
      printf("id socket:%d\n",id_socket);
      saddr.sin_family = AF_INET;
      saddr.sin_port = htons(atoi(argv[2])); //porta
      inet_pton(AF_INET, argv[1], &saddr.sin_addr);
      err = connect(id_socket,(struct sockaddr*)&saddr, sizeof(saddr));
      printf("risultato connessione: %d\n", err);
      unsigned int valore1,valore2;
      char val_1[50],val_2[50];
      scanf("%s %s",val_1,val_2);
      printf("%s adjdzkhzd %s",val_1,val_2);
      int a = write(id_socket,(void *)&val_1,strlen(val_1));
      int b = write(id_socket,(void *)&" ",1);
      int c = write(id_socket,(void *)&val_2,strlen(val_2));
      int d = write(id_socket,(void *)&"\r\n",2);
      printf("prova: %d %d %d\n",a,b,c);
      char* res = malloc(50*sizeof(char));
      char i = ' ';
      int cr = 0;
      while(i!='\n')
      {
	if(read(id_socket,&i,1) != 1) return -1;
	res[cr++] = i;
      }
      if(res[0] > '9' || res[0] < '0')
	printf("OVERFLOW\n");
      else
	printf("%s\n",res);

      err = close(id_socket);
      printf("risultato chiusura %d\n",err);
    }
  return 0;
}
