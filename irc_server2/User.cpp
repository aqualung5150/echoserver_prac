#include "User.hpp"

User::User()
: _server(NULL), _socket(-1), _connected(false), _message(""), _nick(""), _username(""), _realname("")
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
        // command.testPrint();
        command.execute();
        _message = _message.erase(0, _message.find("\r\n") + 2); // trim used message
    }
    return (1);
}

void User::setServer(Server *server)
{
    _server = server;
}

void User::setSocket(int fd)
{
    _socket = fd;
}

void User::setConnected(bool connected)
{
    _connected = connected;
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

int User::getSocket() const
{
    return _socket;
}

bool User::getConnected() const
{
    return _connected;
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