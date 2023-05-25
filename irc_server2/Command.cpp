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

void Command::testPrint()
{
    if (_command.empty())
        std::cout << "Command :\tNone" << std::endl;
    else
        std::cout << "Command :\t" << _command << std::endl;
    for (std::vector<std::string>::iterator it = _params.begin(); it != _params.end(); ++it)
        std::cout << "Parameter :\t" << *it << std::endl;
    if (_trailing.empty())
        std::cout << "Trailing :\tNone" << std::endl;
    else
        std::cout << "Trailing :\t" << _trailing << std::endl;
    std::cout << "------------" << std::endl;
}

void Command::NICK()
{
    if (_params.size() != 1 || !_trailing.empty())
    {
        // err
    }
    //else if (NICK IN USE)
    else
    {
        _sender->setNick(_params[0]);
    }
}

void Command::USER()
{
    if (_params.size() != 3 || _trailing.empty())
    {
        //err
    }
    else
    {
        _sender->setUsername(_params[0]);
        _sender->setRealname(_trailing);
    }

    if (!_sender->getNick().empty() && !_sender->getUsername().empty() && !_sender->getRealname().empty())
    {
        std::string reply;
        reply = ":seunchoi.ft_irc.com 001 " + _sender->getNick() + " :Welcome to ft_irc server!\r\n";

        std::cout << "nick : " << _sender->getNick() << std::endl;
        std::cout << "username : " << _sender->getUsername() << std::endl;
        std::cout << "realname : " << _sender->getRealname() << std::endl;
        
        write(_sender->getSocket(), reply.c_str(), reply.length());
    }
}

void Command::execute()
{
    if (!_command.compare("NICK"))
        NICK();
    if (!_command.compare("USER"))
        USER();
}