#include "Channel.hpp"

Channel::Channel(User *creater)
: _topic(""), _inviteOnly(false), _restrictedTopic(true), _password("")
{
    // _users.insert(std::pair<int, User*>(creater->getSocket(), creater));
    _users.push_back(creater);
    _operators.push_back(creater);
}

void Channel::sendReply(std::string& reply, User *except)
{
    std::vector<User*>::iterator it = _users.begin();

    // Send to all users
    // JOIN PART ...
    if (except == NULL)
    {
        while (it != _users.end())    
        {
            send((*it)->getSocket(), reply.c_str(), reply.size(), MSG_DONTWAIT);
            ++it;
        }
    }
    // Execpt sender
    // PRIVMSG, QUIT ...
    else
    {
        while (it != _users.end())    
        {
            if (*it != except)
            {
                send((*it)->getSocket(), reply.c_str(), reply.size(), MSG_DONTWAIT);
                ++it;
            }
        }
    }
}

void Channel::kickUser(std::string nick)
{
    std::vector<User*>::iterator it;

    it = _users.begin();
    while (it != _users.end())
    {
        if (!nick.compare((*it)->getNick()))
        {
            _users.erase(it);
            break;
        }
        ++it;
    }

    it = _operators.begin();
    while (it != _users.end())
    {
        if (!nick.compare((*it)->getNick()))
        {
            _operators.erase(it);
            break;
        }
        ++it;
    }
}