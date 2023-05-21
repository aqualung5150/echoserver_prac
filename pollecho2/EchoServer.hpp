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
#include <vector>

#define BUF_SIZE 2
#define READ_DONE true
#define OPEN_MAX 1024

class Connection
{
private:
    // 요청 메시지
    std::string _clientCommand;
    // 응답 메시지
    std::string _serverReply;
    const char* _writeBuffer;
    size_t _writeBufferLength, _writeBufferSent;
    int _readDone;

public:
    Connection()
    : _readDone(false), _clientCommand(""), _serverReply("---RESPONSE---\n"), _writeBuffer(NULL), _writeBufferLength(0), _writeBufferSent(0)
    {
    }

    int readCommand(int socket)
    {
        char buf[BUF_SIZE];
        int nread;

        nread = read(socket, buf, BUF_SIZE);

        if (nread == 0 || nread == -1)
            return (0);
        else
            _clientCommand.append(buf, nread);

        // CRLF(delimter)를 찾았다면 메시지 읽기 완료
        if (_clientCommand.find("\nEOF\n") != std::string::npos)
            _readDone = READ_DONE;
        return (1);
    }

    bool isReadDone()
    {
        return (_readDone);
    }

    void makeReply()
    {
        _serverReply.append(_clientCommand + "--------------\n");

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
            _clientCommand.clear();
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
    // std::vector<struct pollfd> _client;
    std::map<int, Connection> _connections;
    // std::vector<Connection> _connections;
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
        int polled;

        std::vector<struct pollfd> client;

        listenSock = socket(PF_INET, SOCK_STREAM, 0);
        ft_bzero(&listenAddr, sizeof(listenAddr));
        listenAddr.sin_family = AF_INET;
        listenAddr.sin_addr.s_addr = htonl(INADDR_ANY);
        listenAddr.sin_port = htons(port);

        bind(listenSock, (struct sockaddr*)&listenAddr, sizeof(listenAddr));
        listen(listenSock, 512);

        struct pollfd listenPoll;
        listenPoll.fd = listenSock;
        listenPoll.events = POLLIN;
        client.push_back(listenPoll);

        while (1)
        {
            polled = poll(&client[0], client.size(), 5000);

            if (client[0].revents & POLLIN)
            {
                clientAddrSize = sizeof(clientAddr);
                clientSock = accept(listenSock, (struct sockaddr *)&clientAddr, &clientAddrSize);

                struct pollfd newPoll;
                newPoll.fd = clientSock;
                newPoll.events = POLLIN;
                client.push_back(newPoll);

                Connection newConnection;
                _connections.insert(std::pair<int, Connection>(clientSock, newConnection));

                std::cout << "connected : " << clientSock << std::endl;
                continue;
            }

            int count = 0;
            std::vector<struct pollfd>::iterator it = client.begin();
            ++it;
            while (it != client.end())
            {
                ++count;
                if (it->revents & POLLHUP)
                {
                    std::cout << "distconnected fd : " << it->fd << std::endl;
                    close(it->fd);
                    _connections.erase(it->fd);
                    it = client.erase(it);
                    continue;
                }

                if (it->revents & (POLLIN | POLLERR))
                {
                    if (_connections.at(it->fd).readCommand(it->fd) <= 0)
                    {
                        std::cout << "can not read" << std::endl;
                        close(it->fd);
                        _connections.erase(it->fd);
                        it = client.erase(it);
                        continue;
                    }
                    if (_connections.at(it->fd).isReadDone() == READ_DONE)
                    {
                        _connections.at(it->fd).makeReply();
                        it->events = POLLOUT;
                    }
                }

                if (it->revents & POLLOUT)
                {
                    if (_connections.at(it->fd).writeReply(it->fd) == 0)
                        it->events = POLLIN;
                }
                ++it;
            }
            std::cout << "clients : " << count << std::endl;
            std::cout << "pollfd : " << client.size() << std::endl;
        }
    }
};

#endif