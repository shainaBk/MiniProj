#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>

int connecToServer(char *ip, int port)
{
    printf("port : %d, ip : %s \n", port, ip);
    struct sockaddr_in sa;
    sa.sin_addr.s_addr = (long)inet_addr(ip);
    sa.sin_family = AF_INET;
    sa.sin_port = htons(port);

    //Creation Socket
    int sd = socket(PF_INET, SOCK_STREAM, 0);
    if (sd == -1)
    {
        printf("Erreur de création de Socket \n");
        return -1;
    }

    //Connexion
    int connexion = connect(sd, (struct sockaddr *)&sa, sizeof(sa));
    if (connexion == -1)
    {
        perror("Erreur de connexion \n");
        return -1;
    }
    printf("%d \n", connexion);
    return sd;
}

int sendMess(int sockId, char *query)
{
    int envoie = send(sockId, query, strlen(query) + 1, 0);
    if (envoie == -1)
    {
        perror("Error d'envoie de message\n");
        return -1;
    }
    return envoie;
}

char *readMess(int sockId)
{
    char *message = (char *)malloc(sizeof(char));
    char c = '0';
    int i = 0;
    while (c != '\0')
    {
        recv(sockId, &c, 1, 0);
        *(message + i) = c;
        i++;
        message = realloc(message, (i + 1) * sizeof(char));
    }
    *(message + i) = '\0';
    return message;
}

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        printf("\n");
        exit(0);
    }
    int port = atoi(*(argv + 1));
    char *ip = *(argv + 2);
    int sd = connecToServer(ip, port);
    sendMess(sd, "Hello");
    char *messageEcho = readMess(sd);
    printf("%s\n", messageEcho);
}