#ifndef COMMAND_HPP
#define COMMAND_HPP

#include "Server.hpp"
#include "User.hpp"
#include "Channel.hpp"
#include "Reply.hpp"

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

    // execute
    void execute();
    void sendReply(int fd, std::string message);
    void connect();
    void NICK();
    void USER();
    void QUIT();
    void PASS();
};

#endif