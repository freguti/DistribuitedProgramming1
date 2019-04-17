/*
 * TEMPLATE 
 */
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <ctype.h>
#include <stdio.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include <rpc/xdr.h>

#include <string.h>
#include <time.h>
#include <unistd.h>

#include "./../../include/errlib.h"
#include "./../../include/sockwrap.h"

#ifdef TRACE
#define trace(x) x
#else
#define trace(x)
#endif

char* prog_name;

int main(int argc, char **argv)
{
    int porta;
    int sock;

    struct sockaddr_in server, client;

    if(argc != 2)
    {
        printf("errore nei parametri\n");
        return -1;
    }
    //TODO: controllo sui caratteri per verifica numeri
    prog_name = argv[0];
    porta = atoi(argv[1]);
    sock = Socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    server.sin_family = AF_INET;
	server.sin_port = htons(porta);
    server.sin_addr.s_addr = htonl(INADDR_ANY);

    Bind(sock,(const struct sockaddr*)&server,sizeof(server));
    trace(err_msg("(%s) listening on %s:%u",prog_name,inet_ntoa(server.sin_addr),ntohs(server.sin_port)));
    
    
    return 0;
}
