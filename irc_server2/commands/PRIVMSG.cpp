#include "Command.hpp"

void Command::PRIVMSG()
{
    if (_params.size() < 1 || _trailing.empty())
    {
        sendReply(_sender->getSocket(), ERR_NEEDMOREPARAMS(_server->getName(), _sender->getNick(), "PRIVMSG"));
        return;
    }

    std::string reply = ":" + _sender->getNick() + "!" + _sender->getUsername() + "@" + _sender->getIP() + " PRIVMSG " + _params[0] + " :" + _trailing + "\r\n";

    // Msg to channel
    if (_params[0][0] == '#')
    {
        // if( No such cahnnel - 403 )
        //:irc.local 403 zzz #mmmm :No such channel
        if (_server->getChannel(_params[0]) == NULL)
        {
            sendReply(_sender->getSocket(), ERR_NOSUCHCHANNEL(_server->getName(), _sender->getNick(), _params[0]));
            return;
        }

        // if( Not join the channel - ERR_CANNOTSENDTOCHAN - 404 )
        //:irc.local 404 zzz #yyyy :You cannot send external messages to this channel whilst the +n (noextmsg) mode is set.
        if (!_sender->isJoined(_params[0]))
        {
            sendReply(_sender->getSocket(), ERR_CANNOTSENDTOCHAN(_server->getName(), _sender->getNick(), _params[0]));
            return;
        }

        // Send message every user in this channel except _sender
        _server->getChannel(_params[0])->sendReply(reply, _sender);
        return;
    }

    // No such nick - ERR_NOSUCHNICK - 401
    if (_server->getUser(_params[0]) == NULL)
    {
        sendReply(_sender->getSocket(), ERR_NOSUCHNICK(_server->getName(), _sender->getNick(), _params[0]));
        return ;
    }

    // Msg to user
    sendReply(_server->getUser(_params[0])->getSocket(), reply);
}