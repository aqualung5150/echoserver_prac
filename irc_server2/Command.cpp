#include "Command.hpp"

Command::Command(Server *server, User *sender)
: _server(server), _sender(sender), _command(""), _trailing("")
{
    std::string message = _sender->getMessage();
    std::stringstream stream;
    std::string buf;

    message.erase(message.find("\r\n")); // crlf 제거

    if (message.find(':') != std::string::npos) // _trailing (include ':')
    {
        _trailing.append(message.substr(message.find(':')));
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