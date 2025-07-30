#include "login.h"
#include "kernel.h"

void login_prompt() {
    printf("Login: ");
    char username[50];
    scanf("%s", username);
    printf("Password: ");
    char password[50];
    scanf("%s", password);
    if (strcmp(username, "admin") == 0 && strcmp(password, "password") == 0) {
        printf("Login successful.\n");
    } else {
        printf("Login failed.\n");
    }
}
