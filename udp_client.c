
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define handle_error(msg) \
	           do { perror(msg); exit(1); } while (0)

int main(int argc, char *argv[])
{
	char sendline[100];
	int sockfd;
	struct sockaddr_in servaddr;

	if (argc != 2)
	{
		handle_error("usage: ./udp_client [ip]");
	}

	/* PF_INET http://blog.csdn.net/xiongmaojiayou/article/details/7584211 */
	sockfd = socket(PF_INET, SOCK_DGRAM, 0);

	bzero(&servaddr, sizeof(servaddr));

	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr(argv[1]);
	servaddr.sin_port = htons(50001);

	sprintf(sendline, "Hello World");
	sendto(sockfd, sendline, strlen(sendline), 0, (struct sockaddr *)&servaddr, sizeof(servaddr));

	close(sockfd);

	return 0;
}


#if 0

/* Internet address. */
struct in_addr {
	__be32  s_addr;
};

struct sockaddr_in {
	sa_family_t       sin_family; /* Address family       */
	__be16        sin_port;   /* Port number          */
	struct in_addr    sin_addr;   /* Internet address     */

	/* Pad to size of `struct sockaddr'. */
	unsigned char     __pad[__SOCK_SIZE__ - sizeof(short int) -
              sizeof(unsigned short int) - sizeof(struct in_addr)];
};

struct sockaddr {
	sa_family_t sa_family;  /* address family, AF_xxx   */
	char		sa_data[14];    /* 14 bytes of protocol address */
};


#endif
