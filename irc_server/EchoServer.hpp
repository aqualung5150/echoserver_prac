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

#define BUF_SIZE 512
#define READ_DONE true

class Channel;

class User
{
private:
    // Socket FD
    int _socket;
    // Command Message
    std::string _message;
    int _readDone;
    // NICK
    std::string _nick;
    // USER
    std::string _username; // first parameter of USER command
    std::string _realname; // last parameter of USER command
    // Invited Channel
    // invite된 채널에 join하면 _invited 목록에서 제외
    std::vector<Channel> _invited;

public:
    void setSocket(int fd)
    {
        _socket = fd;
    }

    int getSocket()
    {
        return _socket;
    }

    std::string getMessage() const
    {
        return _message;
    }

    void clearMessage()
    {
        _message.clear();
        _readDone = false;
    }

    User()
    : _readDone(false), _message(""), _nick(""), _username(""), _realname("")
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

        std::cout << _message << std::endl;

        // CRLF(delimter)를 찾았다면 메시지 읽기 완료
        if (_message.find("\nEOF\n") != std::string::npos)
            _readDone = READ_DONE;
        return (1);
    }

    bool isReadDone()
    {
        return (_readDone);
    }

    // void makeReply()
    // {
    //     _serverReply.append(_message + "--------------\n");

    //     _writeBufferLength = _serverReply.length();
    //     _writeBuffer = _serverReply.c_str();
    //     _writeBufferSent = 0;
    // }

    // int writeReply(int socket)
    // {
    //     int len, nwrite, buf_left;

    //     buf_left = _writeBufferLength - _writeBufferSent;
    //     if (buf_left > BUF_SIZE)
    //         len = BUF_SIZE;
    //     else
    //         len = buf_left;
        
    //     if ((nwrite = write(socket, _writeBuffer + _writeBufferSent, len)) <= 0)
    //         return (-1); //error
    //     _writeBufferSent += nwrite;
    //     if (_writeBufferSent == _writeBufferLength)
    //     // 전송완료 했다면 클라이언트의 정보(메시지)를 초기화 해줌다
    //     {
    //         _readDone = false;
    //         _serverReply.clear();
    //         _message.clear();
    //         _serverReply.append("---RESPONSE---\n");
    //         return (0);
    //     }
    //     // 아직 전송하는 중
    //     return (1);
    // }
};

class Channel
{
private:
    std::map<int, User> _users; // Joined users
    std::vector<User> _operators; // Operators of this channel

    std::string _topic; // Channel's topic

    // MODE
    bool _inviteOnly;       // default : false (MODE i)
    bool _restrictedTopic;  // defualt : true (MODE t)
    std::string _password;  // defualt : ""(false) - empty string (MODE k [password])
    
    /*
    MODE

    k : Set/remove the channel key (password).
    반드시 파라미터(패스워드)를 받아야 함.

    ! MODE #channel -k wrong-password 일 경우
    :irc.local 467 seunchoi #tradis :Channel key already set 라는 reply를 받음.

    -k 하려면 올바른 password를 파라미터로 받아야 함.

    -------------------------------------------------------------------------

    o : Give/take channel operator privilege.
    채널 오퍼레이터라면 누구든지 +/- o 할 수 있음

    => 유효하지 않은 메시지는 reply하지 않음
    e.g. operator가 아닌 user에게 mode -o를 하는 경우
    */ 
public:
    Channel(User &creater)
    : _topic(""), _inviteOnly(false), _restrictedTopic(true), _password("")
    {
        _users.insert(std::pair<int, User>(creater.getSocket(), creater));
        _operators.push_back(creater);
    }
};

class EchoServer
{
private:
    std::map<int, User> _users;
    std::vector<Channel> _channels;
    std::string _password;
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

    void executeCommand(const std::string message)
    {
        /*
        1. [ ":" prefix SPACE ] - 프리픽스
            prefix     =  servername / ( nickname [ [ "!" user ] "@" host ] )
        2. command              - 명령어
        3. [ params ]           - 파라미터
        4. crlf                 - delimiter

        파라미터는 space로 구분되며
        가장 마지막 [ SPACE ":" trailing ] 에서 crlf 전까지

        예상되는 변수
        std::string prefix (클라이언트가 보내는 메세지에는 포함되지 않을 수도 있음)
        std::string command
        std::vector<std::string> params
        std::string trailer
        */
        std::stringstream commandStream;
        int socket;
        std::string line;
        std::string reply;

        // make reply
        commandStream.str(message);
        std::getline(commandStream, line,' ');
        if (!line.compare("PRIVMSG"))
        {
            reply.append("PRIVMSG ");
            std::getline(commandStream, line, ' ');
            socket = atoi(line.c_str());
            std::getline(commandStream, line);
            reply.append(line + "\n");
        }

        // send reply
        for (std::map<int, User>::iterator it = _users.begin(); it != _users.end(); ++it)
        {
            if (it->second.getSocket() == socket)
            {
                write(socket, reply.c_str(), reply.length());
                break ;
            }
        }
    }

    void serverStart(int port)
    {
        int listenSock, clientSock;
        struct sockaddr_in listenAddr, clientAddr;

        socklen_t clientAddrSize;
        int polled;

        std::vector<struct pollfd> pollFD;

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
        pollFD.push_back(listenPoll);

        while (1)
        {
            polled = poll(&pollFD[0], pollFD.size(), 5000);

            if (pollFD[0].revents & POLLIN)
            {
                clientAddrSize = sizeof(clientAddr);
                clientSock = accept(listenSock, (struct sockaddr *)&clientAddr, &clientAddrSize);
                fcntl(clientSock, F_SETFL, O_NONBLOCK);

                struct pollfd newPoll;
                newPoll.fd = clientSock;
                newPoll.events = POLLIN;
                pollFD.push_back(newPoll);

                User newUser;
                newUser.setSocket(clientSock);
                _users.insert(std::pair<int, User>(clientSock, newUser));

                std::cout << "connected : " << clientSock << std::endl;
                continue;
            }

            int count = 0;
            std::vector<struct pollfd>::iterator it = pollFD.begin();
            ++it; // 서버소켓(client[0]) 건너뛰기
            while (it != pollFD.end())
            {
                ++count;
                // 연결 종료
                if (it->revents & POLLHUP)
                {
                    std::cout << "distconnected fd : " << it->fd << std::endl;
                    close(it->fd);
                    _users.erase(it->fd);
                    it = pollFD.erase(it);
                    continue;
                }

                // 소켓 읽기
                if (it->revents & (POLLIN | POLLERR))
                {
                    if (_users.at(it->fd).readMessage(it->fd) <= 0)
                    {
                        std::cout << "can not read" << std::endl;
                        close(it->fd);
                        _users.erase(it->fd);
                        it = pollFD.erase(it);
                        continue;
                    }
                    if (_users.at(it->fd).isReadDone() == READ_DONE)
                    {
                        executeCommand(_users.at(it->fd).getMessage());
                        _users.at(it->fd).clearMessage();
                    }
                }
                ++it;
            }
            // std::cout << "connected users : " << count << std::endl;
            // std::cout << "pollfd size : " << pollFD.size() << std::endl;
        }
    }
};

#endif