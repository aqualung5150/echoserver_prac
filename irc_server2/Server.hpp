#ifndef SERVER_HPP
#define SERVER_HPP

#include "User.hpp"
#include "Channel.hpp"

#include <sys/socket.h>
#include <arpa/inet.h>
#include <poll.h>
#include <fcntl.h>

#include <iostream>
#include <map>
#include <vector>

class User;
class Channel;

class Server
{
private:
    std::map<int, User*> _users;
    std::vector<Channel*> _channels;
    std::string _password;
public:
    void startServer(int port);

    //getters
    std::map<int, User*>& getUsers();
    std::vector<Channel*>& getChannels();
};

#endif