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
#define READ_DONE true

class Connection
{
private:
    bool _status;
    std::string _request;
    std::string _response;
public:
    Connection()
    : _status(false), _request(""), _response("---RESPONSE---\n")
    {
    }

    bool getStatus()
    {
        return _status;
    }

    int readRequest(int socket)
    {
        char buf[BUF_SIZE];
        int nread;

        nread = read(socket, buf, BUF_SIZE);
        if (nread == 0 || nread == -1)
            return (0);
        else
        {
            _request.append(buf, nread);
        }

        // http 요청 메세지의 CRLF / Content-Length / Transfer-Encoding 등을 확인하여 요청이 완료(READ_DONE)되었는지 확인한다.
        if (_request.find("\nEOF\n") != std::string::npos)
        {
            _status = READ_DONE;
        }
        return (1);
    }

    /*
    makeResponse() :
    CRLF / Content-Length / Transfer-Encoding 에 따른 요청의 READ_DONE이 끝났다면 응답을 만들기 시작한다.
    */
    void makeResponse()
    {
        _response.append(_request);
        _response.erase(_response.find("\nEOF"), 4);
        _response.append("--------------");
    }
    
    int writeResponse(int socket)
    {
        size_t len = BUF_SIZE;

        if (_response.length() < BUF_SIZE)
            len = _response.length();

        write(socket, _response.c_str(), len);
        _response.erase(0, len);
        if (_response.empty())
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
        struct timeval timeout;

        fd_set fds, readFDs, writeFDs;

        socklen_t adr_sz;
        int fd_max, fd_num;

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

        FD_ZERO(&writeFDs);

        while(1)
        {
            readFDs = fds;
            
            timeout.tv_sec = 5;
            timeout.tv_usec = 5000;

            /*
            1. readfds에 데이터가 없다면 잡지 않음(event가 없음)
            2. writefds를 등록해 두었다면 쓰기(write)가능한 소켓이 있다면 계속 잡아줌.
            3. 둘 다 event가 없다면 timeout에 설정한 시간 동안 block되어 있다가 풀림.
            */
            if ((fd_num = select(fd_max + 1, &readFDs, &writeFDs, 0, &timeout)) == -1)
                break;

            // std::cout << "After Select" << std::endl;

            if (fd_num == 0)
                continue;

            if (FD_ISSET(listenSock, &readFDs))
            {
                Connection newConnection;
                adr_sz = sizeof(clientAddr);
                clientSock = accept(listenSock, (struct sockaddr*)&clientAddr, &adr_sz);

                _connections.insert(std::pair<int, Connection>(clientSock, newConnection));
                // _connections.insert(std::pair<int, std::string>(clientSock, ""));

                fcntl(clientSock, F_SETFL, O_NONBLOCK);
                FD_SET(clientSock, &fds);
                if (fd_max < clientSock)
                    fd_max = clientSock;
                std::cout << "connected client: " << clientSock << std::endl;
            }

            for (std::map<int, Connection>::iterator it = _connections.begin(); it != _connections.end(); ++it)
            {
                if (FD_ISSET(it->first, &readFDs))
                {
                    if (it->second.readRequest(it->first) == 0 && it->second.getStatus() != READ_DONE)
                    {
                        FD_CLR(it->first, &fds);
                        close(it->first);
                        std::cout << "cntl + c, closed client: " << it->first << std::endl;

                        // std::map의 원소를 지우는 방법 다시 생각해보기...
                        _connections.erase(it);
                        break;
                    }
                    if (it->second.getStatus() == READ_DONE)
                    {
                        FD_SET(it->first, &writeFDs);
                        it->second.makeResponse();
                    }
                    continue;
                }

                

                if (FD_ISSET(it->first, &writeFDs))
                {
                    if (it->second.writeResponse(it->first) == 0)
                    {
                        FD_CLR(it->first, &fds);    
                        FD_CLR(it->first, &writeFDs);
                        close(it->first);
                        std::cout << "response sent, closed client: " << it->first << std::endl;

                        _connections.erase(it);
                        break;
                    }
                }
            }

            

            // for (int i = 0; i < fd_max + 1; ++i)
            // {
            //     // 1. readfd에 읽을 데이터가 있거나 2. 연결을 끊었을 때 FD_ISSET에 걸림
            //     if (FD_ISSET(i, &readFDs))
            //     {
            //         nread = read(i, buf, BUF_SIZE);
            //         if (nread == 0)
            //         {
            //             FD_CLR(i, &fds);
            //             close(i);
            //             std::cout << "closed client: " << i << std::endl;
            //             // continue;
            //         }
            //         else
            //         {
            //             // Read를 하여 객체에 저장
            //             _connections.find(i)->second += std::string(buf);
            //             std::cout << "read : " << _connections.find(i)->second << std::endl;
            //             bzero(&buf, BUF_SIZE);
            //         }
            //         // read를 할 때 마다 isRequestDone() ?-> response 생성
            //         // (하지만 요청이 중간에 잘려 있다면???)
            //         continue;
            //     }

            //     /*
            //     구현하려는 http서버와 다른 점 -> writeSet을 계속해서 잡는다. 이유는
            //     read를 모두 완료하였을 때만 status플래그를 주어 writeSet에 추가한다.(또는 read하면 그냥 추가하고 continue를 넘기면 if(writeSet)에 들어오도록)
            //     write를 모두 완료하면 fd에서 제거하며 연결을 종료.
            //     */
            //     if (FD_ISSET(i, &writeFDs))
            //     {
            //         if (_connections.find(i)->second != "")
            //         {
            //             write(i, _connections.find(i)->second.c_str(), _connections.find(i)->second.length());
            //             _connections.find(i)->second = "";
            //             FD_CLR(i, &fds);
            //             close(i);
            //             std::cout << "closed client: " << i << std::endl;
            //         }
            //     }
            // }
        }

        close(listenSock);
    }
};

#endif