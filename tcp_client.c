#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
	int cfd;
	int recbyte, num = 0;
	int sin_size;
	char send_buf[100] = {0};
	char recv_buf[100] = {0};
	struct sockaddr_in s_add, c_add;
	unsigned short portnum = 0x8888;
	pid_t ret;

	printf("hello, welcome to client!\n");

	if (argc != 2)
	{
		printf("usage:echo ip\n");
		return -1;
	}

	/* 创建一个TCP连接的socket */
	cfd = socket(AF_INET, SOCK_STREAM, 0);
	if (-1 == cfd)
	{
		printf("socket fail!\n");
		return -1;
	}
	printf("socket ok!\n");

	bzero(&s_add, sizeof(struct sockaddr_in));
	s_add.sin_family = AF_INET;
	s_add.sin_addr.s_addr = inet_addr(argv[1]);
	s_add.sin_port = htons(portnum);
	printf("s_addr = %#x, port:%#x\n", s_add.sin_addr.s_addr, s_add.sin_port);

	/* 连接服务器函数 */
	if (-1 == connect(cfd, (struct sockaddr *)(&s_add), sizeof(struct sockaddr)))
	{
		printf("connect fail!\n");
		return -1;
	}
	printf("connect ok!\n");

	ret = fork();
	if (-1 == ret)
	{
		perror("client fork");
		exit(1);
	}
	else if (0 == ret)
	{
		/* child process recv */
		while (1)
		{
			/* 接受服务器发过来的数据 */
			if (-1 == (recbyte = recv(cfd, recv_buf, 100, 0)))
			{
				perror("client recv");
				exit(1);
			}
			printf("client recv ok!\n");
			recv_buf[recbyte] = '\0';
			printf("%s\n", recv_buf);
		}
	}
	else
	{
		/* parent process send */
		while (1)
		{
			memset(send_buf, 0, sizeof(send_buf));
			sprintf(send_buf, "hello,welcome to my client(%d)\n", num++);
			/* 发送函数 */
			send(cfd, send_buf, strlen(send_buf), 0);
		    sleep(1);
		}
	}


	/* 关闭套接字 */
	close(cfd);

	return 0;
}
