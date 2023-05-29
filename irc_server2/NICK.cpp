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
        std::string reply = ":" + _sender->getNick() + "!" + _sender->getUsername() + "@" + _sender->getIP() + " NICK :" + _params[0] + "\r\n";

        sendReply(_sender->getSocket(), reply); // send to _sender

        std::vector<Channel*> joined = _sender->getJoined(); // send to every user in joined channel
        for (std::vector<Channel*>::iterator it = joined.begin(); it != joined.end(); ++it)
            (*it)->sendReply(reply, _sender);

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