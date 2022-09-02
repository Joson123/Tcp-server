#include "thread_work.h"


int main(int argc, char *argv[])
{
	Pool * pool = pool_init(3);
	if(pool == NULL)
	{
		printf("pool init\n");
		return 0;
	}
	int socket_fd = socket(AF_INET,SOCK_STREAM,0);
	if(socket_fd == -1)
	{
		perror("socket failed\n");
		return 0;
	}
	struct socketaddr_in serv;
	socklen_t addrlen = sizeof(serv);
	serv.family = AF_INET;
	serv.sin_port = htons(6666);
	inet_aton(argv[1],&serv.sin_addr);
	int a = bind(socket_fd,NULL,addrlen);
	if(a == -1)
	{
		perror("bind failed\n");
		return 0;
	}
	a = list(socket_fd,5);
	if(a == -1)
	{
		perror("list failed\n");
		return 0;
	}
	while(1)
	{
		struct sockaddr_in client;
		socklen_t clientaddrlen = sizeof(client);
		int acceptsocketfd = accept(socket_fd,(struct sockaddr *)&client,&clientaddrlen);
		if(acceptsocketfd == -1)
		{
			perror("accept failed\n");
		}
		pool_add_worker(pool,tcp,(void *)&acceptsocketfd);
		printf("client addr is %s\n",inet_ntoa(client.sin_addr));
		printf("client port  is %d\n",ntos(client,sin_port));
	}
	return 0;
}
