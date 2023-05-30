#include "Command.hpp"

void Command::PASS()
{
    if (_params.size() < 1)
    {
        sendReply(_sender->getSocket(), ERR_NEEDMOREPARAMS(_server->getName(), "*", "PASS"));
        return;
    }

    if (_params[0].compare(_server->getPassword()))
    {
        _sender->setStatus(DELETE);
        std::string reply = RPL_ERROR(std::string("Closing link"), _sender->getUsername(), _sender->getIP(), std::string("Access denied by configuration"));
        sendReply(_sender->getSocket(), reply);
    }
    else
        _sender->setStatus(ALLOWED);
}