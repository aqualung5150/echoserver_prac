#include "Command.hpp"

void Command::PRIVMSG()
{
    if (_params.size() < 1 || _trailing.empty())
    {
        sendReply(_sender->getSocket(), ERR_NEEDMOREPARAMS(_server->getName(), _sender->getNick(), "PRIVMSG"));
        return;
    }

    

    //init parameters
    std::vector<std::string> target;
    std::string buf;
    std::stringstream stream;
    stream.str(_params[0]);
    while (std::getline(stream, buf, ','))
    {
        target.push_back(buf);
        buf.clear();
    }

    for (std::vector<std::string>::iterator it = target.begin(); it != target.end(); ++it)
    {
        // make reply message
        std::string reply = RPL_PRIVMSG(_sender->getNick(), _sender->getUsername(), _sender->getIP(), *it, _trailing);
        
        // Msg to channel
        if ((*it)[0] == '#')
        {
            // if( No such cahnnel - 403 )
            //:irc.local 403 zzz #mmmm :No such channel
            if (_server->getChannel(*it) == NULL)
            {
                sendReply(_sender->getSocket(), ERR_NOSUCHCHANNEL(_server->getName(), _sender->getNick(), *it));
                continue;
            }

            // if( Not join the channel - ERR_CANNOTSENDTOCHAN - 404 )
            //:irc.local 404 zzz #yyyy :You cannot send external messages to this channel whilst the +n (noextmsg) mode is set.
            if (!_sender->isJoined(*it))
            {
                sendReply(_sender->getSocket(), ERR_CANNOTSENDTOCHAN(_server->getName(), _sender->getNick(), *it));
                continue;
            }

            // Send message every user in this channel except _sender
            _server->getChannel(*it)->sendReply(reply, _sender);
            continue;
        }

        // No such nick - ERR_NOSUCHNICK - 401
        if (_server->getUser(*it) == NULL)
        {
            sendReply(_sender->getSocket(), ERR_NOSUCHNICK(_server->getName(), _sender->getNick(), *it));
            return ;
        }

        // Msg to user
        sendReply(_server->getUser(*it)->getSocket(), reply);
    }
}