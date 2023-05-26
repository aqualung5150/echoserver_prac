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

void Command::NICK()
{
    if (_params.size() != 1 || !_trailing.empty())
    {
        // ERR_NEEDMOREPARAMS (매크로로 재활용 가능)
        std::string reply = ":irc.local 461 * NICK :Not enough parameters.\r\n";
        write(_sender->getSocket(), reply.c_str(), reply.length());
        return;
    }

    if (!_params[0].compare(_sender->getNick()))
        return;

    for (std::map<int, User*>::iterator it = _server->getUsers().begin(); it != _server->getUsers().end(); ++it)
    {
        if (!_params[0].compare(it->second->getNick()))
        {
            // ERR_NICKNAMEINUSE
            std::string reply = ":ft_irc 433 * " + _params[0] + " :Nickname is already in use.\r\n";
            write(_sender->getSocket(), reply.c_str(), reply.length());
            return;
        }
    }

    _sender->setNick(_params[0]);

    // Change nick
    if (_sender->getStatus() == CONNECTED)
    {
        std::string reply = ":ft_irc NICK :" + _sender->getNick() + "\r\n";
        write(_sender->getSocket(), reply.c_str(), reply.length());
        return;
    }
    
    // Init nick (first connection - RPL_WELCOME)
    if (_sender->getStatus() == ALLOWED && !_sender->getNick().empty() && _sender->getRegistered())
    {
        std::string reply;

        _sender->setStatus(CONNECTED);
        // RPL_WELCOME
        reply = ":ft_irc 001 " + _sender->getNick() + " :Welcome to ft_irc server! " + _sender->getNick() + "\r\n";

        std::cout << _sender->getNick() << " has joined the server." << std::endl;
        // std::cout << "username : " << _sender->getUsername() << std::endl;
        // std::cout << "realname : " << _sender->getRealname() << std::endl;

        write(_sender->getSocket(), reply.c_str(), reply.length());
    }
}

void Command::USER()
{
    if (_params.size() != 3 || _trailing.empty())
    {
        // ERR_NEEDMOREPARAMS (매크로로 재활용 가능)
        std::string reply = ":irc.local 461 * USER :Not enough parameters.\r\n";
        write(_sender->getSocket(), reply.c_str(), reply.length());
        return;
    }
    else if (_sender->getRegistered())
    {
        // ERR_ALREADYREGISTRED
        std::string reply = ":irc.local 462 " + _sender->getNick() + " :You may not reregister\r\n";
        write(_sender->getSocket(), reply.c_str(), reply.length());
        return;
    }
    else
    {
        _sender->setUsername(_params[0]);
        _sender->setRealname(_trailing);
        _sender->setRegistered(true);
    }

    if (_sender->getStatus() == ALLOWED && !_sender->getNick().empty() && _sender->getRegistered())
    {
        std::string reply;

        _sender->setStatus(CONNECTED);
        // RPL_WELCOME
        reply = ":ft_irc 001 " + _sender->getNick() + " :Welcome to ft_irc server! " + _sender->getNick() + "\r\n";

        std::cout << _sender->getNick() << " has joined the server." << std::endl;
        // std::cout << "username : " << _sender->getUsername() << std::endl;
        // std::cout << "realname : " << _sender->getRealname() << std::endl;

        write(_sender->getSocket(), reply.c_str(), reply.length());
    }
}

void Command::QUIT()
{
    // 채널에서 유저 지우기
    // std::vector<Channel*> channels = _sender->getJoined();

    // for (std::vector<Channel*>::iterator it = channels.begin(); it != channels.end(); ++it)
    //     (*it)->kickUser(_sender->getNick());
    // _server->disconnect(_sender);
    
    _sender->setStatus(DELETE);
    // reply (ERROR: Closing Link)
}

void Command::PASS()
{
    //temp
    if (_params[0].compare(_server->getPassword()))
        _sender->setStatus(DELETE);
        // reply (ERROR: Closing Link)
    else
        _sender->setStatus(ALLOWED);
}