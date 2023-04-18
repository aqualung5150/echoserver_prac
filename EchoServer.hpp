#ifndef ECHOSERVER_HPP
#define ECHOSERVER_HPP

#include <sys/select.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include <iostream>
#include <map>

#define BUF_SIZE 2

class EchoServer
{
private:
    std::map<int, std::string> _ping;
public:
    static void ft_bzero(void *s, size_t n)
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

        FD_ZERO(&writeSet);

        while(1)
        {
            readSet = fds;
            writeSet = fds;

            
            timeout.tv_sec = 5;
            timeout.tv_usec = 5000;

            /*
            1. readfds에 데이터가 없다면 잡지 않음(event가 없음)
            2. writefds를 등록해 두었다면 쓰기(write)가능한 소켓이 있다면 계속 잡아줌.
            3. 둘 다 event가 없다면 timeout에 설정한 시간 동안 block되어 있다가 풀림.
            */
            if ((fd_num = select(fd_max + 1, &readSet, &writeSet, 0, &timeout)) == -1)
                break;

            // std::cout << "After Select" << std::endl;

            if (fd_num == 0)
                continue;

            if (FD_ISSET(listenSock, &readSet))
            {
                adr_sz = sizeof(clientAddr);
                clientSock = accept(listenSock, (struct sockaddr*)&clientAddr, &adr_sz);

                _ping.insert(std::pair<int, std::string>(clientSock, ""));

                fcntl(clientSock, F_SETFL, O_NONBLOCK);
                FD_SET(clientSock, &fds);
                if (fd_max < clientSock)
                    fd_max = clientSock;
                std::cout << "connected client: " << clientSock << std::endl;
            }

            

            for (int i = 0; i < fd_max + 1; ++i)
            {
                // 1. readfd에 읽을 데이터가 있거나 2. 연결을 끊었을 때 FD_ISSET에 걸림
                if (FD_ISSET(i, &readSet))
                {
                    nread = read(i, buf, BUF_SIZE);
                    if (nread == 0)
                    {
                        FD_CLR(i, &fds);
                        close(i);
                        std::cout << "closed client: " << i << std::endl;
                        // continue;
                    }
                    else
                    {
                        _ping.find(i)->second += std::string(buf);
                        std::cout << "read : " << _ping.find(i)->second << std::endl;
                        bzero(&buf, BUF_SIZE);
                    }
                    continue;
                }

                /*
                구현하려는 http서버와 다른 점 -> writeSet을 계속해서 잡는다. 이유는
                read를 모두 완료하였을 때만 status플래그를 주어 writeSet에 추가한다.
                write를 모두 완료하면 fd에서 제거하며 연결을 종료.
                */
                if (FD_ISSET(i, &writeSet))
                {
                    if (_ping.find(i)->second != "")
                    {
                        write(i, _ping.find(i)->second.c_str(), _ping.find(i)->second.length());
                        _ping.find(i)->second = "";
                        FD_CLR(i, &fds);
                        close(i);
                        std::cout << "closed client: " << i << std::endl;
                    }
                }
            }
        }

        close(listenSock);
    }
};

#endif