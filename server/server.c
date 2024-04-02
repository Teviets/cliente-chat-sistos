#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define MAX_USERS 100
#define BUFFER_SIZE 1024
#define DEFAULT_PORT 8888
#define MAX_TOKENS 100

// Estructura para representar a un usuario
typedef struct {
    int socket;
    char username[50];
    char ip[INET_ADDRSTRLEN];
    char status[20];
} User;

// Estructura para almacenar la información del servidor
typedef struct {
    User users[MAX_USERS];
    int user_count;
    pthread_mutex_t mutex;
} ServerInfo;

ServerInfo server;

// Prototipos de funciones
void handle_request(int user_socket, int option);
void UserRegistration(int user_socket, char *username, char *ip);
void send_connected_users(int user_socket);
void ChangeStatus(char *username, char *status);
void send_message(int user_socket, char *recipient, char *message_text);
void send_user_info(int user_socket, char *username);
void send_response(int user_socket, int option, int code, char *message);
void show_connected_users_and_messages();
void remove_user(int user_socket);

// Función para manejar las conexiones de los usuarios
void *handle_user(void *arg) {
    int user_socket = *((int *)arg);
    char buffer[BUFFER_SIZE];
    int option;
    int code = 0;
    char message[BUFFER_SIZE];

    // Leer la opción del usuario
    if (recv(user_socket, &option, sizeof(int), 0) <= 0) {
        perror("Error al recibir opción del usuario");
        close(user_socket);
        pthread_exit(NULL);
    }

    // Manejar la solicitud del usuario
    handle_request(user_socket, option);

    // Cerrar la conexión y eliminar al usuario del servidor
    remove_user(user_socket);

    close(user_socket);
    pthread_exit(NULL);
}

// Función para manejar las solicitudes de los usuarios
void handle_request(int user_socket, int option) {
    int code;
    char message[BUFFER_SIZE];

    switch (option) {
        case 1: // Registro de Usuarios
            {
                // Recibir el nombre de usuario del cliente
                char buffer[BUFFER_SIZE];
                if (recv(user_socket, buffer, BUFFER_SIZE, 0) <= 0) {
                    perror("Error al recibir el nombre de usuario y la dirección IP");
                    close(user_socket);
                    pthread_exit(NULL);
                }

                char *username = strtok(buffer, "|");
                char *ip = strtok(NULL, "|"); // Pasar NULL para continuar dividiendo la misma cadena
                printf("Registrando usuario %s\n", username);
                
                printf("IP: %s\n", ip);
                //printf("hola");
                //UserRegistration(user_socket, username, ip);
            }
            break;
        case 2: // Usuarios Conectados
            {
                send_connected_users(user_socket);
            }
            break;
        case 3: // Cambio de Estado
            {
                char username[50], status[20];
                // Recibir los datos de cambio de estado del usuario
                if (recv(user_socket, &username, sizeof(username), 0) <= 0 || recv(user_socket, &status, sizeof(status), 0) <= 0) {
                    perror("Error al recibir datos de cambio de estado del usuario");
                    close(user_socket);
                    pthread_exit(NULL);
                }
                ChangeStatus(username, status);
            }
            break;
        case 4: // Mensajes
            {
                char recipient[50], message_text[BUFFER_SIZE];
                // Recibir los datos del mensaje del usuario
                if (recv(user_socket, &recipient, sizeof(recipient), 0) <= 0 || recv(user_socket, &message_text, sizeof(message_text), 0) <= 0) {
                    perror("Error al recibir datos del mensaje del usuario");
                    close(user_socket);
                    pthread_exit(NULL);
                }
                send_message(user_socket, recipient, message_text);
            }
            break;
        case 5: // Información de un usuario en particular
            {
                char username[50];
                // Recibir el nombre de usuario del usuario
                if (recv(user_socket, &username, sizeof(username), 0) <= 0) {
                    perror("Error al recibir nombre de usuario del usuario");
                    close(user_socket);
                    pthread_exit(NULL);
                }
                send_user_info(user_socket, username);
            }
            break;
        default:
            // Opción no válida
            code = 500;
            sprintf(message, "Error: Opción inválida");
            send_response(user_socket, option, code, message);
            break;
    }
}

// Función para registrar a un nuevo usuario
// Función para registrar a un nuevo usuario
void UserRegistration(int user_socket, char *username, char *ip) {
    int code;
    char message[BUFFER_SIZE];

    // Bloquear el acceso a la estructura del servidor para evitar condiciones de carrera
    pthread_mutex_lock(&server.mutex);

    // Verificar si el usuario ya está registrado
    int i;
    for (i = 0; i < server.user_count; i++) {
        if (strcmp(server.users[i].username, username) == 0) {
            code = 500; // Usuario ya registrado
            sprintf(message, "Error: Usuario '%s' ya existe", username);
            send_response(user_socket, 1, code, message);
            pthread_mutex_unlock(&server.mutex);
            
        }
    }

    // Registrar al nuevo usuario
    strcpy(server.users[server.user_count].username, username);
    strcpy(server.users[server.user_count].ip, ip);
    strcpy(server.users[server.user_count].status, "ACTIVO");
    server.users[server.user_count].socket = user_socket;
    server.user_count++;

    // Enviar respuesta exitosa al usuario
    code = 200;
    sprintf(message, "Usuario '%s' registrado correctamente", username);
    send_response(user_socket, 1, code, message);

    

    // Enviar mensaje de bienvenida al cliente
    char welcome_message[BUFFER_SIZE];
    sprintf(welcome_message, "Gracias por conectarte al servidor de Manuel, %s", username);
    send(user_socket, welcome_message, strlen(welcome_message), 0);
    
    // Mostrar los usuarios conectados y si se está enviando algún mensaje
    show_connected_users_and_messages();

    pthread_mutex_unlock(&server.mutex);
}


