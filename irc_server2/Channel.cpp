#include "Channel.hpp"

Channel::Channel(User *creater)
: _topic(""), _inviteOnly(false), _restrictedTopic(true), _password("")
{
    // _users.insert(std::pair<int, User*>(creater->getSocket(), creater));
    _users.push_back(creater);
    _operators.push_back(creater);
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