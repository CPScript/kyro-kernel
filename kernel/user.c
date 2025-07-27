#include "user.h"
#include "kernel.h"
#include <string.h>
#include <stddef.h> 

#ifndef NULL
#define NULL ((void*)0)
#endif

user_t users[MAX_USERS];
int user_count = 0;
static uint32_t next_uid = 1000;

void user_init(void) {
    user_count = 0;
    printf("User system initialized.\n");
}

bool add_user(const char *username, const char *password, bool is_admin) {
    if (user_count >= MAX_USERS) {
        printf("Maximum number of users reached.\n");
        return false;
    }
    
    if (strlen(username) >= USERNAME_LENGTH) {
        printf("Username too long.\n");
        return false;
    }
    
    if (strlen(password) >= PASSWORD_LENGTH) {
        printf("Password too long.\n");
        return false;
    }
    
    // Check if user already exists
    for (int i = 0; i < user_count; i++) {
        if (strcmp(users[i].username, username) == 0) {
            printf("User already exists.\n");
            return false;
        }
    }
    
    // Add new user
    strcpy(users[user_count].username, username);
    strcpy(users[user_count].password, password);
    users[user_count].is_admin = is_admin;
    users[user_count].uid = next_uid++;
    users[user_count].gid = is_admin ? 0 : 1000; // Admin group 0, user group 1000
    
    user_count++;
    return true;
}

bool remove_user(const char *username) {
    for (int i = 0; i < user_count; i++) {
        if (strcmp(users[i].username, username) == 0) {
            // Shift remaining users
            for (int j = i; j < user_count - 1; j++) {
                users[j] = users[j + 1];
            }
            user_count--;
            return true;
        }
    }
    return false;
}

bool authenticate_user(const char *username, const char *password) {
    for (int i = 0; i < user_count; i++) {
        if (strcmp(users[i].username, username) == 0) {
            return strcmp(users[i].password, password) == 0;
        }
    }
    return false;
}

user_t *get_user(const char *username) {
    for (int i = 0; i < user_count; i++) {
        if (strcmp(users[i].username, username) == 0) {
            return &users[i];
        }
    }
    return NULL;
}

void list_users(void) {
    printf("Users:\n");
    for (int i = 0; i < user_count; i++) {
        printf("  %s (UID: %d, %s)\n", 
               users[i].username, 
               users[i].uid,
               users[i].is_admin ? "admin" : "user");
    }
}
