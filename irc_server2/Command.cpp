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
        sendReply(socket, RPL_MYINFO(_server->getName(), _sender->getNick(), "-", "ikot"));

        std::cout << _sender->getNick() << " has joined the server." << std::endl;
    }
}

// void Command::PASS()
// {
//     if (_params.size() < 1)
//     {
//         sendReply(_sender->getSocket(), ERR_NEEDMOREPARAMS(_server->getName(), "*", "PASS"));
//         return;
//     }

//     if (_params[0].compare(_server->getPassword()))
//     {
//         _sender->setStatus(DELETE);
//         std::string reply = "ERROR :Closing link: (" + _sender->getUsername() + "@" + _sender->getIP() + ") [Access denied by configuration]\r\n";
//         sendReply(_sender->getSocket(), reply);
//     }
//     else
//         _sender->setStatus(ALLOWED);
// }

// void Command::NICK()
// {
//     if (_params.size() < 1 || !_trailing.empty())
//     {
//         // ERR_NEEDMOREPARAMS
//         sendReply(_sender->getSocket(), ERR_NEEDMOREPARAMS(_server->getName(), _sender->getNick(), "NICK"));
//         return;
//     }

//     if (!_params[0].compare(_sender->getNick()))
//         return;

//     for (std::map<int, User*>::iterator it = _server->getUsers().begin(); it != _server->getUsers().end(); ++it)
//     {
//         if (!_params[0].compare(it->second->getNick()))
//         {
//             // ERR_NICKNAMEINUSE
//             sendReply(_sender->getSocket(), ERR_NICKNAMEINUSE(_server->getName(), _sender->getNick(), _params[0]));
//             return;
//         }
//     }

//     // Change nick
//     if (_sender->getStatus() == CONNECTED)
//     {
//         std::string reply = ":" + _sender->getNick() + "!" + _sender->getUsername() + "@" + _sender->getIP() + " NICK :" + _params[0] + "\r\n";

//         sendReply(_sender->getSocket(), reply); // send to _sender

//         std::vector<Channel*> joined = _sender->getJoined(); // send to every user in joined channel
//         for (std::vector<Channel*>::iterator it = joined.begin(); it != joined.end(); ++it)
//             (*it)->sendReply(reply, _sender);

//         _sender->setNick(_params[0]);
//         return;
//     }
//     // First connection
//     else
//     {
//         _sender->setNick(_params[0]);
//         connect();
//     }
// }

// void Command::USER()
// {
//     if (_params.size() < 3 || _trailing.empty())
//     {
//         // ERR_NEEDMOREPARAMS
//         sendReply(_sender->getSocket(), ERR_NEEDMOREPARAMS(_server->getName(), _sender->getNick(), "USER"));
//         return;
//     }
//     else if (_sender->getRegistered())
//     {
//         // ERR_ALREADYREGISTRED
//         sendReply(_sender->getSocket(), ERR_ALREADYREGISTRED(_server->getName(), _sender->getNick()));
//         return;
//     }

//     // Register
//     _sender->setUsername(_params[0]);
//     _sender->setRealname(_trailing);
//     _sender->setRegistered(true);

//     // RPL_WELCOME
//     connect();
// }

// void Command::QUIT()
// {
//     _sender->setStatus(DELETE);

//     std::string reply;

//     // reply to _sender
//     if (_trailing.empty())
//         reply = "ERROR :Closing link: (" + _sender->getUsername() + "@" + _sender->getIP() + ") [Client exited]\r\n";
//     else
//         reply = "ERROR :Closing link: (" + _sender->getUsername() + "@" + _sender->getIP() + ") [Quit: " + _trailing + "]\r\n";
//     sendReply(_sender->getSocket(), reply);

//     // reply to _joind channel
//     if (_trailing.empty())
//         reply = ":" + _sender->getNick() + "!" + _sender->getUsername() + "@" + _sender->getIP() + " QUIT :Client exited\r\n";
//     else
//         reply = ":" + _sender->getNick() + "!" + _sender->getUsername() + "@" + _sender->getIP() + " QUIT :Quit: " + _trailing + "\r\n";
//     std::vector<Channel*> channels = _sender->getJoined();
//     for (std::vector<Channel*>::iterator it = channels.begin(); it != channels.end(); ++it)
//         (*it)->sendReply(reply, _sender);
// }

