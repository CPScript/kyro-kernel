#include "../kernel/kernel.h"
#include "../kernel/user.h"  
#include "../include/fs/fs.h"
#include "../kernel/printf.h"

void init_users();
void init_file_system();

void init_users(void) {
    add_user("admin", "password1", true); 
    add_user("user1", "userpass1", false); /
    printf("User system initialized with %d users.\n", user_count);
}

void init_file_system(void) {
    create_directory("home");
    create_file("readme.txt", false);
    write_file("readme.txt", "Welcome to the kyro OS!\nHours spent making this: 377");
    printf("File system initialized with %d files.\n", file_count);
}
