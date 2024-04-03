// Cliente
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <time.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>
#include <sys/stat.h>


#include "vista/vista.h"
#include "protocol/protocol.h"

#define MAX_CHATS 50
#define ACTIVE "activo"
#define BUSY "ocupado"
#define INACTIVE "inactivo"

UserInfo *users[50];

MessageCommunication *chats[50];
UserInfo *user[75];

pthread_mutex_t mutex;

ClientPetition petition;
ServerResponse response;

int option = 0;
int numChats = 0;
int numUsers = 0;
char *msg;

int sockfd;

char username[20];

int sockfd;


void createSocket(char *user, char *ip, char *port) {
    // Crear socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Error al crear el socket");
        exit(1);
    }

    // Conectar al servidor
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(atoi(port));

    if(inet_pton(AF_INET, ip, &server.sin_addr) <= 0) {
        perror("Error al convertir la dirección IP");
        exit(1);
    }
    
    if (connect(sockfd, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("Error al conectar al servidor");
        exit(1);
    }

    ClientPetition petition;
    petition.option = 1;

    // Asignar memoria para username e ip
    size_t username_len = strlen(user) + 1; // +1 para el carácter nulo
    size_t ip_len = strlen(ip) + 1; // +1 para el carácter nulo

    petition.registration.username = malloc(username_len);
    if (petition.registration.username == NULL) {
        perror("Error al asignar memoria para el nombre de usuario");
        exit(1);
    }

    petition.registration.ip = malloc(ip_len);
    if (petition.registration.ip == NULL) {
        perror("Error al asignar memoria para la dirección IP");
        exit(1);
    }

    // Copiar las cadenas de forma segura
    strncpy(petition.registration.username, user, username_len);
    strncpy(petition.registration.ip, ip, ip_len);

    // Asegurarse de que las cadenas estén terminadas con un carácter nulo
    petition.registration.username[username_len - 1] = '\0';
    petition.registration.ip[ip_len - 1] = '\0';

    // Enviar la petición de registro al servidor
    if (send(sockfd, &petition, sizeof(ClientPetition), 0) < 0) {
        perror("Error al enviar la petición de registro");
        exit(1);
    }

    // Cerrar el socket (opcional)
    // close(sockfd);
}

void fetchChats(){
    ClientPetition request;
    request.option = 4;
    
    if (send(sockfd, &request, sizeof(ClientPetition), 0) < 0) {
        perror("Error al enviar la solicitud de mensajes");
        exit(1);
    }

    // Recibir mensajes
    int numChats;

}

void fetchChangeStatus(int status){

}


void *privateChat(void *arg){
    int userChoice = *(int *)arg;

    UserInfo *selected = users[userChoice];

    printf("Chat privado con %s\n", selected->username);

    getChats(2);

    pthread_mutex_lock(&mutex);
    while (strcmp(msg, "exit") != 0) {
        chat(chats, numChats);
        msg = pushChat();
        updateChats(msg,2);
    }

    pthread_mutex_unlock(&mutex);
    
    pthread_exit(NULL);
}

void getChats(int type){
    // fake chats
    for (int i = 0; i < 10; i++){
        
        MessageCommunication *chat = malloc(sizeof(MessageCommunication));  
        chat->sender = username;
        chat->message = "Hola";
        if (type == 1)
        {
            chat->recipient = "everyone";
        }else{
            chat->recipient = "user";
        }
        
        chats[numChats++] = chat;
    }

    
}

void getUsers(){
    for (int i = 0; i < 10; i++){
        UserInfo *user = malloc(sizeof(UserInfo));
        user->username = "user";
        // generar random status
        int status = rand() % 3;
        if (status == 0) {
            user->status = ACTIVE;
        } else if (status == 1) {
            user->status = BUSY;
        } else {
            user->status = INACTIVE;
        }
        user->ip = "xxx.xxx.xxx.xxx";
        users[numUsers++] = user; // Almacena el usuario en el arreglo users[]
    }
    
}

void requestUsers(int sockfd){
    ClientPetition request;

    request.option = 2;

    if (send(sockfd, &request, sizeof(ClientPetition), 0) < 0) {
        perror("Error al enviar la solicitud de usuarios");
        exit(1);
    }
}


void updateChats(char *msg, int type) {
    MessageCommunication *chat = malloc(sizeof(MessageCommunication));
    chat->sender = username;
    chat->message = msg;
    if (type == 1) {
        chat->recipient= "everyone";
    } else {
        chat->recipient= "user";
    }
    chats[numChats++] = chat;
}

