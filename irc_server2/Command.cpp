#include "Command.hpp"

Command::Command(Server *server, User *sender)
: _server(server), _sender(sender), _command(""), _trailing("")
{
    std::string message = _sender->getMessage();
    std::stringstream stream;
    std::string buf;

    message.erase(message.find("\r\n")); // crlf 제거

    if (message.find(':') != std::string::npos) // _trailing
    {
        _trailing.append(message.substr(message.find(':') + 1)); //(exclude ':')
        // _trailing.append(message.substr(message.find(':'))); //(include ':')
        message.erase(message.find(':')); // message에서 _trailing 제거
    }

    stream.str(message);

    std::getline(stream, buf, ' ');
    _command.append(buf); // _command
    buf.clear();

    while(std::getline(stream, buf, ' ')) // _params
    {
        _params.push_back(buf);
        buf.clear();
    }
}

void Command::execute()
{
    if (!_command.compare("PASS"))
        PASS();
    else if (!_command.compare("NICK"))
        NICK();
    else if (!_command.compare("USER"))
        USER();
    else if (!_command.compare("QUIT"))
        QUIT();
    else if (!_command.compare("PRIVMSG"))
        PRIVMSG();
    else if (!_command.compare("JOIN"))
        JOIN();
    else if (!_command.compare("PART"))
        PART();
}

void Command::sendReply(int fd, std::string reply)
{
    int ret;

	ret = send(fd, reply.c_str(), reply.size(), MSG_DONTWAIT);
	if (ret == -1)
		std::cerr << "Error: " << reply.c_str() << std::endl;
	std::cout << "Send to client " << fd << std::endl;
	std::cout << reply << std::endl;
}

void Command::connect()
{
    if (_sender->getStatus() == NOPASS && _sender->getNick().compare("*") && _sender->getRegistered())
    {
        _sender->setStatus(DELETE);
        std::string reply = "ERROR :Closing link: (" + _sender->getUsername() + "@" + _sender->getIP() + ") [Access denied by configuration]\r\n";
        sendReply(_sender->getSocket(), reply);
        return ;
    }

    if (_sender->getStatus() == ALLOWED && _sender->getNick().compare("*") && _sender->getRegistered())
    {
        int socket = _sender->getSocket();

        _sender->setStatus(CONNECTED);
        // RPL_WELCOME
        sendReply(socket, RPL_WELCOME(_server->getName(), _sender->getNick(), _sender->getUsername(), _sender->getIP()));
        // RPL_YOURHOST
        sendReply(socket, RPL_YOURHOST(_server->getName(), _sender->getNick()));
        // RPL_CREATED
        sendReply(socket, RPL_CREATED(_server->getName(), _sender->getNick()));
        // RPL_MYINFO
        sendReply(socket, RPL_MYINFO(_server->getName(), _sender->getNick(), "-", "iklot"));

        std::cout << _sender->getNick() << " has joined the server." << std::endl;
    }
}