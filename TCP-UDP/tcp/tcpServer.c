#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#define PORT 5000
#define BUFFER_SIZE 1024

int main() {
    int serverSocket, newSocket;
    struct sockaddr_in serverAddr;
    struct sockaddr_in clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);
    char buffer[BUFFER_SIZE];

    // Cria um socket TCP
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    // Preenche a estrutura sockaddr_in com os dados do servidor
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    memset(serverAddr.sin_zero, '\0', sizeof(serverAddr.sin_zero));

    // Relaciona o socket criado com os dados do servidor
    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        printf("Erro ao fazer o bind do socket.\n");
        exit(1);
    }

    // Escuta o socket aguardando solicitações de conexão
    if (listen(serverSocket, 1) < 0) {
        printf("Erro ao escutar o socket.\n");
        exit(1);
    }

    printf("Servidor TCP aguardando conexões...\n");

    // Aceita a conexão, criando um novo socket para atender o cliente
    newSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrLen);

    printf("Cliente conectado.\n");

    // Loop principal
    while (1) {
        // Opções do servidor
        printf("\nOpções do servidor:\n");
        printf("1. Enviar mensagem para o cliente\n");
        printf("2. Alternar para receber mensagem do cliente\n");
        printf("3. Encerrar conexão\n");
        printf("Escolha uma opção: ");

        int option;
        scanf("%d", &option);
        getchar(); // Limpa o buffer do teclado

        switch (option) {
            case 1: {
                // Envia mensagem para o cliente
                printf("Digite a mensagem para enviar ao cliente: ");
                fgets(buffer, BUFFER_SIZE, stdin);
                send(newSocket, buffer, strlen(buffer), 0);
                break;
            }

            case 2: {
                // Recebe mensagem do cliente
                printf("Aguardando mensagem do cliente...\n");
                memset(buffer, 0, BUFFER_SIZE);
                ssize_t recvSize = recv(newSocket, buffer, BUFFER_SIZE - 1, 0);
                if (recvSize > 0) {
                    buffer[recvSize] = '\0';
                    printf("Mensagem recebida do cliente: %s\n", buffer);
                } else if (recvSize == 0) {
                    printf("Cliente encerrou a conexão.\n");
                    close(newSocket);
                    close(serverSocket);
                    return 0;
                } else {
                    printf("Erro ao receber a mensagem do cliente.\n");
                    close(newSocket);
                    close(serverSocket);
                    return 1;
                }
                break;
            }

            case 3: {
                // Encerra a conexão e sai do programa
                printf("Encerrando conexão com o cliente...\n");
                close(newSocket);
                close(serverSocket);
                return 0;
            }

            default:
                printf("Opção inválida.\n");
                break;
        }
    }

    return 0;
}