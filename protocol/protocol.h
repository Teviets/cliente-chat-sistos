#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <string.h>
// REGISTRO DE USUARIOS NUEVOS

typedef struct {
    char* username;
    char* ip;
}UserRegistration;


// INFORMACION DEL USUARIO BASE
typedef struct{
    char* username;
    char* status;
    char* ip;
}UserInfo;


// SOLICITAR INFORMACION DE UN USUARIO O DE TODOS
// user: (username || everyone)
typedef struct UserRequest {
    char* user;
}UserRequest;


// ENVIADO POR EL SERVIDOR
typedef struct {
    UserInfo connectedUsers;
}ConnectedUsersResponse;


// CAMBIAR DE ESTADO
// activo, inactivo, ocupado
typedef struct ChangeStatus {
    char* username;
    char* status;
}ChangeStatus;


// ENVIO DE MENSAJES (DIRECTOS || PA TODOS)
// recipient: (username || everyone)
// sender: username del remitente
typedef struct {
    char* message;
    char* recipient; 
    char* sender; 
}MessageCommunication;

// option 1: Registro de Usuarios
// option 2: Usuarios Conectados
// option 3: Cambio de Estado
// option 4: Mensajes
// option 5: Informacion de un usuario en particular
//(SON LAS MISMAS OPCIONES QUE LO QUE RESPONDERA EL SERVER PARA ESTAR EN EL MISMO FORMATO)
// PARA LA INFO DE TODOS LOS USUARIOS CONECTADOS MANDAR UN UserRequest con everyone
// PARA LA INFO DE SOLO 1 USUARIO MANDAR UN UserRequest con el username
typedef struct {
    int option;

    UserRegistration registration;

    UserRequest users;

    ChangeStatus change;

    MessageCommunication messageCommunication;
}ClientPetition;


// option 1: Registro de Usuarios
// option 2: Usuarios Conectados
// option 3: Cambio de Estado
// option 4: Mensajes
// option 5: Informacion de un usuario en particular
// codigo 200: Exito
// codigo 500: Fallo
// EN RESPUESTA A UNA SOLICITUD DEL CLIENTE POR EJEMPLO: REGISTRO DE USUARIOS
// EL SERVER RETORNA:
//OPCION, 
//CODIGO 
//MENSAJE DEL SERVIDOR
// PARA SABER SI EL REQUEST FUE SUCCESS O ALGO MURIO EN EL SERVER 
typedef struct  {
    int option;

    int code;

    char* serverMessage;

    ConnectedUsersResponse connectedUsers;

    MessageCommunication messageCommunication;

    UserInfo userInfoResponse;

    ChangeStatus change;
}ServerResponse;

#endif