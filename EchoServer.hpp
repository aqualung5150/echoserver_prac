#ifndef ECHOSERVER_HPP
#define ECHOSERVER_HPP

#include <sys/select.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <stdlib.h>
#include <iostream>
#include <unistd.h>
#include <fcntl.h>

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
        int listenSock, clientSock;
        struct sockaddr_in listenAddr, clientAddr;
        struct timeval timeout;

        fd_set fds, readSet, writeSet;

        socklen_t adr_sz;
        int fd_max, fd_num, nread;
        char buf[BUF_SIZE];

        listenSock = socket(PF_INET, SOCK_STREAM, 0);
        ft_bzero(&listenAddr, sizeof(listenAddr));
        listenAddr.sin_family = AF_INET;
        listenAddr.sin_addr.s_addr = htonl(INADDR_ANY);
        listenAddr.sin_port = htons(port);

        bind(listenSock, (struct sockaddr*)&listenAddr, sizeof(listenAddr));
        listen(listenSock, 5);

        FD_ZERO(&fds);
        FD_SET(listenSock, &fds);
        fd_max = listenSock;

        while(1)
        {
            readSet = fds;
            writeSet = fds;

            timeout.tv_sec = 5;
            timeout.tv_usec = 5000;

            if ((fd_num = select(fd_max + 1, &readSet, &writeSet, 0, &timeout)) == -1)
                break;

            if (fd_num == 0)
                continue;

            if (FD_ISSET(listenSock, &readSet))
            {
                adr_sz = sizeof(clientAddr);
                clientSock = accept(listenSock, (struct sockaddr*)&clientAddr, &adr_sz);
                fcntl(clientSock, F_SETFL, O_NONBLOCK);
                FD_SET(clientSock, &fds);
                if (fd_max < clientSock)
                    fd_max = clientSock;
                std::cout << "connected client: " << clientSock << std::endl;
            }

            for (int i = 0; i < fd_max + 1; ++i)
            {
                if (FD_ISSET(i, &readSet))
                {
                    nread = read(i, buf, BUF_SIZE);
                    if (nread == 0)
                    {
                        FD_CLR(i, &fds);
                        close(i);
                        std::cout << "closed client: " << i << std::endl;
                        continue;
                    }
                    else
                    {
                        write(i, buf, nread);
                    }
                }                
            }
        }

        close(listenSock);
    }
};

#endif