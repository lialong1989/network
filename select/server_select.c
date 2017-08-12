
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include "list.h"

#define SER_IP			"192.168.174.131"
#define SER_PORT		0x8888

#define err_out(x)	\
	do { perror(x); exit(1); } while(0)
		
struct network
{
	int cfd;
	struct list_head list;
};

int main(void)
{

	int sfd, maxfd;
	int ret;
	int size;
	char buf[128];
	fd_set r_set;
	struct sockaddr_in s_addr, c_addr;
	struct list_head head;
	struct list_head *pos;
	struct list_head *n;
	struct network *net = NULL;
	struct network *tmp = NULL;
	
	INIT_LIST_HEAD(&head);

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
		FD_SET(0, &r_set);
		FD_SET(sfd, &r_set);
		maxfd = sfd;

		list_for_each(pos, &head)
		{
			tmp = list_entry(pos, struct network, list);
			FD_SET(tmp->cfd, &r_set);
			if (tmp->cfd > maxfd)
			{
				maxfd = tmp->cfd;
			}
		}

		printf("select wait\n");
		ret = select(maxfd+1, &r_set, NULL, NULL, NULL);
		if (ret < 0)
			err_out("select");
		else if (ret > 0)
		{
			if (FD_ISSET(0, &r_set))
			{
				bzero(buf, 128);
				fgets(buf, 128, stdin);
				if (0 == strncmp(buf, "quit", 4))
					break;
				printf("%s\n", buf);
			}

			if (FD_ISSET(sfd, &r_set))
			{
				ret = accept(sfd, (struct sockaddr *)&c_addr, &size);
				if (ret < 0)
					err_out("accept");
				net = (struct network *)malloc(sizeof(struct network));
				if (net == NULL)
					err_out("malloc");
				net->cfd = ret;
				list_add_tail(&net->list, &head);
				printf("new client connected, cfd = %d\n", net->cfd);
			}

			list_for_each_safe(pos, n, &head)
			{
				tmp = list_entry(pos, struct network, list);
				if (FD_ISSET(tmp->cfd, &r_set))
				{
					ret = read(tmp->cfd, buf, 128);
					if (ret < 0)
						err_out("read");
					else if ((ret == 0) || (0 == strncmp(buf, "quit", 4)))
					{
						FD_CLR(tmp->cfd, &r_set);
						close(tmp->cfd);
						list_del(&(tmp->list));
						free(tmp);
						maxfd = -1;
						printf("client exit\n");
					}
					else
						printf("client: %s\n", buf);
				}
			}
			
			
		}
	}

	close(sfd);

	return 0;
}
