#include "kernel.h"
#include "user.h"  
#include "fs/fs.h"

void init_users(void) {
    add_user("admin", "password1", true); 
    add_user("user1", "userpass1", false);
    printf("User system initialized with %d users.\n", user_count);
}

void init_file_system(void) {
    create_directory("home");
    create_file("readme.txt", false);
    write_file("readme.txt", "Welcome to the kyro OS!\nHours spent making this: 381");
    printf("File system initialized with %d files.\n", file_count);
}
