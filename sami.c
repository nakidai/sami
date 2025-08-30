#include "sami.h"

#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/poll.h>


#define ACTORS_MAX 1024
#define MESSAGE_LENGTH_MAX 65536

static struct pollfd pfd[ACTORS_MAX];
static SAMI actors[ACTORS_MAX];
char buffer[MESSAGE_LENGTH_MAX];
static int self_fd;

#define log(level, message) \
	fprintf("%s:%d [%s]: %s\n", __FILE__, __LINE__, level, message)

static ssize_t findspace(void)
{
	size_t i;

	for (i = 1; i < ACTORS_MAX; ++i)
		if (pfd[i].fd == -1)
			return i;

	return -1;
}

int SAMI_make(SAMI *actor, SAMI_Handler *handler)
{
	int pair[2], res;
	ssize_t i, sz;

	i = findspace();
	if (i == -1)
		return 1;

	if (socketpair(AF_UNIX, SOCK_DGRAM, 0, pair) == -1)
		return 1;
	actor->fd = pair[0];

	if (!(actor->pid = fork()))
	{
		close(pair[0]);
		actors[0].fd = pfd[0].fd = self_fd;
		self_fd = pair[1];
		actors[0].pid = -1;

		for (i = 1; i < ACTORS_MAX; ++i)
			actors[i].fd = -1,
			pfd[i].fd = -1,
			pfd[i].events = POLLIN;
retry:
		waitpid(-1, 0, WNOHANG);
		res = poll(pfd, ACTORS_MAX, 0);
		if (res == -1)
			if (errno == EINTR)
				goto retry;
			else
				log("CRITICAL", strerror(errno)), exit(1);
		else if (res == 0)
			goto retry;

		for (i = 0; i < ACTORS_MAX; ++i)
		{
			if (!(pfd[i].revents & POLLIN))
				break;

			sz = recv(pfd[i].fd, buffer, sizeof(buffer), 0);
			if (sz == -1)
			{
				log("WARNING", strerror(errno));
				continue;
			}
			if (handler(&actor[i], buffer, sz))
				exit(0);
		}
		goto retry;
	}

	close(pair[1]);
	if (actor->pid == -1)
		close(pair[0]);
	else
		memcpy(&actors[i], actor, sizeof(*actor));

	return actor->pid == -1;
}

int SAMI_send(SAMI *actor, void *buf, size_t length)
{
	return send(actor->fd, buf, length, 0) == -1;
}

int SAMI_kill(SAMI *actor)
{
	int res;

	if (actor->pid == -1
	 || kill(actor->pid, SIGKILL))
		return 1;
}

SAMI *SAMI_parent(void)
{
	return &actors[0];
}
