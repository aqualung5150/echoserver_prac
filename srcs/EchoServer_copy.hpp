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

#define BUF_SIZE 8192
#define READ_DONE true

class User
{
private:
    // 요청 메시지
    std::string _message;
    // 응답 메시지
    std::string _serverReply;
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
    User()
    : _readDone(false), _message(""), _serverReply(""), _writeBuffer(NULL), _writeBufferLength(0), _writeBufferSent(0), _payloadFD(-1), _payloadDone(false)
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

        // http 요청 메세지의 CRLF / Content-Length / Transfer-Encoding 등을 확인하여 요청이 완료(READ_DONE)되었는지 확인한다.
        if (_message.find("\nEOF\n") != std::string::npos)
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
        if (_payloadFD < 0 && _message.find("GET image") != std::string::npos)
            _payloadFD = open("./GB.bmp", O_RDONLY);
        else if (_payloadFD < 0)
            _serverReply.append("No GET Method.");
            nread = 0;

        if (_payloadFD > 0)
        {
            nread = read(_payloadFD, buf, BUF_SIZE);

            if (nread > 0)
                _serverReply.append(buf, nread);
            else if (nread == 0)
            {
                close(_payloadFD);
                _payloadFD = -1;
            }
        }
        
        if (nread == 0)
        {
            _payloadDone = true;
            _writeBufferLength = _serverReply.length();
            _writeBuffer = _serverReply.c_str();
            _writeBufferSent = 0;
        }
        

        return nread;
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
            return (-1);
        _writeBufferSent += nwrite;
        if (_writeBufferSent == _writeBufferLength)
            return (0);
        return (1);
    }
};

class Server
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
        struct timeval timeout;

        fd_set readSet, readSetCopy, writeSet, writeSetCopy;

        socklen_t clientAddrSize;
        int fd_max, fd_num;

        listenSock = socket(PF_INET, SOCK_STREAM, 0);
        ft_bzero(&listenAddr, sizeof(listenAddr));
        listenAddr.sin_family = AF_INET;
        listenAddr.sin_addr.s_addr = htonl(INADDR_ANY);
        listenAddr.sin_port = htons(port);

        bind(listenSock, (struct sockaddr*)&listenAddr, sizeof(listenAddr));
        listen(listenSock, 512);

        FD_ZERO(&readSet);
        FD_ZERO(&writeSet);
        FD_SET(listenSock, &readSet);
        fd_max = listenSock;

        while(1)
        {
            readSetCopy = readSet;
            writeSetCopy = writeSet;
            
            timeout.tv_sec = 5;
            timeout.tv_usec = 0;

            // Polling
            if ((fd_num = select(fd_max + 1, &readSetCopy, &writeSetCopy, 0, &timeout)) == -1)
                break;
            if (fd_num == 0)
                continue;

            // 클라이언트 연결
            if (FD_ISSET(listenSock, &readSetCopy))
            {
                User newConnection;
                clientAddrSize = sizeof(clientAddr);
                clientSock = accept(listenSock, (struct sockaddr*)&clientAddr, &clientAddrSize);

                _users.insert(std::pair<int, User>(clientSock, newConnection));

                fcntl(clientSock, F_SETFL, O_NONBLOCK);

                FD_SET(clientSock, &readSet);
                if (fd_max < clientSock)
                    fd_max = clientSock;
                std::cout << "connected : " << clientSock << std::endl;
            }

            std::map<int, User>::iterator it = _users.begin();
            while (it != _users.end())
            {
                if (FD_ISSET(it->first, &readSetCopy))
                {
                    // 소켓 읽기 및 연결 종료
                    if (it->second.readMessage(it->first) <= 0)
                    {
                        // 연결 종료
                        FD_CLR(it->first, &readSet);
                        close(it->first);
                        std::cout << "cntl + c, connection closed : " << it->first << std::endl;
                        // 원소 삭제
                        if (it->first == fd_max)
                        {
                            std::map<int, User>::iterator it_cpy = it;
                            if (it_cpy != _users.begin())
                            {
                                --it_cpy;
                                fd_max = it_cpy->first;
                            }
                            else
                            {
                                fd_max = listenSock;
                            }
                        }
                        it = _users.erase(it);
                        continue;
                    }
                    if (it->second.isReadDone() == READ_DONE)
                        FD_SET(it->first, &writeSet);
                }

                // 메시지 전송
                if (FD_ISSET(it->first, &writeSetCopy))
                {
                    if (it->second.isPayloadDone() == false)
                    {
                        it->second.makeResponse();
                        ++it;
                        continue;
                    }
                    // 전송 및 연결 종료
                    if (it->second.writeReply(it->first) == 0)
                    {
                        // 연결 종료
                        FD_CLR(it->first, &readSet);
                        FD_CLR(it->first, &writeSet);
                        close(it->first);
                        std::cout << "response sent, connection closed : " << it->first << std::endl;
                        // 원소 삭제
                        if (it->first == fd_max)
                        {
                            std::map<int, User>::iterator it_cpy = it;
                            if (it_cpy != _users.begin())
                            {
                                --it_cpy;
                                fd_max = it_cpy->first;
                            }
                            else
                            {
                                fd_max = listenSock;
                            }
                        }
                        it = _users.erase(it);
                    }
                    else
                        ++it;
                    
                    continue;
                }

                ++it;
            }
        }
        close(listenSock);
    }
};

#endif