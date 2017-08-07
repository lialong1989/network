
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>

#define SER_IP			"192.168.137.128"
#define SER_PORT		0x8888

#define err_out(x)	\
	do { perror(x); exit(1); } while(0)

int main(void)
{

	int sfd, cfd;
	int ret;
	int size;
	pid_t pid;
	char buf[128];
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
		cfd = accept(sfd, (struct sockaddr *)(&c_addr), &size);
		if (-1 == cfd)
			err_out("accept");

		printf("client connnected!\n");
		printf("client ip = %s\tport = %d\n", inet_ntoa(c_addr.sin_addr), ntohs(c_addr.sin_port));
		
		pid = fork();

		if (pid < 0)
			err_out("fork");
		else if (pid == 0)
		{
			while (1)
			{
				bzero(buf, 128);

				ret = read(cfd, buf, 127);
				if (ret < 0)
					err_out("read");
				else
				{
					if ((0 == ret) || (!strncmp(buf, "quit", 4)))
					{
						bzero(buf, 128);
						printf("client exit\n");
						close(cfd);
						exit(0);
					}
					printf("%s\n", buf);
				}
			}
		}
	}

	close(sfd);

	return 0;
}
