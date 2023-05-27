#include "User.hpp"

User::User()
: _server(NULL), _socket(-1), _status(NOPASS), _registered(false), _message(""), _nick("*"), _username("*"), _realname("*")
{
}

int User::readMessage(int socket)
{
    char buf[BUF_SIZE];
    int nread;

    nread = read(socket, buf, BUF_SIZE);

    if (nread == 0 || nread == -1)
        return (0);
    else
        _message.append(buf, nread);

    while (_message.find("\r\n") != std::string::npos)
    {
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