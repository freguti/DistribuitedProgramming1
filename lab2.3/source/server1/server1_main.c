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

#define MSG_OK "+OK\r\n"
#define MSG_ERROR "-ERR\r\n"
#define QUEUE_SIZE 5
#define RECVDIM 200 //per ora ricevo filename solo fino a 200 caratteri 
#define NAMEDIM 30 //per ora nomefile max 30
#define SENDDIM 1500 //1500 byte dimensione tcp 

char* prog_name;

int main(int argc, char **argv)
{
    fd_set cset;
    struct timeval tval; //gestione del tempo

    int porta;
    int sock;
    int connection;
    int offset = 0;
    int file = 0;
    int uscita;

     char recv_buffer[RECVDIM];
     char send_buffer[SENDDIM];
     char filename[255];

    //const char error[7] = {'-','E','R','R','\r','\n'};

    struct stat statfile;

    struct sockaddr_in server, client;
    socklen_t clientaddr = sizeof(struct sockaddr_in);

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

        memset(recv_buffer,0,RECVDIM);
        memset(send_buffer,0,SENDDIM);
        memset(filename,0,255);
        uscita = 0;
        connection = accept(sock, (struct sockaddr*) &client, &clientaddr);
        printf("%d \n(%s) - new connection from client %s:%u\n", connection,prog_name, inet_ntoa(client.sin_addr),ntohs(client.sin_port));

        char msg[5];

            char tmp = ' ';
            int i = 0;
            /*while(tmp != '\n' && i < RECVDIM)
            {
                if(read(connection,&tmp,1)!= 1) 
                    {
                        uscita = 1;   
                        break;
                    }
                recv_buffer[i++] = tmp;
            }*/
            FD_ZERO(&cset);
		    FD_SET(connection,&cset);
		    tval.tv_sec = 15; //assegno i secondi
		    tval.tv_usec = 0; //microsecondi
		    if(select(FD_SETSIZE,&cset,NULL,NULL,&tval)) //attendo una ricezione
		    {
                if((i = read(connection,recv_buffer,RECVDIM)) == -1)
                {
                    close(connection);
                    continue;
                }
            }
            else
            {
                printf("TIMEOUT\n");
                close(connection);
                continue;
            }    
            sscanf(recv_buffer, "%s %s\r\n",msg,filename);
            printf("%s\n",recv_buffer);
            if(strcmp(msg,"GET") != 0 || recv_buffer[i-1]!='\n' || recv_buffer[i-2]!= '\r')
            {
                printf("error %d\n", i);
                Send(connection,MSG_ERROR,strlen(MSG_ERROR),0);  
                close(connection);
                continue;
            }
            else
            {   
                //sleep(5);
                printf("filename : %s\n",filename);
                //cercare file con nome filename
                file = open(filename, O_RDONLY);
                if (file == -1)
                {
                    close(connection);
                    printf("errore di apertura nel file!\n");
                }
                else
                {
                    int res = stat(filename,&statfile);
                    if(res == 0)
                        printf("stat dimensione %lu  timestamp %lu\n",statfile.st_size,statfile.st_mtime);
                        
                    else   
                        printf("error stat\n");
                    if(S_ISDIR(statfile.st_mode) == 0)
                    {
                        int c = 0;
                        u_int32_t dimension = htonl(statfile.st_size);
                        int usable_dim = statfile.st_size;

                        write(connection,MSG_OK,strlen(MSG_OK));
                        write(connection,&dimension,4);
                        printf("dimensione inviata: %u\n",dimension);

                        memset(send_buffer,0,SENDDIM);
                        int ret_val = 0;
                        int sent_char=0;
                        while(sent_char < usable_dim ) // ho più di RECVDIM byte da inviare
                        {
                            memset(send_buffer,0,SENDDIM);
                            if(SENDDIM > (usable_dim - sent_char))
                                ret_val = read(file,send_buffer,usable_dim - sent_char); // potrei fare un controllo sul n di byte letti 
                            else
                                ret_val = read(file,send_buffer,SENDDIM);
                            offset = write(connection,send_buffer,ret_val);
                            if(offset != ret_val)
                            {
                                uscita = 1;   
                                break;
                            }
                            sent_char += offset;

                        }

                        if(uscita == 1)
                        {
                            close(connection);
                            continue;
                        }
                        printf("byte inviati: %d\n",sent_char);
                        printf("timestamp %lu\n",statfile.st_mtime);
                        uint32_t timestamp = htonl(statfile.st_mtime);
                        offset = 0;
                        offset = write(connection,&timestamp,4);
                        printf("byte inviati timestamp %d\n",offset);
                        close(connection);
                        
                    }
                    else
                    {
                        close(connection);
                        printf("bisogna scegliere un file\n");
                    }
                    
                }
            }
    }

    return 0; //28/04/2019
}
