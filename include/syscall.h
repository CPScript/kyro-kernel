#ifndef SYSCALL_H
#define SYSCALL_H

#include <stdint.h>

#define SYS_EXIT        1
#define SYS_FORK        2
#define SYS_READ        3
#define SYS_WRITE       4
#define SYS_OPEN        5
#define SYS_CLOSE       6
#define SYS_WAITPID     7
#define SYS_CREAT       8
#define SYS_LINK        9
#define SYS_UNLINK      10
#define SYS_EXECVE      11
#define SYS_CHDIR       12
#define SYS_TIME        13
#define SYS_MKNOD       14
#define SYS_CHMOD       15
#define SYS_GETPID      20
#define SYS_GETUID      24
#define SYS_PTRACE      26
#define SYS_ALARM       27
#define SYS_KILL        37
#define SYS_MKDIR       39
#define SYS_RMDIR       40
#define SYS_PIPE        42
#define SYS_BRK         45
#define SYS_GETGID      47
#define SYS_SIGNAL      48
#define SYS_GETEUID     49
#define SYS_GETEGID     50
#define SYS_IOCTL       54
#define SYS_FCNTL       55
#define SYS_SETPGID     57
#define SYS_DUP2        63
#define SYS_GETPPID     64
#define SYS_SETSID      66
#define SYS_SIGACTION   67
#define SYS_SETREUID    70
#define SYS_SETREGID    71
#define SYS_MMAP        90
#define SYS_MUNMAP      91
#define SYS_TRUNCATE    92
#define SYS_FTRUNCATE   93
#define SYS_FCHMOD      94
#define SYS_GETPRIORITY 96
#define SYS_SETPRIORITY 97
#define SYS_STAT        106
#define SYS_LSTAT       107
#define SYS_FSTAT       108

typedef uint32_t (*syscall_handler_t)(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);

void syscall_init(void);
void syscall_handler(void);
uint32_t syscall_dispatch(uint32_t call_num, uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4, uint32_t arg5);

uint32_t sys_exit(uint32_t status);
uint32_t sys_fork(void);
uint32_t sys_read(uint32_t fd, uint32_t buffer, uint32_t count);
uint32_t sys_write(uint32_t fd, uint32_t buffer, uint32_t count);
uint32_t sys_open(uint32_t pathname, uint32_t flags, uint32_t mode);
uint32_t sys_close(uint32_t fd);
uint32_t sys_getpid(void);
uint32_t sys_brk(uint32_t addr);

#endif
