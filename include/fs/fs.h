#ifndef FS_H
#define FS_H

#include <stdbool.h>
#include <stdint.h>

#define MAX_FILES 5000
#define FILENAME_LENGTH 50
#define FILE_CONTENT_LENGTH 256

typedef struct {
    char name[FILENAME_LENGTH];
    char content[FILE_CONTENT_LENGTH];
    bool is_directory;
} File;

extern File file_list[MAX_FILES];
extern int file_count;

bool create_file(const char *name, bool is_directory);
bool create_directory(const char *name);
bool delete_file(const char *name);
bool delete_directory(const char *name);
void list_files(void);
bool read_file(const char *name);
bool write_file(const char *name, const char *content);
void fs_init(void);

#endif
