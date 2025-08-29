#include "sami.h"

#include <errno.h>
#include <signal.h>
#include <stdlib.h>

#include <unistd.h>
#include <sys/socket.h>
#include <sys/wait.h>


int SAMI_make(SAMI *actor, SAMI_Handler *handler, void *arg)
{
	int pair[2];

	if (socketpair(AF_UNIX, SOCK_DGRAM, 0, pair) == -1)
		return 1;
	actor->fd = pair[0];

	if (!(actor->pid = fork()))
	{
		SAMI__fd = pair[1];
		handler(arg);
		exit(0);
	}

	return actor->pid == -1;
}

int SAMI_send(SAMI *actor, void *buf, size_t length)
{
	return send(actor->fd, buf, length, 0) == -1;
}

int SAMI_recv(void *buf, size_t length)
{
	return recvfrom(SAMI__fd, buf, length, 0, 0, 0) == -1;
}

int SAMI_kill(SAMI *actor)
{
	int res;

	if (kill(actor->pid, SIGKILL))
		return 1;

	for (;;)
		if (waitpid(actor->pid, 0, 0) == -1)
			if (errno == EINTR)
				continue;
			else
				return 1;
		else
			return 0;
}
