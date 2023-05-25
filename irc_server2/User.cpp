#include "User.hpp"

User::User()
: _server(NULL), _socket(-1), _message(""), _nick(""), _username(""), _realname("")
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
        command.testPrint();
        _message = _message.erase(0, _message.find("\r\n") + 2); // trim used message
    }
    return (1);
}

void User::setSocket(int fd)
{
    _socket = fd;
}

void User::setServer(Server *server)
{
    _server = server;
}

int User::getSocket() const
{
    return _socket;
}

std::string User::getMessage() const
{
    return _message;
}