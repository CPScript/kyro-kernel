#include "fs/fs.h"
#include "kernel.h"
#include <string.h>

File file_list[MAX_FILES];
int file_count = 0;

bool create_file(const char *name, bool is_directory) {
    if (file_count >= MAX_FILES) {
        printf("File limit reached.\n");
        return false;
    }
    if (strlen(name) == 0) {
        printf("Filename cannot be empty.\n");
        return false;
    }
    if (strlen(name) >= FILENAME_LENGTH) {
        printf("Filename too long.\n");
        return false;
    }
    strcpy(file_list[file_count].name, name);
    file_list[file_count].is_directory = is_directory;
    file_list[file_count].content[0] = '\0';
    file_count++;
    return true;
}

bool create_directory(const char *name) {
    return create_file(name, true);
}

bool delete_file(const char *name) {
    for (int i = 0; i < file_count; i++) {
        if (strcmp(file_list[i].name, name) == 0) {
            for (int j = i; j < file_count - 1; j++) {
                file_list[j] = file_list[j + 1];
            }
            file_count--;
            return true;
        }
    }
    printf("File not found.\n");
    return false;
}

bool delete_directory(const char *name) {
    for (int i = 0; i < file_count; i++) {
        if (strcmp(file_list[i].name, name) == 0) {
            if (file_list[i].is_directory) {
                for (int j = i; j < file_count - 1; j++) {
                    file_list[j] = file_list[j + 1];
                }
                file_count--;
                return true;
            } else {
                printf("Not a directory.\n");
                return false;
            }
        }
    }
    printf("Directory not found.\n");
    return false;
}

void list_files() {
    printf("Files:\n");
    for (int i = 0; i < file_count; i++) {
        printf("%s%s\n", file_list[i].name, 
               file_list[i].is_directory ? "/" : "");
    }
}

bool read_file(const char *name) {
    for (int i = 0; i < file_count; i++) {
        if (strcmp(file_list[i].name, name) == 0) {
            if (!file_list[i].is_directory) {
                printf("%s\n", file_list[i].content);
                return true;
            } else {
                printf("Cannot read directory.\n");
                return false;
            }
        }
    }
    printf("File not found.\n");
    return false;
}

bool write_file(const char *name, const char *content) {
    for (int i = 0; i < file_count; i++) {
        if (strcmp(file_list[i].name, name) == 0) {
            if (!file_list[i].is_directory) {
                strncpy(file_list[i].content, content, FILE_CONTENT_LENGTH - 1);
                file_list[i].content[FILE_CONTENT_LENGTH - 1] = '\0';
                return true;
            } else {
                printf("Cannot write to directory.\n");
                return false;
            }
        }
    }
    printf("File not found.\n");
    return false;
}

void fs_init() {
    file_count = 0;
    printf("File system initialized.\n");
}
