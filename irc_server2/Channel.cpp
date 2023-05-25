#include "Channel.hpp"

Channel::Channel(User *creater)
: _topic(""), _inviteOnly(false), _restrictedTopic(true), _password("")
{
    _users.insert(std::pair<int, User*>(creater->getSocket(), creater));
    _operators.push_back(creater);
}