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
    std::string _name;
    std::string _password;

    std::vector<struct pollfd> _pollFD;
    std::map<int, User*> _users;
    std::vector<Channel*> _channels;
public:
    Server(std::string& password);

    // While server running
    void startServer(int port);
    void disconnect(User *user);

    // Get specific object
    User* getUser(std::string& nick);
    Channel* getChannel(std::string channel);

    void addChannel(Channel* channel);
    void removeChannel(Channel* channel);

    //getters
    std::map<int, User*>& getUsers();
    std::vector<Channel*>& getChannels();
    std::string getPassword() const;
    std::string getName() const;
};

#endif