void requestChat(int sockfd){
    ClientPetition request;

    request.option = 2;

    if (send(sockfd, &request, sizeof(ClientPetition), 0) < 0) {
        perror("Error al enviar la solicitud de chat");
        exit(1);
    }
}

void handle_server_response(ServerResponse response) {
    switch (response.option) {
        case 2: // Usuarios Conectados
            if (response.code == 200) {
                printf("Usuarios conectados:\n");
                printf("Username: %s, Status: %s, IP: %s\n", response.connectedUsers.connectedUsers.username, response.connectedUsers.connectedUsers.status, response.connectedUsers.connectedUsers.ip);
            } else {
                printf("Error al obtener usuarios conectados: %s\n", response.serverMessage);
            }
            break;
        case 4: // Mensajes
            if (response.code == 200) {
                printf("Mensaje recibido:\n");
                printf("De: %s\n", response.messageCommunication.sender);
                printf("Para: %s\n", response.messageCommunication.recipient);
                printf("Mensaje: %s\n", response.messageCommunication.message);
            } else {
                printf("Error al recibir mensaje: %s\n", response.serverMessage);
            }
            break;
        // Aquí puedes agregar más casos para manejar otras opciones de respuesta del servidor
        default:
            printf("Opción de respuesta no reconocida: %d\n", response.option);
            break;
    }
}



void vida(){
    msg = malloc(1024 * sizeof(char));
    int choose;
    while(option != 7){
        option = menu();

        switch (option)
        {
            case 1:
                
                getChats(1);
                //requestChat(sockfd);
                while (strcmp(msg, "exit") != 0) {
                    chat(chats, numChats);
                    msg = pushChat();
                    updateChats(msg, 1);
                }
                
                break;
            case 2:
                getUsers();
                choose = chooseUser(users, numUsers);


                pthread_t threadId;

                if(pthread_create(&threadId, NULL, privateChat, (void *)&choose) != 0){
                    perror("Error al crear el hilo");
                    exit(1);
                }

                break;
            case 3:
                int status = changeStatus();
                
                if (status == 1) {
                    ChangeStatus change= {username, ACTIVE};
                    petition.change = change;
                }else if (status == 2){
                    ChangeStatus change= {username, BUSY};
                    petition.change = change;
                }else if (status == 3){
                    ChangeStatus change= {username, INACTIVE};
                    petition.change = change;
                }
                
                //enviar status al servidor
                sleep(2);
                break;
            case 4:
                
                petition.option = 2;
                if (send(sockfd, &petition, sizeof(ClientPetition), 0) < 0) {
                    perror("Error al enviar la solicitud de usuarios conectados");
                    exit(1);
                }
                // Esperar la respuesta del servidor
                if (recv(sockfd, &response, sizeof(ServerResponse), 0) < 0) {
                    perror("Error al recibir la respuesta del servidor");
                    exit(1);
                }
                // Procesar la respuesta del servidor
                handle_server_response(response);
                break;

                
                break;
            case 5:
                getUsers();
            
                int choose = chooseUser(users, numUsers);
                infoUser(users, choose);
                break;
            case 6:
                printf("Ayuda\n");
                break;
            case 7:
                printf("Saliendo del chat\n");
                break;
            default:
                option = 0;
                break;
        }

        if(send(sockfd, &petition, sizeof(ClientPetition), 0) < 0){
            perror("Error al enviar la opcion");
            exit(1);
        }
    }
}

void begginMain(){
    int op = 0;
    while (op != 3){
        op = beggin();
        switch (op)
        {
            case 1:
                //login
                char * use = login();
                strcpy(username, use);
                // check User
                // if user exists
                vida();

                break;
            case 2:
                //register
                char * useRE = registerUser();

                int op = 1;
                if(send(sockfd,&op, sizeof(int), 0) < 0){
                    perror("Error al enviar la opcion");
                    exit(1);
                }
                strcpy(username, use);

                break;
            case 3:
                printf("Saliendo del chat\n");
                system("clear");
                break;
            default:
                break;
        }
    }
    
}


int main (int argc, char *argv[]) {
    // recibir parametros de nombre de usuario, ip del servidor y puerto del servidor
    if (argc != 4) {
        printf("Error: Debe ingresar 3 parametros\n");
        return 1;
    }
    
    char *username = argv[1];
    char *ip = argv[2];
    char *port = argv[3];

    printf("Usuario: %s\n", username);
    printf("IP: %s\n", ip);

    createSocket(username, ip, port);

    //begginMain();
    vida();

    pthread_mutex_destroy(&mutex);
    close(sockfd);
    system("clear");
    return 0;

}