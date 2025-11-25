#ifndef __UNISTD_H__
#define __UNISTD_H__ 1

#include "sys/types.h"

typedef int intptr_t;

pid_t fork(void);
pid_t getpid(void);

int execv(const char *path, char *const argv[]);
int execvp(const char *file, char *const argv[]);
int execve(const char *path, char *const argv[], char *const envp[]);
int execvpe(const char *file, char *const argv[], char *const envp[]);

#endif
