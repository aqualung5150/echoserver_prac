#include "Command.hpp"

void Command::NICK()
{
    if (_params.size() < 1 || !_trailing.empty())
    {
        // ERR_NEEDMOREPARAMS
        sendReply(_sender->getSocket(), ERR_NEEDMOREPARAMS(_server->getName(), _sender->getNick(), "NICK"));
        return;
    }

    if (!_params[0].compare(_sender->getNick()))
        return;

    for (std::map<int, User*>::iterator it = _server->getUsers().begin(); it != _server->getUsers().end(); ++it)
    {
        if (!_params[0].compare(it->second->getNick()))
        {
            // ERR_NICKNAMEINUSE
            sendReply(_sender->getSocket(), ERR_NICKNAMEINUSE(_server->getName(), _sender->getNick(), _params[0]));
            return;
        }
    }

    // Change nick
    if (_sender->getStatus() == CONNECTED)
    {
        std::string reply = RPL_NICK(_sender->getNick(), _sender->getUsername(), _sender->getIP(), _params[0]);

        sendReply(_sender->getSocket(), reply); // send to _sender
        _sender->sendNoRepeat(reply);           // send to _joined no repeat(except _sender)

        _sender->setNick(_params[0]);
        return;
    }
    // First connection
    else
    {
        _sender->setNick(_params[0]);
        connect();
    }
}