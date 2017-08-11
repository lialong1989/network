

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <pthread.h>

#define SER_IP			"192.168.174.131"
#define SER_PORT		0x8888

#define err_out(x)	\
	do { perror(x); exit(1); } while(0)

struct thread_arg
{
	pthread_t thread;
	int cfd;
};

void *pthread_func(void *arg)
{
	int ret;
	char buf[128] = {0};
	struct thread_arg *tmp = (struct thread_arg *)arg;


	while ((ret = read(tmp->cfd, buf, 127)) > 0)
	{
		if (0 == strncmp(buf, "quit", 4))
			break;
		printf("%s\n", buf);
		bzero(buf, 128);
	}

	bzero(buf, 128);
	printf("client exit\n");
	close(tmp->cfd);
	pthread_exit(NULL);
}

int main(void)
{

	int sfd;
	int ret;
	int size;
	struct thread_arg th;
	struct sockaddr_in s_addr, c_addr;

	sfd =  socket(AF_INET, SOCK_STREAM, 0);
	if (-1 == sfd)
		err_out("server socket");

	bzero(&s_addr, sizeof(struct sockaddr_in));

	s_addr.sin_family = AF_INET;
	s_addr.sin_port = htons(SER_PORT);
	s_addr.sin_addr.s_addr = inet_addr(SER_IP);

	ret =  bind(sfd, (struct sockaddr *)&s_addr, sizeof(struct sockaddr));
	if (-1 == ret)
		err_out("bind");

	ret = listen(sfd, 5);
	if (-1 == ret)
		err_out("listen");

	size = sizeof(struct sockaddr_in);

	while (1)
	{
		th.cfd = accept(sfd, (struct sockaddr *)(&c_addr), &size);
		if (-1 == th.cfd)
			err_out("accept");

		printf("client connnected!\n");
		printf("client ip = %s\tport = %d\n", inet_ntoa(c_addr.sin_addr), ntohs(c_addr.sin_port));
	
		ret = pthread_create(&(th.thread), NULL, pthread_func, (void *)&th);
		if (0 != ret)
			err_out("pthread");
	}

	close(sfd);

	return 0;
}
