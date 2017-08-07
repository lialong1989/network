
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>

#define SER_IP          "192.168.137.128"
#define SER_PORT        0x8888
 
#define err_out(x)  \
    do { perror(x); exit(1); } while(0)

int main(void)
{
	int cfd;
	int ret;
	char buf[128];
	struct sockaddr_in s_addr;

	cfd = socket(AF_INET, SOCK_STREAM, 0);
	if (-1 == ret)
		err_out("client socket");

	bzero(&s_addr, sizeof(struct sockaddr_in));
	s_addr.sin_family = AF_INET;
	s_addr.sin_port = htons(SER_PORT);
	s_addr.sin_addr.s_addr = inet_addr(SER_IP);

	ret = connect(cfd, (struct sockaddr *)(&s_addr), sizeof(struct sockaddr));
	if (-1 == ret)
		err_out("connect");

	while (1)
	{
		bzero(buf, 128);

		fgets(buf, 128, stdin);
		write(cfd, buf, strlen(buf));
		if (0 == strncmp(buf, "quit", 4))
			break;
	}

	close(cfd);

	return 0;
}
