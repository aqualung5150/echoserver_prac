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
#include <algorithm>

#define BUF_SIZE 512

enum Status
{
    NOPASS,
    ALLOWED,
    CONNECTED,
    DELETE
};

class Server;

class User
{
private:
    Server *_server;
    // Socket FD
    int _socket;
    //IPv4
    std::string _ip;
    // Status
    int _status;
    // Registered
    bool _registered;
    // Command Message
    std::string _message;
    // int _readDone;
    // NICK
    std::string _nick;
    // USER
    std::string _username; // first parameter of USER command
    std::string _realname; // last parameter of USER command
    // Channel
    std::vector<Channel*> _joined;
    // invite된 채널에 join하면 _invited 목록에서 제외
    std::vector<Channel*> _invited;

public:
    User();

    int readMessage();

    bool isJoined(std::string channel);
    bool isInvited(Channel* channel);
    void sendNoRepeat(std::string& reply); //send to _joined user no repeat(except _sender)

    //setters
    void setServer(Server *server);
    void setSocket(int fd);
    void setIP(std::string ip);
    void setStatus(int status);
    void setRegistered(bool connected);
    void setNick(std::string &nick);
    void setUsername(std::string &username);
    void setRealname(std::string &realname);

    void addJoined(Channel* channel);
    void removeJoined(Channel* channel);

    //getters
    Server *getServer();
    int getSocket() const;
    std::string getIP() const;
    int getStatus() const;
    bool getRegistered() const;
    std::string getMessage() const;
    std::string getNick() const;
    std::string getUsername() const;
    std::string getRealname() const;
    std::vector<Channel*> getJoined();
};

#endif