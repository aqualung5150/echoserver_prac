#ifndef COMMAND_HPP
#define COMMAND_HPP

#include "Server.hpp"
#include "User.hpp"
#include "Channel.hpp"

class Server;

class Command
{
private:
    Server *_server;
    User *_sender;

    std::string _command;
    std::vector<std::string> _params;
    std::string _trailing;

public:
    Command(Server *server, User *sender);

    // test
    void testPrint();
};

#endif