// void Command::PRIVMSG()
// {
//     if (_params.size() < 1 || _trailing.empty())
//     {
//         sendReply(_sender->getSocket(), ERR_NEEDMOREPARAMS(_server->getName(), _sender->getNick(), "PRIVMSG"));
//         return;
//     }

//     std::string reply = ":" + _sender->getNick() + "!" + _sender->getUsername() + "@" + _sender->getIP() + " PRIVMSG " + _params[0] + " :" + _trailing + "\r\n";

//     // Msg to channel
//     if (_params[0][0] == '#')
//     {
//         // if( No such cahnnel - 403 )
//         //:irc.local 403 zzz #mmmm :No such channel
//         if (_server->getChannel(_params[0]) == NULL)
//         {
//             sendReply(_sender->getSocket(), ERR_NOSUCHCHANNEL(_server->getName(), _sender->getNick(), _params[0]));
//             return;
//         }

//         // if( Not join the channel - ERR_CANNOTSENDTOCHAN - 404 )
//         //:irc.local 404 zzz #yyyy :You cannot send external messages to this channel whilst the +n (noextmsg) mode is set.
//         if (!_sender->isJoined(_params[0]))
//         {
//             sendReply(_sender->getSocket(), ERR_CANNOTSENDTOCHAN(_server->getName(), _sender->getNick(), _params[0]));
//             return;
//         }

//         // Send message every user in this channel except _sender
//         _server->getChannel(_params[0])->sendReply(reply, _sender);
//         return;
//     }

//     // No such nick - ERR_NOSUCHNICK - 401
//     if (_server->getUser(_params[0]) == NULL)
//     {
//         sendReply(_sender->getSocket(), ERR_NOSUCHNICK(_server->getName(), _sender->getNick(), _params[0]));
//         return ;
//     }

//     // Msg to user
//     sendReply(_server->getUser(_params[0])->getSocket(), reply);
// }


// //todo
// void Command::JOIN()
// {
//     if (_params.size() < 1)
//     {
//         sendReply(_sender->getSocket(), ERR_NEEDMOREPARAMS(_server->getName(), _sender->getNick(), "PASS"));
//         return;
//     }

//     // Not registered
//     // :irc.local 451 <nick> JOIN :You have not registered.
//     if (_sender->getStatus() != CONNECTED)
//     {
//         sendReply(_sender->getSocket(), ERR_NOTREGISTERED(_server->getName(), _sender->getNick(), "JOIN"));
//         return;
//     }

//     // NO # sign - ERR_BADCHANMASK
//     //:irc.local 476 <nick> tradis :Invalid channel name
//     if (_params[0][0] != '#')
//     {
//         sendReply(_sender->getSocket(), ERR_BADCHANMASK(_server->getName(), _sender->getNick(), _params[0]));
//         return;
//     }
    
//     //todo

//     //reply
//     //:zzz!root@127.0.0.1 JOIN :#yyyy
//     std::string reply = ":" + _sender->getNick() + "!" + _sender->getUsername() + "@" + _sender->getIP() + " JOIN :" + _params[0] + "\r\n";
//     //Create channel
//     if (_server->getChannel(_params[0]) == NULL)
//     {
//         Channel* newChannel = new Channel();
//         newChannel->setName(_params[0]);
//         newChannel->addUser(_sender);
//         newChannel->addOperator(_sender);

//         _server->addChannel(newChannel);
//         _sender->addJoined(newChannel);
//     }
//     // Join already exist
//     else
//     {
//         _server->getChannel(_params[0])->addUser(_sender);
//         _sender->addJoined(_server->getChannel(_params[0]));
//     }

//     Channel* channel = _server->getChannel(_params[0]);

//     //Make user list
//     std::string userList = "";
//     std::vector<User*> users = channel->getUsers();
//     for (std::vector<User*>::iterator it = users.begin(); it != users.end(); ++it)
//     {
//         if (channel->isOperator(*it))
//             userList += "@" + (*it)->getNick();
//         else
//             userList += (*it)->getNick();

//         if (it != users.end() - 1)
//             userList += " ";
//     }

//     channel->sendReply(reply);
//     sendReply(_sender->getSocket(), RPL_NAMEREPLY(_server->getName(), _sender->getNick(), _params[0], userList));
//     sendReply(_sender->getSocket(), RPL_ENDOFNAMES(_server->getName(), _sender->getNick(), _params[0]));
// }