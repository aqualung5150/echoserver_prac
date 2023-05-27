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
}

static void sendReply(int fd, std::string reply)
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
    if (_sender->getStatus() == ALLOWED && !_sender->getNick().empty() && _sender->getRegistered())
    {
        int socket = _sender->getSocket();

        _sender->setStatus(CONNECTED);
        // RPL_WELCOME
        sendReply(socket, RPL_WELCOME(_server->getName(), _sender->getNick(), _sender->getUsername(), _sender->getIP()));
        // RPL_YOURHOST
        sendReply(socket, RPL_YOURHOST(_server->getName(), _sender->getNick()));
        // RPL_CREATED
        sendReply(socket, RPL_CREATED(_server->getName(), _sender->getNick()));

        std::cout << _sender->getNick() << " has joined the server." << std::endl;
    }
}

void Command::NICK()
{
    if (_params.size() < 1 || !_trailing.empty())
    {
        // ERR_NEEDMOREPARAMS
        sendReply(_sender->getSocket(), ERR_NEEDMOREPARAMS(_server->getName(), "NICK"));
        return;
    }

    if (!_params[0].compare(_sender->getNick()))
        return;

    for (std::map<int, User*>::iterator it = _server->getUsers().begin(); it != _server->getUsers().end(); ++it)
    {
        if (!_params[0].compare(it->second->getNick()))
        {
            // ERR_NICKNAMEINUSE
            sendReply(_sender->getSocket(), ERR_NICKNAMEINUSE(_server->getName(), _params[0]));
            return;
        }
    }

    // Change nick
    if (_sender->getStatus() == CONNECTED)
    {
        std::string reply = ":" + _sender->getNick() + "!" + _sender->getUsername() + "@" + _sender->getIP() + " NICK :" + _params[0] + "\r\n";
        sendReply(_sender->getSocket(), reply);
        _sender->setNick(_params[0]);
        return;
    }
    // First connection
    else
    {
        _sender->setNick(_params[0]);
        connect();
    }
}

void Command::USER()
{
    if (_params.size() < 3 || _trailing.empty())
    {
        // ERR_NEEDMOREPARAMS
        sendReply(_sender->getSocket(), ERR_NEEDMOREPARAMS(_server->getName(), "USER"));
        return;
    }
    else if (_sender->getRegistered())
    {
        // ERR_ALREADYREGISTRED
        sendReply(_sender->getSocket(), ERR_ALREADYREGISTRED(_server->getName(), _sender->getNick()));
        return;
    }

    // Register
    _sender->setUsername(_params[0]);
    _sender->setRealname(_trailing);
    _sender->setRegistered(true);

    // RPL_WELCOME
    connect();
}

void Command::QUIT()
{
    _sender->setStatus(DELETE);

    std::string reply;

    // reply to _sender
    if (_trailing.empty())
        reply = "ERROR :Closing link: (" + _sender->getUsername() + "@" + _sender->getIP() + ") [Client exited]\r\n";
    else
        reply = "ERROR :Closing link: (" + _sender->getUsername() + "@" + _sender->getIP() + ") [Quit: " + _trailing + "]\r\n";
    sendReply(_sender->getSocket(), reply);

    // reply to _joind channel
    if (_trailing.empty())
        reply = ":" + _sender->getNick() + "!" + _sender->getUsername() + "@" + _sender->getIP() + " QUIT :Client exited\r\n";
    else
        reply = ":" + _sender->getNick() + "!" + _sender->getUsername() + "@" + _sender->getIP() + " QUIT :Quit: " + _trailing + "\r\n";
    std::vector<Channel*> channels = _sender->getJoined();
    for (std::vector<Channel*>::iterator it = channels.begin(); it != channels.end(); ++it)
        (*it)->sendReply(reply, _sender);
}

void Command::PASS()
{
    if (_params.size() < 1)
    {
        sendReply(_sender->getSocket(), ERR_NEEDMOREPARAMS(_server->getName(), "PASS"));
        return;
    }

    if (_params[0].compare(_server->getPassword()))
    {
        _sender->setStatus(DELETE);
        std::string reply = "ERROR :Closing link: (" + _sender->getUsername() + "@" + _sender->getIP() + ") [Access denied by configuration]\r\n";
        sendReply(_sender->getSocket(), reply);
    }
    else
        _sender->setStatus(ALLOWED);
}