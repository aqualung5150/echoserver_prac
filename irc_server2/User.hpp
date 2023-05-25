#ifndef USER_HPP
#define USER_HPP

#include "Server.hpp"
#include "Channel.hpp"
#include "Command.hpp"

#include <unistd.h>
#include <iostream>
#include <sstream>
#include <map>
#include <vector>

#define BUF_SIZE 512

class Server;

class User
{
private:
    Server *_server;
    // Socket FD
    int _socket;
    // Command Message
    std::string _message;
    // int _readDone;
    // NICK
    std::string _nick;
    // USER
    std::string _username; // first parameter of USER command
    std::string _realname; // last parameter of USER command
    // Invited Channel
    // invite된 채널에 join하면 _invited 목록에서 제외
    std::vector<Channel *> _invited;

public:
    User();

    int readMessage(int socket);

    //setters
    void setSocket(int fd);
    void setServer(Server *server);
    //getters
    int getSocket() const;
    std::string getMessage() const;
};

#endif