#ifndef USER_H
#define USER_H

/* user/user.h - user-space API for this xv6 build */

#include "kernel/types.h"   /* provides uint, uchar, uint64, etc. */
#include "kernel/stat.h"    /* for struct stat */
#include "kernel/fcntl.h"   /* for O_RDONLY etc. */

/* sbrk modes used by this tree (add more if kernel supports others) */
#define SBRK_ERROR ((char *)-1)
#define SBRK_EAGER 0
#define SBRK_LAZY  1

/* system calls (user-visible prototypes) */
int fork(void);
int exit(int) __attribute__((noreturn));
int wait(int*);
int pipe(int*);
int write(int, const void*, int);
int read(int, void*, int);
int close(int);
int kill(int);
int exec(const char*, char**);
int open(const char*, int);
int mknod(const char*, short, short);
int unlink(const char*);
int fstat(int fd, struct stat*);
int link(const char*, const char*);
int mkdir(const char*);
int chdir(const char*);
int dup(int);
int getpid(void);

/* Low-level syscall wrapper for sbrk: user-space stub named sys_sbrk.
   This matches the usys.S stub (sys_sbrk) and the kernel handler (sys_sbrk). */
char* sys_sbrk(int, int);

/* High-level sbrk wrappers exposed to programs (call the sys_ stub). */
char* sbrk(int n);        /* eager allocation (uses SBRK_EAGER) */
char* sbrklazy(int n);    /* lazy allocation (uses SBRK_LAZY) */

int pause(int);
int uptime(void);

/* user library (ulib) functions */
int stat(const char*, struct stat*);
char* strcpy(char*, const char*);
void *memmove(void*, const void*, int);
char* strchr(const char*, char);
int strcmp(const char*, const char*);
char* gets(char*, int);

/* size/type-safe wrappers */
uint strlen(const char*);
void* memset(void*, int, uint);
int atoi(const char*);
int memcmp(const void *, const void *, uint);
void *memcpy(void *, const void *, uint);

/* user malloc/free (if implemented) */
void* malloc(uint);
void  free(void*);

/* simple helpers / demos */
int kinfo(void);
int hello(void);

/* animation control syscalls exposed to userland */
int start_anim(void);
int stop_anim(void);
int set_speed(int n);
int view_anim(void);
// Framebuffer user APIs
int fb_write(int x, int y, uint32 color);
int fb_clear(uint32 color);
/* deprecated/not needed for this change: no kernel headers beyond above */
/* simple helpers / demos */
int printf(const char*, ...);
void fprintf(int fd, const char *fmt, ...);

#endif /* USER_H */

