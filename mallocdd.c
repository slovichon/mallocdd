/* $Id$ */

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

service()
{
	ssize_t rc;

	while ((rc = read(fd, &m, sizeof(m))) == sizeof(m)) {
		printf();
		snprintf(buf, sizeof()"{ echo attach %d; echo bt; echo detach; echo quit; } | "
		    "gdb -q -x /dev/stdin /usr/src/usr.sbin/smtpd.current/smtpd/obj/smtpd\n");
		system(buf);
		write(fd, &m, sizeof(m));
	}
	if (rc == -1)
		err(1, "");
}

int
main(int argc, char *argv[])
{
	struct malloc_debug_msg m;
	struct sockaddr_un sun;
	mode_t old_umask;
	int s;

	if (getopt(argc, argv, "") != -1)
		usage();

	s = socket(AF_UNIX, SOCK_STREAM, 0);
	if (s == -1)
		fatal("socket");

	bzero(&sun, sizeof(sun));
	sun.sun_family = AF_UNIX;
	if (strlcpy(sun.sun_path, PATH_MALLOCDD_SOCK,
	    sizeof(sun.sun_path)) >= sizeof(sun.sun_path))
		fatal("socket name too long");

	if (connect(s, (struct sockaddr *)&sun, sizeof(sun)) == 0)
		fatalx("control socket already listening");

	if (unlink(PATH_MALLOCDD_SOCK) == -1)
		if (errno != ENOENT)
			fatal("cannot unlink socket");

	old_umask = umask(0117);
	if (bind(s, (struct sockaddr *)&sun, sizeof(sun)) == -1) {
		umask(old_umask);
		fatal("bind");
	}
	umask(old_umask);

	if (chmod(PATH_MALLOCDD_SOCK, 0666) == -1) {
		unlink(PATH_MALLOCDD_SOCK);
		fatal("chmod");
	}

	for (;;) {
		accept();
		switch (fork()) {
		case -1:
			err(1, "fork");
		case 0:
			break;
		default:
			break;
		close();
		}
	}

	exit(0);
}
