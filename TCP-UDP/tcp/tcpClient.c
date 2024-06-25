#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define SERVER_IP "127.0.0.1"
#define PORT 5000
#define BUFFER_SIZE 1024

int main() {
    int clientSocket;
    struct sockaddr_in serverAddr;
    char buffer[BUFFER_SIZE];
    int option;

    // Cria um socket TCP
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);

    // Preenche uma estrutura sockaddr_in com os dados do servidor
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);
    memset(serverAddr.sin_zero, '\0', sizeof(serverAddr.sin_zero));

    // Estabelece conexão com o servidor
    if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == 0) {
        printf("Conexão estabelecida com o servidor.\n");
    } else {
        printf("Falha ao conectar ao servidor.\n");
        exit(1);
    }

    // Loop principal
    while (1) {
        // Exibe o menu de opções
        printf("\n--- MENU ---\n");
        printf("1. Enviar mensagem para o servidor\n");
        printf("2. Receber mensagem do servidor\n");
        printf("3. Sair\n");
        printf("Escolha uma opção: ");
        scanf("%d", &option);

        // Limpa o buffer de entrada
        while (getchar() != '\n');

        // Executa a opção selecionada
        switch (option) {
            case 1:
                // Envia mensagem para o servidor
                memset(buffer, 0, BUFFER_SIZE);
                printf("Digite uma mensagem para enviar ao servidor (FIM para encerrar): ");
                fgets(buffer, BUFFER_SIZE, stdin);

                // Remove a quebra de linha do final da mensagem
                size_t len = strlen(buffer);
                if (len > 0 && buffer[len - 1] == '\n') {
                    buffer[len - 1] = '\0';
                }

                // Verifica se é a mensagem de encerramento
                if (strcmp(buffer, "FIM") == 0) {
                    printf("Encerrando conexão com o servidor...\n");
                    close(clientSocket);
                    return 0;
                }

                // Envia a mensagem para o servidor
                send(clientSocket, buffer, strlen(buffer), 0);
                printf("Mensagem enviada para o servidor.\n");
                break;

            case 2:
                // Recebe mensagem do servidor
                memset(buffer, 0, BUFFER_SIZE);
                ssize_t recvSize = recv(clientSocket, buffer, BUFFER_SIZE - 1, 0);
                if (recvSize > 0) {
                    buffer[recvSize] = '\0';
                    printf("Mensagem recebida do servidor: %s\n", buffer);
                } else if (recvSize == 0) {
                    printf("Servidor encerrou a conexão.\n");
                    close(clientSocket);
                    return 0;
                } else {
                    printf("Erro ao receber a mensagem do servidor.\n");
                    close(clientSocket);
                    return 1;
                }
                break;

            case 3:
                // Encerra a conexão e sai do programa
                printf("Encerrando conexão com o servidor...\n");
                close(clientSocket);
                return 0;

            default:
                printf("Opção inválida.\n");
                break;
        }
    }

    return 0;
}