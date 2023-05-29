#ifndef COMMAND_HPP
#define COMMAND_HPP

#include "Server.hpp"
#include "User.hpp"
#include "Channel.hpp"
#include "Reply.hpp"

// MODE - Bit Mask
// #define MODE_I 0x0001 // Invite mode
// #define MODE_T 0x0002 // restrict topic mode
// #define MODE_K 0x0004 // channel key

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
    void sendReply(int fd, std::string reply);
    void connect();
    void PASS();
    void NICK();
    void USER();
    void QUIT();
    void PRIVMSG();
    void JOIN();
    void PART();
};

#endif