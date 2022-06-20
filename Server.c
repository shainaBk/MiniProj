#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>

struct Message{
    char *msg;
    int size;
};
typedef struct Message message;

message *readMess(int sockId){
    message * mess = (message*)malloc(sizeof(message));
    mess->msg = (char *)malloc(sizeof(char));
    mess->size = 0;

    char c = '0';
    int i = 0;

    while (c != '\0')
    {
        mess->size++;
        recv(sockId, &c, 1, 0);
        *(mess->msg + i) = c;
        i++;
        mess->msg = realloc(mess->msg, (i + 1) * sizeof(char));
    }
    *(mess->msg + i) = '\0';
    return mess;
}

char * readMessReverse(char * Msg, int taille){
    int i;
    int cpt =0;

    char *message = (char *)malloc(sizeof(char)*taille);
    for(i=taille-2 ; i>=0 ; i--){
        *(message+cpt) = (*(Msg+i));
        cpt++;
    }
    return message;
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
    int *socketClient = (int *)agrs;
    char *messageReverse; 
    while(1){
        message *mess = (message*)malloc(sizeof(message));
        mess=readMess(*socketClient);
        char *message = (char *)malloc(sizeof(char)*mess->size);
        message = mess->msg;
        printf("%s\n",message);
        messageReverse = readMessReverse(message,mess->size);
        printf("%s\n",messageReverse);
        //sendMess(*socketClient, message);
    }
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