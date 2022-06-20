#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>

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
        printf("%c",*(message + i));
        i++;
        message = realloc(message, (i + 1) * sizeof(char));
    }
    *(message + i) = '\0';
    return message;
}

int createServer(struct sockaddr_in *sa)
{
    int sd = socket(PF_INET, SOCK_STREAM, 0);
    if (sd == -1)
    {
        perror("erreur de création de socket serveur");
        exit(0);
    }

    int berr = bind(sd, (struct sockaddr *)sa, sizeof(*sa));
    if (berr == -1)
    {
        perror("erreur de configuration de la socket serveur");
        exit(0);
    }

    int enlist = listen(sd, SOMAXCONN);
    if (enlist == -1)
    {
        perror("erreur sur le listen");
        exit(0);
    }

    printf("%d \n", sd);
    return sd;
}

int createConnection(int SockerServeur, struct sockaddr_in *sa)
{
    int size = sizeof(*sa);
    int socClient = accept(SockerServeur, (struct sockaddr *)sa, &size);
    if (socClient == -1)
    {
        perror("erreur de connexion du client");
        return -1;
    }
    return socClient;
    /* while (1)
{
int socClient = accept(sd, (struct sockaddr*)sa, sizeof(sa));
}*/
}

// int multiThreads(int SockerServeur, struct sockaddr_in * sa) {
// pthread_t * threads;
// threads = (pthread_t *) malloc(sizeof(pthread_t));
// pthread_create((threads), NULL, createConnection, sa);
// }

void *threadEcho(void *agrs)
{
    printf("Test");
    int *socketClient = (int *)agrs;
    char *message = readMess(*socketClient);
    printf("%s\n", message);
    //sendMess(*socketClient, message);
    close(*socketClient);
}

void mainServeurEcho(int port)
{
    struct sockaddr_in sa;
    char *ip = "127.0.0.1";
    sa.sin_family = AF_INET;
    sa.sin_port = htons(port);
    sa.sin_addr.s_addr = (long)inet_addr(ip);
    int srv = createServer(&sa);

    // pthread_t * threads;
    // int i=0;

    pthread_t *tid;

    while (1)
    {
        tid = (pthread_t *)malloc(sizeof(pthread_t));
        int socClient = createConnection(srv, &sa);
        if (socClient == -1)
        {
            perror("Connection refusé");
            exit(EXIT_FAILURE);
        }
        printf("%d\n",socClient);
        pthread_create(tid, NULL, threadEcho, &socClient);
        pthread_join(*tid, NULL);
    }
}

int main(int argc, char **argv)
{
    if (argc < 1)
    {
        printf("Usage ; [executable] [port] \n");
        return 1;
    }
    int port = atoi((*(argv + 1)));
    mainServeurEcho(port);
}