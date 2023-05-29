#include "Command.hpp"

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
    std::vector<Channel*> channels = _sender->getJoined(); // multiple send to same user???
    for (std::vector<Channel*>::iterator it = channels.begin(); it != channels.end(); ++it)
        (*it)->sendReply(reply, _sender);
}