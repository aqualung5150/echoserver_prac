#include "User.hpp"

User::User()
: _server(NULL), _socket(-1), _status(NOPASS), _registered(false), _message(""), _nick("*"), _username("*"), _realname("*")
{
}

int User::readMessage()
{
    char buf[BUF_SIZE];
    int nread;

    nread = read(_socket, buf, BUF_SIZE);

    if (nread == 0 || nread == -1)
        return (0);
    else
        _message.append(buf, nread);

    while (_message.find("\r\n") != std::string::npos)
    {
        std::cout << "READ : " << _message << std::endl;
        Command command(_server, this);
        command.execute();
        _message = _message.erase(0, _message.find("\r\n") + 2); // trim used message
    }
    if (_status == DELETE)
        return (-1);
    return (1);
}

bool User::isJoined(std::string channel)
{
    for (std::vector<Channel*>::iterator it = _joined.begin(); it != _joined.end(); ++it)
    {
        if (!(*it)->getName().compare(channel))
            return true;
    }
    return false;
}

bool User::isInvited(Channel* channel)
{
    for (std::vector<Channel*>::iterator it = _invited.begin(); it != _invited.end(); ++it)
    {
        if (*it == channel)
        {
            _invited.erase(it); // also remove invited channel
            return true;
        }
    }
    return false;
}

void User::sendNoRepeat(std::string& reply)
{
    std::vector<User*> targets;

    for (std::vector<Channel*>::iterator it = _joined.begin(); it != _joined.end(); ++it)
    {
        std::vector<User*> users = (*it)->getUsers();
        for (std::vector<User*>::iterator it2 = users.begin(); it2 != users.end(); ++it2)
        {
            if (std::find(targets.begin(), targets.end(), *it2) == targets.end() && (*it2) != this)
                targets.push_back(*it2);
        }
    }

    for (std::vector<User*>::iterator it = targets.begin(); it != targets.end(); ++it)
        send((*it)->getSocket(), reply.c_str(), reply.size(), MSG_DONTWAIT);
}

//setters
void User::setServer(Server *server)
{
    _server = server;
}

void User::setSocket(int fd)
{
    _socket = fd;
}

void User::setIP(std::string ip)
{
    _ip = ip;
}

void User::setStatus(int status)
{
    _status = status;
}

void User::setRegistered(bool registered)
{
    _registered = registered;
}

void User::setNick(std::string &nick)
{
    _nick = nick;
}

void User::setUsername(std::string &username)
{
    _username = username;
}

void User::setRealname(std::string &realname)
{
    _realname = realname;
}

void User::addJoined(Channel* channel)
{
    _joined.push_back(channel);
}

void User::removeJoined(Channel* channel)
{
    std::vector<Channel *>::iterator target = std::find(_joined.begin(), _joined.end(), channel);

    if (target != _joined.end())
        _joined.erase(target);
}

//getters
Server* User::getServer()
{
    return _server;
}

int User::getSocket() const
{
    return _socket;
}

std::string User::getIP() const
{
    return _ip;
}

int User::getStatus() const
{
    return _status;
}

bool User::getRegistered() const
{
    return _registered;
}

std::string User::getMessage() const
{
    return _message;
}

std::string User::getNick() const
{
    return _nick;
}

std::string User::getUsername() const
{
    return _username;
}

std::string User::getRealname() const
{
    return _realname;
}

std::vector<Channel*> User::getJoined()
{
    return _joined;
}