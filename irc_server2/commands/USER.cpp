#include "Command.hpp"

void Command::USER()
{
    if (_params.size() < 3 || _trailing.empty())
    {
        // ERR_NEEDMOREPARAMS
        sendReply(_sender->getSocket(), ERR_NEEDMOREPARAMS(_server->getName(), _sender->getNick(), "USER"));
        return;
    }
    else if (_sender->getRegistered())
    {
        // ERR_ALREADYREGISTRED
        sendReply(_sender->getSocket(), ERR_ALREADYREGISTRED(_server->getName(), _sender->getNick()));
        return;
    }

    // Register
    _sender->setUsername(_params[0]);
    _sender->setRealname(_trailing);
    _sender->setRegistered(true);

    // RPL_WELCOME
    connect();
}