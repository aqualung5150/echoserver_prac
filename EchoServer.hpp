#ifndef ECHOSERVER_HPP
#define ECHOSERVER_HPP

#include <sys/select.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <stdlib.h>
#include <iostream>
#include <unistd.h>

#define BUF_SIZE 10

class EchoServer
{
public:
    void ft_bzero(void *s, size_t n)
{
	size_t	i;

	i = 0;
	while (i < n)
	{
		((unsigned char *)s)[i] = 0;
		i++;
	}
}

    void serverStart(int port)
    {
        int serverSocket, clientSocket;
        struct sockaddr_in serverAddress, clientAddress;
        struct timeval timeout;

        fd_set clients, cpy_clients;

        socklen_t adr_sz;
        int fd_max, fd_num, nread;
        char buf[BUF_SIZE];

        serverSocket = socket(PF_INET, SOCK_STREAM, 0);
        ft_bzero(&serverAddress, sizeof(serverAddress));
        serverAddress.sin_family = AF_INET;
        serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
        serverAddress.sin_port = htons(port);

        bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress));
        listen(serverSocket, 5);

        FD_ZERO(&clients);
        FD_SET(serverSocket, &clients);
        fd_max = serverSocket;

        while(1)
        {
            cpy_clients = clients;
            timeout.tv_sec = 5;
            timeout.tv_usec = 5000;

            if ((fd_num = select(fd_max + 1, &cpy_clients, 0, 0, &timeout)) == -1)
                break;

            if (fd_num == 0)
                continue;

            for (int i = 0; i < fd_max + 1; ++i)
            {
                if (FD_ISSET(i, &cpy_clients))
                {
                    if (i == serverSocket)
                    {
                        adr_sz = sizeof(clientAddress);
                        clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddress, &adr_sz);
                        FD_SET(clientSocket, &clients);
                        if (fd_max < clientSocket)
                            fd_max = clientSocket;
                        std::cout << "connected client: " << clientSocket << std::endl;
                    }
                    else
                    {
                        nread = read(i, buf, BUF_SIZE);
                        if (nread == 0)
                        {
                            FD_CLR(i, &clients);
                            close(i);
                            std::cout << "closed client: " << i << std::endl;
                        }
                        else
                        {
                            write(i, buf, nread);
                        }
                    }
                }
            }
        }

        close(serverSocket);
    }
};

#endif