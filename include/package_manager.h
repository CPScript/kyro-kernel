#ifndef PACKAGE_MANAGER_H
#define PACKAGE_MANAGER_H

void add_package(const char *name, const char *version);
void install_package(const char *name);
void update_package(const char *name);
void remove_package(const char *name);

#endif
