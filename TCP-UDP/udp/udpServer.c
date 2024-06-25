#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h> /* close() */
#include <string.h> /* memset() */
#include <stdlib.h>
#include "queue.h"
#include <unistd.h>
#include <pthread.h>

int main(int argc, char *argv[]) {
    int			sd, rc, n;
    socklen_t		tam_Cli;
    struct sockaddr_in	endCli;     /* Vai conter identificacao do cliente */ 
    struct sockaddr_in	endServ;    /* Vai conter identificacao do servidor local */
    char			msg[MAX_MSG];

    if (argc < 3) {
        printf("Digite IP e a Porta para este servidor\n");
        exit(1); 
    }
    /* Criacao do socket UDP */
    sd=socket(AF_INET, SOCK_DGRAM, 0);
    
    if(sd < 0) {
        printf("%s: Nao pode abrir o socket \n",argv[0]);
        exit(1);  
    }

    /* Preenchendo informacoes sobre o servidor */
    endServ.sin_family 	    = AF_INET;
    endServ.sin_addr.s_addr = inet_addr(argv[1]); 
    endServ.sin_port 	    = htons(atoi(argv[2]));

    /* Fazendo um bind na porta local do servidor */
    rc = bind (sd, (struct sockaddr *) &endServ,sizeof(endServ));
    if(rc < 0){
        printf("%s: Não pode fazer bind na porta %s \n", argv[0], argv[2]);
        exit(1); 
    }
    printf("%s: Esperando o IP: %s, e a porta UDP numero: %s\n", argv[0], argv[1], argv[2]);

    /* Este servidor entra num loop infinito esperando dados de clientes */
    int state = 0;
    char nome_arquivo[64];
    FILE *fp;
    while(1){
        if(state == 0){
            printf("Send me files\n");
            memset(msg,0x0,MAX_MSG);
            tam_Cli = sizeof(endCli);
            n = recvfrom(sd, msg, MAX_MSG, 0, (struct sockaddr *) &endCli, &tam_Cli);
            if(n < 0) {
                printf("%s: Não pode receber dados! \n",argv[0]);
                continue;
            }
            else{
                printf("Informe um nome para o novo arquivo que deseja salvar (abaixo)!");
                scanf("%s",nome_arquivo);
                fp = fopen(nome_arquivo, "wb");
            }
            msg[MAX_MSG] = '\0';

            if(strncmp(msg,"SWAP",4) == 0){
                state = 1;
                continue;
            }
            printf("Recebi! %s\n", msg);
            memset(msg, 0x0, MAX_MSG);
        }
        else{
            printf("Envie o modo mensagem:\n>");
            fgets(msg, MAX_MSG, stdin); /* le dados do teclado */
            msg[strlen(msg)] = '\0';
            rc = sendto(sd, msg, strlen(msg), 0, (struct sockaddr *)&endCli, sizeof(endCli));
            printf("Enviei!\n");
            printf("%s",msg);

            if(strncmp(msg, "SWAP",4) == 0){
                state = 0;
                printf("swapei\n");
                continue;
            }

            if (strncmp(msg, "FIM",3) == 0){
                printf("Estou indo!\n");
                exit(0);
            }

            if (rc < 0){
                printf("Nao pode enviar dados!\n");
                close(sd);
                exit(1);
            }
            memset(msg,0x0,MAX_MSG);
        }
    }
    return 0;
} /* fim do programa */