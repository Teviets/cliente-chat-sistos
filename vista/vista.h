#ifndef VISTA_H
#define VISTA_H

#include <stdbool.h>
#include "../protocol/protocol.h"

#define ACTIVE "activo"
#define BUSY "ocupado"
#define INACTIVE "inactivo"

typedef struct {
    char username[20];
    char chat[1024];
} Chat;

int menu();
void chat(MessageCommunication *chat[], int numChats);
char* pushChat();

int beggin();
char* login();
char* registerUser();

int changeStatus();

void getOnlineUsers(UserInfo* users[], int numUsers);

int chooseUser(UserInfo* users[], int numUsers);
void infoUser(UserInfo* user, int id);

#endif