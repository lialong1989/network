#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>

int main(void)
{
	int sfp, nfp, num = 0;
	struct sockaddr_in s_add, c_add;
	int sin_size;
	unsigned short portnum = 0x8888;
	char send_buf[100] = {0};
	char recv_buf[100] = {0};
	pid_t ret;
	int recbyte;

	printf("hello, welcome to my server!\n");

	/* 创建TCP连接的套接字 */
	sfp = socket(AF_INET, SOCK_STREAM, 0);
	if (-1 == sfp)
	{
		printf("socket fail!\n");
		return -1;
	}
	printf("socker ok!\n");

	bzero(&s_add, sizeof(struct sockaddr_in));

	s_add.sin_family = AF_INET;					//Internet
	s_add.sin_addr.s_addr = htonl(INADDR_ANY);	//将本机long数据装换为网络long数据
	s_add.sin_port = htons(portnum);			//将本机short转换为网络short

	/* 绑定s_add到套接字sfp上 */
	if (-1 == bind(sfp, (struct sockaddr *)(&s_add), sizeof(struct sockaddr)))
	{
		printf("bind fail!\n");
		return -1;
	}
	printf("bink ok!\n");

	/* 监听函数，静听客户端的连接请求, 最大允许连接5个客户端 */
	if (-1 == listen(sfp, 5))
	{
		printf("listen fail!\n");
		return -1;
	}
	printf("listen ok!\n");

	sin_size = sizeof(struct sockaddr_in);
	/* 连接请求,若无连接则阻塞 */
	nfp = accept(sfp, (struct sockaddr *)(&c_add), &sin_size);
	if (-1 == nfp)
	{
		printf("accept fail!\n");
		return -1;
	}
	printf("accept ok!\nServer start get connect from %#x:%#x\n", 
			ntohl(c_add.sin_addr.s_addr), ntohs(c_add.sin_port));

	ret = fork();
	if (-1 == ret)
	{
		perror("server fork");
		exit(1);
	}
	else if (0 == ret)
	{
		/* child process recv */
		while (1)
		{
			if (-1 == (recbyte = recv(nfp, recv_buf, 100, 0)))
			{
				perror("server recv");
				exit(1);
			}
			printf("server recv ok!\n");
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
			sprintf(send_buf, "hello,welcome to my server(%d)\n", num++);
			/* 发送函数 */
			send(nfp, send_buf, strlen(send_buf), 0);
			sleep(1);
		}
	}

	/* 关闭socker连接 */
	close(nfp);
	close(sfp);

	return 0;
}
