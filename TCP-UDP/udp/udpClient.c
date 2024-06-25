
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>  
#include <stdlib.h>

#include "queue.h"

#define MAX_MSG 50

int main(int argc, char *argv[]){
    int sd, rc, i;
    struct sockaddr_in ladoCli;    //dados do cliente local    
    struct sockaddr_in ladoServ; 	  //dados do servidor remoto  
    int debugger = 0;
    //confere o numero de argumentos passados para o programa  
    printf("OPA %d\n",debugger++);
    printf("%s\n",argv[3]);
    //Preenchendo as informacoes de identificacao do remoto  
    ladoServ.sin_family 	    = AF_INET;
    ladoServ.sin_addr.s_addr    = inet_addr(argv[1]);
    ladoServ.sin_port 	        = htons(atoi(argv[2]));
    printf("OPA %d\n",debugger++);

    //Preenchendo as informacoes de identificacao do cliente  
    ladoCli.sin_family 	        = AF_INET;
    ladoCli.sin_addr.s_addr     = htonl(INADDR_ANY);
    ladoCli.sin_port 	        = htons(0);   //usa porta livre entre (1024-5000) 
    printf("OPA %d\n",debugger++);

    //Criando um socket. Nesse momento a variavel        
    // /sd contem apenas dados sobre familia e protocolo   

    sd = socket(AF_INET,SOCK_DGRAM,0);

    if(sd < 0) {
        printf("%s: nao pode abrir o socket \n",argv[0]);
        exit(1);
    }
    printf("OPA %d\n",debugger++);

    //Relacionando o socket sd com a estrutura ladoCli  
    //Depois do bind, sd faz referencia a protocolo local, ip local e porta local  
    rc = bind(sd, (struct sockaddr *) &ladoCli, sizeof(ladoCli));
    if( rc < 0) {
        printf("%s: nao pode fazer um bind da porta\n", argv[0]);
        exit(1);
    }
    printf("{UDP, IP_Cli: %s, Porta_Cli: %u, IP_R: %s, Porta_R: %s}\n", inet_ntoa(ladoCli.sin_addr), ntohs(ladoCli.sin_port), argv[1], argv[2]);
    printf("OPA %d\n",debugger++);

    char nome_arquivo[64];
    
    // ler o arquivo e passar ele pra fila de mensagem
    //Queue *q = createQueue();

    char message[MAX_MSG];
    int aux = 0;
    int state = 0;

    while(1) {
        if(state == 0){
            printf("Qual nome do arquivo quer enviar ? (DIGITE SWAP para receber mensagem)");
            scanf("%s",nome_arquivo);
            if(strcmp(nome_arquivo,"SWAP") == 0){
                printf("Mudando para receber message mode . . .\n");
                state = 1;
                continue;
            }
            while(1){
                if (strcmp(message,"1") != 0 ) {
                    printf("Vou enviar %s\n",message);
                    rc = sendto(sd, message, MAX_MSG, 0,(struct sockaddr *) &ladoServ, sizeof(ladoServ));
                    
                    if(rc<0) {
                        printf("%s: nao pode enviar dados\n",argv[0]);
                        close(sd);
                        exit(1); 
                    } 
                }
                else{
                    break;
                }
            }
        }
        else if(state == 1){
            printf("Pronto para receber mensagem\n");
            rc = sendto(sd, "SWAP", MAX_MSG, 0,(struct sockaddr *) &ladoServ, sizeof(ladoServ));

            char msg[MAX_MSG];
            int tam_Serv;
            int n;

            /* inicia o buffer */
            memset(msg, 0x0, MAX_MSG);
            tam_Serv = sizeof(ladoServ);
            /* recebe a mensagem  */
            n = recvfrom(sd, msg, MAX_MSG, 0, (struct sockaddr *)&ladoServ, &tam_Serv);
            
            if (n < 0){
                printf("%s: nao pode receber dados \n", msg);
                continue;
            }

            if (strncmp(msg, "SWAP",4) == 0){
                state = 0;
                printf("Mudando para enviar file mode\n");
                continue;
            }

            if (strcmp(msg, "FIM") == 0)
                exit(0);

            /* imprime a mensagem recebida na tela do usuario */
            printf("{UDP, IP_L: %s, Porta_L: %u", inet_ntoa(ladoServ.sin_addr), ntohs(ladoServ.sin_port));
            printf(" IP_R: %s, Porta_R: %u} => %s\n", inet_ntoa(ladoServ.sin_addr), ntohs(ladoServ.sin_port), msg);
        }
    }
    return 0;
}