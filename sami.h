#ifndef __SAMI_H__
#define __SAMI_H__

#include <stddef.h>
#include <unistd.h>


struct SAMI
{
	int fd;
	pid_t pid;
};
typedef struct SAMI SAMI;

typedef int SAMI_Handler(const SAMI *sender, void *message, size_t length);

int SAMI_make(SAMI *actor, SAMI_Handler *handler);
int SAMI_send(SAMI *actor, void *buf, size_t length);
int SAMI_kill(SAMI *actor);
SAMI *SAMI_parent(void);

#endif /* __SAMI_H__ */
