/*
 * TEMPLATE 
 */
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

#define QUEUE_SIZE 15
#define RECVDIM 200 //per ora ricevo filename solo fino a 200 caratteri 
#define DIM_ERR 6
#define NAMEDIM 30 //per ora nomefile max 30
#define SENDDIM 1500 //1500 byte dimensione tcp 

char* prog_name;

int main(int argc, char **argv)
{
    int porta;
    int sock;
    int connection;
    int offset = 0;
    int file = 0;
    FILE* f;

    char *recv_buffer = malloc(RECVDIM);
    char *send_buffer = malloc(1500);
    char *filename;
    const char error[6] = {'-','E','R','R','\r','\n'};
    char *success;

    struct stat statfile;

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
        int read_b = recv(connection,recv_buffer,(size_t)RECVDIM,0);
        printf("stringa ricevuta: %s\n",recv_buffer);
        if(read_b == -1)
        {
            printf("errore di ricezione!\n");
        }
        else
        {
            if(!(recv_buffer[0] == 'G' && recv_buffer[1] == 'E' && recv_buffer[2] == 'T' && recv_buffer[3] == ' ' && recv_buffer[read_b-1] != '\r' && recv_buffer[read_b-2] != '\n'))
            {
                printf("error\n");
                Send(connection,(void *)error,DIM_ERR,0);
            }
            else
            {   
                char msg[5];
                sscanf(recv_buffer,"%s %s\n\r",msg, filename);
                printf("filename : %s\n",filename);
                //cercare file con nome filename
                file = open(filename, O_RDONLY);
                if (file == -1)
                {
                    printf("errore di apertura nel file!\n");
                }
                else
                {
                    fflush(stdout);
                    int res = stat(filename,&statfile);
                    if(res == 0)
                        printf("stat dimensione %lu  timestamp %lu\n",statfile.st_size,statfile.st_mtime);
                    else   
                        printf("error stat\n");
                    
                    if(S_ISDIR(statfile.st_mode) == 0)
                    {
                        //invio file
                        fflush(stdout);
                        strcpy(send_buffer,"+OK\r\n");
                        int dimension = htonl(statfile.st_size);
                        printf("dimensione convertita %u\n",dimension);
                        fflush(stdout);
                        sprintf(&send_buffer[5],"%u",dimension);
                        offset += read(file,&send_buffer[9],1500-9);
                        Send(connection,send_buffer,1500,0);
                        memset(send_buffer,0,SENDDIM);
                        while((statfile.st_size - offset) > (SENDDIM-4) ) // ho più di RECVDIM byte da inviare
                        {
                            offset += read(file,send_buffer,1500); // potrei fare un controllo sul n di byte letti 
                            write(connection,(void *)send_buffer,1500);
                            //printf("%d\n",offset);
                            memset(send_buffer,0,SENDDIM);
                        }
                        //fine, invio i rimanenti byte
                        offset += read(file,send_buffer,(statfile.st_size - offset) );
                        printf("%d\n",offset);
                        Send(connection,send_buffer,(statfile.st_size - offset) ,0);
                        uint32_t timestamp = htonl(statfile.st_mtime);
                        sprintf(send_buffer,"%u",timestamp);
                        Send(connection,send_buffer,4,0);
                        printf("byte inviati: %d\n",offset);
                    }
                    else
                    {
                        printf("bisogna scegliere un file\n");
                    }
                    
                }
            }
        }
    }
    return 0;
}
