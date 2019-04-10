#include "sockwrap.h"
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

#define RECV_BUFF_SIZE 32
#define SEND_BUFF_SIZE 32

#ifdef TRACE
#define trace(x) x
#else
#define trace(x)
#endif

char* prog_name;

int main(int argc, char **argv)
{
    char *rcvbuffer = malloc(RECV_BUFF_SIZE*sizeof(char));
    char sndbuffer[SEND_BUFF_SIZE];

    int s;

    struct sockaddr_in server,client;

    if(argc != 2)
        return -1;

    s = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
    memset(&server,0,sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(atoi(argv[1]));
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    /*if(connect(s,(const struct sockaddr*)&server,sizeof(server)) == -1)
        return -1;*/
    Bind(s,(SA*)&server,sizeof(server));
    
    trace(err_msg("(%s) listening on %s:%u",argv[0],inet_ntoa(server.sin_addr),ntohs(server.sin_port)));

    while(1){
        socklen_t client_len = sizeof(client);
        ssize_t rec = recvfrom(s,rcvbuffer,RECV_BUFF_SIZE,0,(struct sockaddr *)&client,&client_len);
        printf("numero di caratteri letti %ld  dato: %s\n",rec,rcvbuffer);
            memcpy(sndbuffer,rcvbuffer,strlen(rcvbuffer)*sizeof(char));
            ssize_t send = sendto(s,sndbuffer,strlen(sndbuffer),0,(const struct sockaddr *)&client,sizeof(client));
            printf("numero di caratteri inviati %ld dato: %s\n",send,sndbuffer);
    }
    close(s);
    return 0;
}