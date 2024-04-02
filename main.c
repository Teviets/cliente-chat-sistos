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

int option = 0;
int numChats = 0;
int numUsers = 0;
char *msg;

char username[20];

/*void createSocket(char *username, char *ip, char *port) {
    // Crear socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Error al crear el socket");
        exit(1);
    }

    // Conectar al servidor
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(atoi(port));
    server.sin_addr.s_addr = inet_addr(ip);

    if (connect(sockfd, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("Error al conectar al servidor");
        exit(1);
    }

    // Enviar nombre de usuario al servidor
    if (send(sockfd, username, strlen(username), 0) < 0) {
        perror("Error al enviar el nombre de usuario");
        exit(1);
    }

    // Recibir mensaje de bienvenida del servidor
    char buffer[1024];
    if (recv(sockfd, buffer, 1024, 0) < 0) {
        perror("Error al recibir el mensaje de bienvenida");
        exit(1);
    }

    printf("%s\n", buffer);

    // Cerrar el socket
    close(sockfd);
}*/

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
                strcpy(username, use);

                break;
            case 3:
                printf("Saliendo del chat\n");
                break;
            default:
                break;
        }
    }
    
}



void vida(){
    msg = malloc(1024 * sizeof(char));
    while(option != 7){
        option = menu();

        switch (option)
        {
            case 1:
                
                getChats(1);
                while (strcmp(msg, "exit") != 0) {
                    chat(chats, numChats);
                    msg = pushChat();
                    updateChats(msg, 1);
                }
                
                break;
            case 2:
                getChats(2);

                while (strcmp(msg, "exit") != 0) {
                    chat(chats, numChats);
                    msg = pushChat();
                    updateChats(msg,2);
                }
                

                break;
            case 3:
                int status = changeStatus();
                if (status == 1) {
                    ChangeStatus change= {username, ACTIVE};
                }else if (status == 2){
                    ChangeStatus change= {username, BUSY};
                }else if (status == 3){
                    ChangeStatus change= {username, INACTIVE};
                }
                //enviar status al servidor
                sleep(2);
                break;
            case 4:
                
                getUsers();
    
                getOnlineUsers(users, numUsers); // Corrección aquí

                
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
    }
}



int main (int argc, char *argv[]) {
    // recibir parametros de nombre de usuario, ip del servidor y puerto del servidor
    /*if (argc != 3) {
        printf("Error: Debe ingresar 3 parametros\n");
        return 1;
    }
    
    char *username = argv[1];
    char *ip = argv[2];
    char *port = argv[3];*/

    //createSocket(username, ip, port);

    begginMain();

    return 0;

}