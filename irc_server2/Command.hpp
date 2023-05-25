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

    // received message
    std::string _command;
    std::vector<std::string> _params;
    std::string _trailing;

    // reply
    std::string _reply;

public:
    Command(Server *server, User *sender);

    // test
    void testPrint();

    // execute
    void execute();
    void NICK();
    void USER();
};

#endif