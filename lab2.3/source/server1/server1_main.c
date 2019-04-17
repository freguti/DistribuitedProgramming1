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
#include <sys/stat.h>
#include <rpc/xdr.h>

#include <string.h>
#include <time.h>
#include <unistd.h>

#include "./../errlib.h"
#include "./../sockwrap.h"

#define QUEUE_SIZE 15
#define RECVDIM 200 //per ora ricevo filename solo fino a 200 caratteri 
#define DIM_ERR 6
#define NAMEDIM 30 //per ora nomefile max 30

char* prog_name;

int main(int argc, char **argv)
{
    int porta;
    int sock;
    int connection;
    int offset = 3;

    char *recv_buffer = malloc(RECVDIM * sizeof(char));
    char *filename;
    const char error[6] = {'-','E','R','R','\r','\n'};
    char *success;

    struct stat statfile;

    FILE* f;

    struct sockaddr_in server, client;
    socklen_t clientaddr = sizeof(client);

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
    printf("(%s) listening on %s:%u\n",prog_name,inet_ntoa(server.sin_addr),ntohs(server.sin_port));
    Listen(sock, QUEUE_SIZE);
    while(1)
    {
        connection = accept(sock, (SA*) &client, &clientaddr);
        printf("(%s) - new connection from client %s:%u\n", prog_name, inet_ntoa(client.sin_addr),ntohs(client.sin_port));
        int read = recv(connection,recv_buffer,(size_t)(RECVDIM*sizeof(char)),0);
        printf("stringa ricevuta: %s\n",recv_buffer);
        if(read == -1)
        {
            printf("errore di ricezione!\n");
        }
        else
        {
            if(!(recv_buffer[0] == 'G' && recv_buffer[1] == 'E' && recv_buffer[2] == 'T' && recv_buffer[3] == ' ' && recv_buffer[read-1] != '\r' && recv_buffer[read-2] != '\n'))
            {
                printf("error\n");
                Send(connection,(void *)error,DIM_ERR*sizeof(char),0);
            }
            else
            {   
                sscanf(recv_buffer,"GET %s\n\r",filename);
                printf("filename : %s\n",filename);
                //cercare file con nome filename
                f = fopen(filename,"r");
                if (f == NULL)
                {
                    printf("errore di apertura nel file!\n");
                }
                else
                {
                    int res = stat(filename,&statfile);
                    if(res == 0)
                        printf("stat dimensione %lu  timestamp %lu\n",statfile.st_size,statfile.st_mtime);
                    else   
                        printf("error stat\n");
                    
                    //invio stringa di conferma
                }
            }
        }
    }
    return 0;
}
