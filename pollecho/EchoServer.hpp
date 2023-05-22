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
#include <fstream>
#include <sstream>

#include <ctime>

#include <poll.h>

#define BUF_SIZE 2
#define READ_DONE true
#define OPEN_MAX 1024

class User
{
private:
    // 요청 메시지
    std::string _message;
    // 응답 메시지
    std::string _serverReply;
    const char* _writeBuffer;
    size_t _writeBufferLength, _writeBufferSent;
    int _readDone;

public:
    User()
    : _readDone(false), _message(""), _serverReply("---RESPONSE---\n"), _writeBuffer(NULL), _writeBufferLength(0), _writeBufferSent(0)
    {
    }

    int readMessage(int socket)
    {
        char buf[BUF_SIZE];
        int nread;

        nread = read(socket, buf, BUF_SIZE);

        if (nread == 0 || nread == -1)
            return (0);
        else
            _message.append(buf, nread);

        // CRLF(delimter)를 찾았다면 메시지 읽기 완료
        if (_message.find("\nEOF\n") != std::string::npos)
            _readDone = READ_DONE;
        return (1);
    }

    bool isReadDone()
    {
        return (_readDone);
    }

    void makeReply()
    {
        _serverReply.append(_message + "--------------\n");

        _writeBufferLength = _serverReply.length();
        _writeBuffer = _serverReply.c_str();
        _writeBufferSent = 0;
    }

    int writeReply(int socket)
    {
        int len, nwrite, buf_left;

        buf_left = _writeBufferLength - _writeBufferSent;
        if (buf_left > BUF_SIZE)
            len = BUF_SIZE;
        else
            len = buf_left;
        
        if ((nwrite = write(socket, _writeBuffer + _writeBufferSent, len)) <= 0)
            return (-1); //error
        _writeBufferSent += nwrite;
        if (_writeBufferSent == _writeBufferLength)
        // 전송완료 했다면 클라이언트의 정보(메시지)를 초기화 해줌다
        {
            _readDone = false;
            _serverReply.clear();
            _message.clear();
            _serverReply.append("---RESPONSE---\n");
            return (0);
        }
        // 아직 전송하는 중
        return (1);
    }
};

class EchoServer
{
private:
    std::map<int, User> _users;
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

        socklen_t clientAddrSize;
        int i, maxi;
        int polled;

        struct pollfd client[OPEN_MAX];

        listenSock = socket(PF_INET, SOCK_STREAM, 0);
        ft_bzero(&listenAddr, sizeof(listenAddr));
        listenAddr.sin_family = AF_INET;
        listenAddr.sin_addr.s_addr = htonl(INADDR_ANY);
        listenAddr.sin_port = htons(port);

        bind(listenSock, (struct sockaddr*)&listenAddr, sizeof(listenAddr));
        listen(listenSock, 512);

        client[0].fd = listenSock;
        client[0].events = POLLIN;

        for (i = 1; i < OPEN_MAX; ++i)
            client[i].fd = -1;
        maxi = 0;

        while (1)
        {
            polled = poll(client, maxi + 1, 5000);

            if (client[0].revents & POLLIN)
            {
                clientAddrSize = sizeof(clientAddr);
                clientSock = accept(listenSock, (struct sockaddr *)&clientAddr, &clientAddrSize);
                for (i = 1; i < OPEN_MAX; i++)
                {
                    if (client[i].fd < 0)
                    {
                        client[i].fd = clientSock;
                        break ;
                    }
                }

                if (i == OPEN_MAX)
                {
                    std::cout << "too many clients" << std::endl;
                    exit(0);
                }

                User newConnection;
                _users.insert(std::pair<int, User>(i, newConnection));

                client[i].events = POLLIN;

                if (i > maxi)
                    maxi = i;

                std::cout << "connected : " << clientSock << std::endl;
                continue;
            }

            std::cout << "largest index : " << maxi << std::endl;

            int count = 0;
            //map< poll_index, Connection>
            std::map<int, User>::iterator it = _users.begin();
            while (it != _users.end())
            {
                ++count;
                if (client[it->first].revents & (POLLIN | POLLERR))
                {
                    if (it->second.readMessage(client[it->first].fd) <= 0)
                    {
                        std::cout << "can not read" << std::endl;
                        close(client[it->first].fd);
                        client[it->first].fd = -1;
                        // 원소 삭제
                        if (it->first == maxi)
                        {
                            std::map<int, User>::iterator it_cpy = it;
                            if (it_cpy != _users.begin())
                            {
                                --it_cpy;
                                maxi = it_cpy->first;
                            }
                            else
                                maxi = 0;
                        }
                        std::cout << "distconnected index : " << it->first << std::endl;
                        it = _users.erase(it);
                        continue;
                    }
                    if (it->second.isReadDone() == READ_DONE)
                    {
                        it->second.makeReply();
                        client[it->first].events = POLLOUT;
                    }
                }

                if (client[it->first].revents & POLLOUT)
                {
                    if (it->second.writeReply(client[it->first].fd) == 0)
                        client[it->first].events = POLLIN;
                }
                ++it;
            }
            std::cout << "number of client : " << count << std::endl;
        }
    }
};

#endif