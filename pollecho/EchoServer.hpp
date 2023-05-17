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

#define BUF_SIZE 8192
#define READ_DONE true
#define OPEN_MAX 1024

class Connection
{
private:
    // 요청 메시지
    std::string _requestMessage;
    // 응답 메시지
    std::string _responseMessage;
    const char* _writeBuffer;
    size_t _writeBufferLength, _writeBufferSent;
    int _payloadFD;
    bool _payloadDone;
    
    // 요청 메시지 속성
    bool _readDone;
    int _method;
    std::string _target;
    bool transferCodingChunk;
    int _contentLength;
    // ...
    // 응답 메시지 속성
    int _statusCode;
    // ...
public:
    Connection()
    : _readDone(false), _requestMessage(""), _responseMessage(""), _writeBuffer(NULL), _writeBufferLength(0), _writeBufferSent(0), _payloadFD(-1), _payloadDone(false)
    {
    }

    int readRequest(int socket)
    {
        char buf[BUF_SIZE];
        int nread;

        nread = read(socket, buf, BUF_SIZE);
        if (nread == 0 || nread == -1)
            return (0);
        else
            _requestMessage.append(buf, nread);

        // http 요청 메세지의 CRLF / Content-Length / Transfer-Encoding 등을 확인하여 요청이 완료(READ_DONE)되었는지 확인한다.
        if (_requestMessage.find("\nEOF\n") != std::string::npos)
            _readDone = READ_DONE;
        return (1);
    }

    bool isReadDone()
    {
        return (_readDone);
    }

    bool isPayloadDone()
    {
        return (_payloadDone);
    }

    ssize_t makeResponse()
    {
        char buf[BUF_SIZE];
        ssize_t nread;

        //요청 메시지 분석
        if (_payloadFD < 0 && _requestMessage.find("GET image") != std::string::npos)
            _payloadFD = open("./GB.bmp", O_RDONLY);
        else if (_payloadFD < 0)
            _responseMessage.append("No GET Method.");
            nread = 0;

        if (_payloadFD > 0)
        {
            nread = read(_payloadFD, buf, BUF_SIZE);

            if (nread > 0)
                _responseMessage.append(buf, nread);
            else if (nread == 0)
            {
                close(_payloadFD);
                _payloadFD = -1;
            }
        }
        
        if (nread == 0)
        {
            _payloadDone = true;
            _writeBufferLength = _responseMessage.length();
            _writeBuffer = _responseMessage.c_str();
            _writeBufferSent = 0;
        }
        

        return nread;
    }

    int writeResponse(int socket)
    {
        int len, nwrite, buf_left;

        buf_left = _writeBufferLength - _writeBufferSent;
        if (buf_left > BUF_SIZE)
            len = BUF_SIZE;
        else
            len = buf_left;
        
        if ((nwrite = write(socket, _writeBuffer + _writeBufferSent, len)) <= 0)
            return (-1);
        _writeBufferSent += nwrite;
        if (_writeBufferSent == _writeBufferLength)
            return (0);
        return (1);
    }
};

class EchoServer
{
private:
    std::map<int, Connection> _connections;
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

        /////////////
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

                Connection newConnection;
                _connections.insert(std::pair<int, Connection>(i, newConnection));

                client[i].events = POLLIN;

                if (i > maxi)
                    maxi = i;
                // if (--polled <= 0)
                //     continue;

                std::cout << "connected : " << clientSock << std::endl;
                continue;
            }

            std::cout << "maxi : " << maxi << std::endl;

            // for (i = 1; i <= maxi; ++i)
            // {
            //     if (client[i].fd < 0)
            //         continue;
            //     if (client[i].revents & (POLLIN | POLLERR))
            //     {
            //         ssize_t nread;
            //         char buf[BUF_SIZE];
            //         if ((nread = read(client[i].fd, buf, BUF_SIZE)) <= 0)
            //         {
            //             std::cout << "can not read" << std::endl;
            //             close(client[i].fd);
            //             client[i].fd = -1;
            //             //원소 삭제
            //             // if (it->first == maxi)
            //             // {
            //             //     std::map<int, Connection>::iterator it_cpy = it;
            //             //     if (it_cpy != _connections.begin())
            //             //     {
            //             //         --it_cpy;
            //             //         maxi = it_cpy->first;
            //             //     }
            //             //     else
            //             //     {
            //             //         maxi = 0;
            //             //     }
            //             // }
            //             // std::cout << "distconnected index : " << it->first << std::endl;
            //             // it = _connections.erase(it);
            //             // continue;
            //         }
            //         else
            //             write(client[i].fd, buf, nread);
            //     }
            // }

            int count = 0;
            std::map<int, Connection>::iterator it = _connections.begin();
            while (it != _connections.end())
            {
                ++count;
                // if (client[it->first].fd < 0)
                //     continue;
                if (client[it->first].revents & (POLLIN | POLLERR))
                {
                    ssize_t nread;
                    char buf[BUF_SIZE];
                    if ((nread = read(client[it->first].fd, buf, BUF_SIZE)) <= 0)
                    {
                        std::cout << "can not read" << std::endl;
                        close(client[it->first].fd);
                        client[it->first].fd = -1;
                        // 원소 삭제
                        if (it->first == maxi)
                        {
                            std::map<int, Connection>::iterator it_cpy = it;
                            if (it_cpy != _connections.begin())
                            {
                                --it_cpy;
                                maxi = it_cpy->first;
                            }
                            else
                            {
                                maxi = 0;
                            }
                        }
                        std::cout << "distconnected index : " << it->first << std::endl;
                        it = _connections.erase(it);
                        continue;
                    }
                    else
                        write(client[it->first].fd, buf, nread);
                }
                ++it;
            }
            std::cout << "loop count : " << count << std::endl;
        }
    }
};

#endif