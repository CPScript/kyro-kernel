#ifndef NETWORKING_H
#define NETWORKING_H

#include <stdint.h>
#include <stdbool.h>

void networking_init(void);
void start_tor(void);
int check_tor_installed(void);
void install_tor(void);
int is_first_boot(void);
void create_first_boot_flag(void);

// Network status
typedef enum {
    NET_STATUS_DISCONNECTED,
    NET_STATUS_CONNECTING,
    NET_STATUS_CONNECTED,
    NET_STATUS_ERROR
} network_status_t;

// Basic networking functions
int network_connect(const char *host, uint16_t port);
int network_disconnect(int socket);
int network_send(int socket, const void *data, size_t length);
int network_receive(int socket, void *buffer, size_t length);

// Network configuration
typedef struct {
    uint32_t ip_address;
    uint32_t subnet_mask;
    uint32_t gateway;
    uint32_t dns_server;
} network_config_t;

void network_set_config(network_config_t *config);
void network_get_config(network_config_t *config);

#endif
