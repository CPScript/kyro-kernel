#ifndef USER_H
#define USER_H

#include <stdbool.h>
#include <stdint.h> 

#define MAX_USERS 100
#define USERNAME_LENGTH 32
#define PASSWORD_LENGTH 64

typedef struct {
    char username[USERNAME_LENGTH];
    char password[PASSWORD_LENGTH];
    bool is_admin;
    uint32_t uid;
    uint32_t gid;
} user_t;

extern user_t users[MAX_USERS];
extern int user_count;

void user_init(void);
bool add_user(const char *username, const char *password, bool is_admin);
bool remove_user(const char *username);
bool authenticate_user(const char *username, const char *password);
user_t *get_user(const char *username);
void list_users(void);

#endif