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

// void Command::testPrint()
// {
//     if (_command.empty())
//         std::cout << "Command :\tNone" << std::endl;
//     else
//         std::cout << "Command :\t" << _command << std::endl;
//     for (std::vector<std::string>::iterator it = _params.begin(); it != _params.end(); ++it)
//         std::cout << "Parameter :\t" << *it << std::endl;
//     if (_trailing.empty())
//         std::cout << "Trailing :\tNone" << std::endl;
//     else
//         std::cout << "Trailing :\t" << _trailing << std::endl;
//     std::cout << "------------" << std::endl;
// }

void Command::execute()
{
    if (!_command.compare("NICK"))
        NICK();
    if (!_command.compare("USER"))
        USER();
    if (!_command.compare("QUIT"))
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
    if (_sender->getConnected())
    {
        std::string reply = ":ft_irc NICK :" + _sender->getNick() + "\r\n";
        write(_sender->getSocket(), reply.c_str(), reply.length());
        return;
    }
    
    // Init nick (first connection - RPL_WELCOME)
    if (!_sender->getConnected() && !_sender->getNick().empty() && !_sender->getUsername().empty() && !_sender->getRealname().empty())
    {
        std::string reply;

        _sender->setConnected(true);
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
    else if (_sender->getConnected())
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
    }

    if (!_sender->getConnected() && !_sender->getNick().empty() && !_sender->getUsername().empty() && !_sender->getRealname().empty())
    {
        std::string reply;

        _sender->setConnected(true);
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

}