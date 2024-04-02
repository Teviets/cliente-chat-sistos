#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "../protocol/protocol.h"

#define ACTIVE "activo"
#define BUSY "ocupado"
#define INACTIVE "inactivo"

int beggin() {
    system("clear");
    printf("Bienvenido al chat\n");
    printf("1. Iniciar sesión\n");
    printf("2. Registrarse\n");
    printf("3. Salir\n");
    int opcion = 0;
    scanf("%d", &opcion);
    return opcion;

}

char* login(){
    system("clear");
    printf("Iniciar sesión\n");
    printf("Ingrese su nombre de usuario: ");
    char *username = (char *)malloc(20);
    scanf(" %[^\n]", username);
    return username;
}

char* registerUser(){
    system("clear");
    printf("Registrarse\n");
    printf("Ingrese su nombre de usuario: ");
    char *username = (char *)malloc(20);
    scanf(" %[^\n]", username);
    return username;
}

int menu(){
    system("clear");

    int opcion;
    printf("1. Chat general\n");
    printf("2. Chat privado\n");
    printf("3. Cambiar status\n");
    printf("4. Usuarios conectados\n");
    printf("5. Información de los usuarios\n");
    printf("6. Ayuda\n");
    printf("7. Salir\n");
    printf("Ingrese una opcion: ");
    scanf("%d", &opcion);
    return opcion;
}


void chat(MessageCommunication *chat[], int numChats) {
    system("clear");
    printf("------------------------\n");
    printf("Para salir escribe 'exit'\n");
    printf("------------------------\n");
    for (int i = 0; i < numChats; i++) {
        if (strcmp(chat[i]->recipient, "everyone") == 0) {
            printf("\033[0;32m%s: ", chat[i]->sender);
            printf("\033[0m%s\n", chat[i]->message);
        } else {
            printf("\033[0;33m%s: ", chat[i]->sender);
            printf("\033[0m%s\n", chat[i]->message);
        }
    }
}

char* pushChat(){
    printf("Di algo: ");
    char *chat = (char *)malloc(1024);
    scanf(" %[^\n]", chat);
    getchar();
    return chat;
}



int changeStatus(){
    system("clear");
    printf("Cambiar status\n");
    printf("1. Activo\n");
    printf("2. Ocupado\n");
    printf("3. Inactivo\n");
    printf("Seleccione una opcion: ");
    int opcion;
    scanf("%d", &opcion);

    if (opcion == 1){
        printf("Status cambiado a Activo\n");
        sleep(2);
    }else if (opcion == 2){
        printf("Status cambiado a Ocupado\n");
        sleep(2);
    }else if (opcion == 3){
        printf("Status cambiado a Inactivo\n");
        sleep(2);
    }
    
    return opcion;
}


void getOnlineUsers(UserInfo* users[], int numUsers) {
    system("clear");
    printf("Usuarios conectados\n");

    for (int i = 0; i < numUsers; i++) {
        if (users[i] != NULL) {
            char *status = users[i]->status;
            if (strcmp(status, ACTIVE) == 0) {
                printf("\033[0;32m%s: ", users[i]->username);
                printf("\033[0m%s\n", status);
            } else if (strcmp(status, BUSY) == 0) {
                printf("\033[0;33m%s: ", users[i]->username);
                printf("\033[0m%s\n", status);
            } else if (strcmp(status, INACTIVE) == 0) {
                printf("\033[0;31m%s: ", users[i]->username);
                printf("\033[0m%s\n", status);
            }
        }
    }
    printf("\033[0m");
    char exit[6]; // Aumenta el tamaño del arreglo a 6
    while (strcmp(exit, "exit") != 0) {
        printf("Escribe 'exit' para salir: ");
        scanf(" %5s", exit); // Limita la longitud de entrada a 5 caracteres
        getchar(); // Consume el salto de línea restante
    }
}

int chooseUser(UserInfo* users[], int numUsers){
    system("clear");
    printf("Escoger usuario\n\n");

    for (int i = 0; i < numUsers; i++)
    {
        printf("%d. %s\n", i+1, users[i]->username);
    }
    
    printf("Seleccione un usuario: ");
    int opcion;
    scanf("%d", &opcion);
    return opcion-1;
}

void infoUser(UserInfo* user[], int id){
    system("clear");
    printf("Información del usuario\n");
    printf("Nombre de usuario: %s\n", user[id]->username);
    printf("Estado: %s\n", user[id]->status);
    printf("IP: %s\n", user[id]->ip);

    char exit[6]; // Aumenta el tamaño del arreglo a 6
    while (strcmp(exit, "exit") != 0) {
        printf("Escribe 'exit' para salir: ");
        scanf(" %5s", exit); // Limita la longitud de entrada a 5 caracteres
        getchar(); // Consume el salto de línea restante
    }
}


