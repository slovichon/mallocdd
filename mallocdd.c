/* $Id$ */

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#include <stdio.h>
#include <stdlib.h>

#define MALLOC_DEBUG_OP_ALLOC	0
#define MALLOC_DEBUG_OP_FREE	1
#define MALLOC_DEBUG_OP_REALLOC	2

struct malloc_debug_msg {
	size_t	 siz;
	void	*p;
	void	*p0;
	pid_t	 pid;
	int	 op;
} m;

__dead void
usage(void)
{
	fprintf(stderr, "usage: %s\n", __progname);
	exit(1);
}

struct conn {
	int c_sock;
};

int
main(int argc, char *argv[])
{
	int s;

	if (getopt(argc, argv, "") != -1)
		usage();




	if ((fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1)
		fatal("control: socket");

	bzero(&sun, sizeof(sun));
	sun.sun_family = AF_UNIX;
	if (strlcpy(sun.sun_path, SMTPD_SOCKET,
	    sizeof(sun.sun_path)) >= sizeof(sun.sun_path))
		fatal("control: socket name too long");

	if (connect(fd, (struct sockaddr *)&sun, sizeof(sun)) == 0)
		fatalx("control socket already listening");



	memset(&m, 0, sizeof(m));
	m.siz = size;
	m.p = p;
	m.p0 = p0;
	m.pid = getpid();
	m.op = op;

	read(libc_malloc_debug_sock, &m, sizeof(m));
	printf();
	snprintf(buf, sizeof()"{ echo attach %d; echo bt; echo detach; echo quit; } | "
	    "gdb -q -x /dev/stdin /usr/src/usr.sbin/smtpd.current/smtpd/obj/smtpd\n");
	system(buf);
	write(libc_malloc_debug_sock, &m, sizeof(m));

	exit(0);

}
