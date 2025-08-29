#ifndef __SAMI_H__
#define __SAMI_H__

#include <stddef.h>
#include <unistd.h>

extern int SAMI__fd;

struct SAMI
{
	int fd;
	pid_t pid;
};
typedef struct SAMI SAMI;

typedef void SAMI_Handler(void *arg);

int SAMI_make(SAMI *actor, SAMI_Handler *handler, void *arg);
int SAMI_send(SAMI *actor, void *buf, size_t length);
int SAMI_recv(void *buf, size_t length);
int SAMI_kill(SAMI *actor);

#endif /* __SAMI_H__ */
