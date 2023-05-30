#include "Channel.hpp"

Channel::Channel()
: _name(""), _topic("default topic"), _mode(MODE_T), _key(""), _limit(0)
{
}

// Channel::Channel(std::string name, User *creater)
// : _name(name), _topic(""), _inviteOnly(false), _restrictedTopic(true), _password("")
// {
//     // _users.insert(std::pair<int, User*>(creater->getSocket(), creater));
//     _users.push_back(creater);
//     _operators.push_back(creater);
// }

void Channel::sendReply(std::string& reply, User* except)
{
    std::vector<User*>::iterator it = _users.begin();

    // Send to all users
    // JOIN PART MODE...
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
                send((*it)->getSocket(), reply.c_str(), reply.size(), MSG_DONTWAIT);
            ++it;
        }
    }
}

User* Channel::getUser(std::string& nick)
{
    for (std::vector<User*>::iterator it = _users.begin(); it != _users.end(); ++it)
    {
        if (!(*it)->getNick().compare(nick))
            return *it;
    }
    return NULL;
}

// void Channel::removeUser(std::string nick)
// {
//     std::vector<User*>::iterator it;

//     it = _users.begin();
//     while (it != _users.end())
//     {
//         if (!nick.compare((*it)->getNick()))
//         {
//             _users.erase(it);
//             break;
//         }
//         ++it;
//     }

//     it = _operators.begin();
//     while (it != _operators.end())
//     {
//         if (!nick.compare((*it)->getNick()))
//         {
//             _operators.erase(it);
//             break;
//         }
//         ++it;
//     }
// }

void Channel::removeUser(User* user)
{
    std::vector<User*>::iterator it;

    it = _users.begin();
    while (it != _users.end())
    {
        if (*it == user)
        {
            _users.erase(it);
            break;
        }
        ++it;
    }

    it = _operators.begin();
    while (it != _operators.end())
    {
        if (*it == user)
        {
            _operators.erase(it);
            break;
        }
        ++it;
    }
}

// void Channel::removeOperator(std::string nick)
// {
//     std::vector<User*>::iterator it;

//     it = _operators.begin();
//     while (it != _operators.end())
//     {
//         if (!nick.compare((*it)->getNick()))
//         {
//             _operators.erase(it);
//             break;
//         }
//         ++it;
//     }
// }

void Channel::removeOperator(User* user)
{
    std::vector<User*>::iterator it;

    it = _operators.begin();
    while (it != _operators.end())
    {
        if (*it == user)
        {
            _operators.erase(it);
            break;
        }
        ++it;
    }
}

void Channel::addMode(int mode)
{
    _mode += mode;
}

void Channel::removeMode(int mode)
{
    _mode -= mode;
}

bool Channel::isOperator(User* user)
{
    for (std::vector<User*>::iterator it = _operators.begin(); it != _operators.end(); ++it)
    {
        if (*it == user)
            return true;
    }
    return false;
}

bool Channel::empty()
{
    return _users.empty();
}

void Channel::setName(std::string name)
{
    _name = name;
}

void Channel::setKey(std::string key)
{
    _key = key;
}

void Channel::addUser(User* user)
{
    _users.push_back(user);
}

void Channel::addOperator(User* user)
{
    _operators.push_back(user);
}

//getters
std::string Channel::getName() const
{
    return _name;
}

std::vector<User*> Channel::getUsers()
{
    return _users;
}
std::vector<User*> Channel::getOperators()
{
    return _operators;
}

int Channel::getMode() const
{
    return _mode;
}

std::string Channel::getKey() const
{
    return _key;
}

size_t Channel::getLimit() const
{
    return _limit;
}