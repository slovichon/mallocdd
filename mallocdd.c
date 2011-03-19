/* $Id$ */

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/un.h>

#include <err.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MALLOC_DEBUG_OP_ALLOC	0
#define MALLOC_DEBUG_OP_FREE	1
#define MALLOC_DEBUG_OP_REALLOC	2

struct malloc_debug_msg {
	size_t	 siz;
	void	*p;
	void	*p0;
	pid_t	 pid;
	int	 op;
};

const char *__progname;
char *prog;

__dead void
usage(void)
{
	fprintf(stderr, "usage: %s\n", __progname);
	exit(1);
}

struct conn {
	int c_sock;
};

#define PATH_MALLOCDD_SOCK "/var/run/malloc_debug.sock"

void
service(int fd)
{
	struct malloc_debug_msg m;
	char buf[BUFSIZ];
	ssize_t rc;

	for (;;) {
		rc = read(fd, &m, sizeof(m));
		if (rc == -1)
			err(1, "read");
		if (rc != sizeof(m))
			break;
		printf("\nALLOC(%zd)=%p\n", m.siz, m.p);
		snprintf(buf, sizeof(buf),
		    "{ echo attach %d; echo bt; echo detach; echo quit; } | "
		    "gdb -q -x /dev/stdin %s", m.pid, prog);
		system(buf);
		write(fd, &m, sizeof(m));
	}
}

int
main(int argc, char *argv[])
{
	struct sockaddr_un sun;
	mode_t old_umask;
	int s, fd;

	if (getopt(argc, argv, "") != -1)
		usage();

	s = socket(AF_UNIX, SOCK_STREAM, 0);
	if (s == -1)
		err(1, "socket");

	bzero(&sun, sizeof(sun));
	sun.sun_family = AF_UNIX;
	if (strlcpy(sun.sun_path, PATH_MALLOCDD_SOCK,
	    sizeof(sun.sun_path)) >= sizeof(sun.sun_path))
		err(1, "socket name too long");

	if (connect(s, (struct sockaddr *)&sun, sizeof(sun)) == 0)
		errx(1, "control socket already listening");

	if (unlink(PATH_MALLOCDD_SOCK) == -1)
		if (errno != ENOENT)
			err(1, "cannot unlink socket");

	old_umask = umask(0117);
	if (bind(s, (struct sockaddr *)&sun, sizeof(sun)) == -1) {
		umask(old_umask);
		err(1, "bind");
	}
	umask(old_umask);

	if (chmod(PATH_MALLOCDD_SOCK, 0666) == -1) {
		unlink(PATH_MALLOCDD_SOCK);
		err(1, "chmod");
	}

	if (listen(s, 15) == -1)
		err(1, "listen");

	for (;;) {
		fd = accept(s, NULL, NULL);
		switch (fork()) {
		case -1:
			err(1, "fork");
		case 0:
			close(fd);
			break;
		default:
			close(s);
			service(fd);
			close(fd);
			exit(0);
		}
	}
	exit(0);
}