// Función para enviar la lista de usuarios conectados al usuario
void send_connected_users(int user_socket) {
    int count = server.user_count;
    send(user_socket, &count, sizeof(int), 0);
    for (int i = 0; i < server.user_count; i++) {
        send(user_socket, &server.users[i], sizeof(User), 0);
    }
}

// Función para cambiar el estado de un usuario
void ChangeStatus(char *username, char *status) {
    pthread_mutex_lock(&server.mutex);

    // Buscar al usuario en la lista de usuarios
    for (int i = 0; i < server.user_count; i++) {
        if (strcmp(server.users[i].username, username) == 0) {
            // Actualizar el estado del usuario
            strcpy(server.users[i].status, status);
            break;
        }
    }

    // Mostrar los usuarios conectados y si se está enviando algún mensaje
    show_connected_users_and_messages();

    pthread_mutex_unlock(&server.mutex);
}

// Función para enviar un mensaje a un usuario
void send_message(int user_socket, char *recipient, char *message_text) {
    pthread_mutex_lock(&server.mutex);

    // Buscar al destinatario en la lista de usuarios
    for (int i = 0; i < server.user_count; i++) {
        if (strcmp(server.users[i].username, recipient) == 0) {
            // Enviar el mensaje al destinatario
            send(server.users[i].socket, message_text, strlen(message_text), 0);
            break;
        }
    }

    // Mostrar los usuarios conectados y si se está enviando algún mensaje
    show_connected_users_and_messages();

    pthread_mutex_unlock(&server.mutex);
}

// Función para enviar la información de un usuario al usuario
void send_user_info(int user_socket, char *username) {
    pthread_mutex_lock(&server.mutex);

    // Buscar al usuario en la lista de usuarios
    for (int i = 0; i < server.user_count; i++) {
        if (strcmp(server.users[i].username, username) == 0) {
            // Enviar la información del usuario al usuario
            send(user_socket, &server.users[i], sizeof(User), 0);
            break;
        }
    }

    pthread_mutex_unlock(&server.mutex);
}

// Función para enviar una respuesta al usuario
void send_response(int user_socket, int option, int code, char *message) {
    send(user_socket, &option, sizeof(int), 0);
    send(user_socket, &code, sizeof(int), 0);
    send(user_socket, message, strlen(message), 0);
}

// Función para mostrar los usuarios conectados y si se está enviando algún mensaje
void show_connected_users_and_messages() {
    printf("\nUsuarios Conectados:\n");
    for (int i = 0; i < server.user_count; i++) {
        printf("- %s (%s) - Estado: %s\n", server.users[i].username, server.users[i].ip, server.users[i].status);
    }
    printf("\nEnviando mensajes: %s\n", server.user_count > 0 ? "Sí" : "No");
}

// Función para remover un usuario del listado de usuarios conectados
void remove_user(int user_socket) {
    pthread_mutex_lock(&server.mutex);

    for (int i = 0; i < server.user_count; i++) {
        if (server.users[i].socket == user_socket) {
            // Eliminar al usuario del listado
            for (int j = i; j < server.user_count - 1; j++) {
                server.users[j] = server.users[j + 1];
            }
            server.user_count--;
            break;
        }
    }

    // Mostrar los usuarios conectados y si se está enviando algún mensaje
    show_connected_users_and_messages();

    pthread_mutex_unlock(&server.mutex);
}

int main(int argc, char *argv[]) {
    int server_socket, user_socket, port;
    struct sockaddr_in server_addr, user_addr;
    socklen_t user_len = sizeof(user_addr);
    pthread_t tid;

    // Verificar si se proporcionó un puerto como argumento, de lo contrario, usar el puerto predeterminado
    if (argc < 2) {
        port = DEFAULT_PORT;
    } else {
        port = atoi(argv[1]);
    }

    // Crear socket del servidor
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Fallo al crear socket");
        exit(EXIT_FAILURE);
    }

    // Configurar la dirección del servidor
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    // Enlazar el socket del servidor
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Fallo al enlazar");
        exit(EXIT_FAILURE);
    }

    // Escuchar por conexiones entrantes
    if (listen(server_socket, 5) < 0) {
        perror("Fallo al escuchar");
        exit(EXIT_FAILURE);
    }

    printf("Servidor escuchando en el puerto %d...\n", port);

    // Inicializar la estructura del servidor
    server.user_count = 0;
    pthread_mutex_init(&server.mutex, NULL);

    while (1) {
        // Aceptar la conexión entrante
        if ((user_socket = accept(server_socket, (struct sockaddr *)&user_addr, &user_len)) < 0) {
            perror("Fallo al aceptar");
            exit(EXIT_FAILURE);
        }

        // Crear un hilo para manejar al usuario
        if (pthread_create(&tid, NULL, handle_user, &user_socket) != 0) {
            perror("Fallo al crear hilo");
            exit(EXIT_FAILURE);
        }
    }

    close(server_socket);
    pthread_mutex_destroy(&server.mutex);

    return 0;
}