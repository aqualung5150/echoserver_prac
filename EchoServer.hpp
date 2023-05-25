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

class User
{
private:
    bool _status;
    std::string _request;
    std::string _response;
    const char* _writeBuffer;
    size_t _writeBufferLength, _writeBufferSent;
public:
    User()
    : _status(false), _request(""), _response("---RESPONSE---\n"), _writeBuffer(NULL), _writeBufferLength(0), _writeBufferSent(0)
    {
    }

    bool getStatus()
    {
        return _status;
    }

    int readMessage(int socket)
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
    void makeReply()
    {
        _response.append(_request);
        _response.erase(_response.find("\nEOF"), 4);
        _response.append("--------------");
        _writeBuffer = _response.c_str();
        _writeBufferLength = strlen(_writeBuffer);
        _writeBufferSent = 0;
    }
    
    int writeReply(int socket)
    {
        // size_t len = BUF_SIZE;

        // if (_response.length() < BUF_SIZE)
        //     len = _response.length();

        // write(socket, _response.c_str(), len);
        // _response.erase(0, len);
        // if (_response.empty())
        //     return (0);
        // return (1);

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
        listenAddr.sin_family = AF_INET; // 주소체계
        listenAddr.sin_addr.s_addr = htonl(INADDR_ANY); // htonl(), htons()는 빅엔디안으로 데이터를 정렬함
        listenAddr.sin_port = htons(port);

        // 사용의 편의를 위해 sockaddr_in 구조체로 데이터를 처리한 뒤 sockaddr로 형변환
        bind(listenSock, (struct sockaddr*)&listenAddr, sizeof(listenAddr));
        listen(listenSock, 512);

        FD_ZERO(&readSet);
        FD_ZERO(&writeSet);
        FD_SET(listenSock, &readSet);
        fd_max = listenSock;

        while(1)
        {
            // select() 이후에는 원본(fds)을 알 수 없으므로 readFDs에 복사하여 사용한다.
            readSetCopy = readSet;
            writeSetCopy = writeSet;
            
            timeout.tv_sec = 5;
            timeout.tv_usec = 0;

            /*
            1. readfds에 데이터가 없다면 잡지 않음(event가 없음)
            2. writefds를 등록해 두었다면 쓰기(write)가능한 소켓이 있다면 계속 잡아줌.
            3. 둘 다 event가 없다면 timeout에 설정한 시간 동안 block되어 있다가 풀림.
            */
            if ((fd_num = select(fd_max + 1, &readSetCopy, &writeSetCopy, 0, &timeout)) == -1)
                break;
            if (fd_num == 0)
            {
                std::cout << "select() timeout" << std::endl;
                continue;
            }

            // listenSock에 이벤트가 발생했다면 해당 클라이언트를 accept()
            // select는 최대 1024개의 fd만 검사함 -> select()대신 poll()을 이용하면 해결
            if (FD_ISSET(listenSock, &readSetCopy))
            {
                User newConnection;
                clientAddrSize = sizeof(clientAddr);
                clientSock = accept(listenSock, (struct sockaddr*)&clientAddr, &clientAddrSize);

                // map에도 추가해줌
                _users.insert(std::pair<int, User>(clientSock, newConnection));

                /*
                소켓은 논블록처리함
                -> select에서 이미 읽기/쓰기가 가능한 소켓을 알려주지만 막상 소켓에 read/write 했더니 block이 될 수 있음.
                */
                fcntl(clientSock, F_SETFL, O_NONBLOCK);

                FD_SET(clientSock, &readSet);
                if (fd_max < clientSock)
                    fd_max = clientSock;
                std::cout << "connected : " << clientSock << std::endl;
            }

            std::map<int, User>::iterator it = _users.begin();
            while (it != _users.end())
            // for (std::map<int, Connection>::iterator it = _connections.begin(); it != _connections.end(); ++it)
            {
                if (FD_ISSET(it->first, &readSetCopy))
                {
                    if (it->second.readMessage(it->first) <= 0)
                    {
                        FD_CLR(it->first, &readSet);
                        close(it->first);
                        std::cout << "cntl + c, connection closed : " << it->first << std::endl;

                        // fd_max를 새로 설정해주고 해당 원소 삭제
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
                    if (it->second.getStatus() == READ_DONE)
                    {
                        FD_SET(it->first, &writeSet);
                        it->second.makeReply();
                    }
                    ++it;
                    continue;
                }

                

                if (FD_ISSET(it->first, &writeSetCopy))
                {
                    if (it->second.writeReply(it->first) == 0)
                    {
                        FD_CLR(it->first, &readSet);
                        FD_CLR(it->first, &writeSet);
                        close(it->first);
                        std::cout << "response sent, connection closed : " << it->first << std::endl;

                        // fd_max를 새로 설정해주고 해당 원소 삭제
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