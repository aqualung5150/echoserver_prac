#include "Command.hpp"

//todo
void Command::JOIN()
{
    // Error

    // Need more params
    if (_params.size() < 1)
    {
        sendReply(_sender->getSocket(), ERR_NEEDMOREPARAMS(_server->getName(), _sender->getNick(), "PASS"));
        return;
    }

    // Not registered
    // :irc.local 451 <nick> JOIN :You have not registered.
    if (_sender->getStatus() != CONNECTED)
    {
        sendReply(_sender->getSocket(), ERR_NOTREGISTERED(_server->getName(), _sender->getNick(), "JOIN"));
        return;
    }

    // init parameters - map
    std::map<std::string, std::string> nameKey;

    std::string nameBuf;
    std::string keyBuf;
    std::stringstream nameStream;
    std::stringstream keyStream;

    nameStream.str(_params[0]);
    if (_params.size() > 1)
        keyStream.str(_params[1]);
    
    while (std::getline(nameStream, nameBuf, ','))
    {
        nameKey.insert(std::pair<std::string, std::string>(nameBuf, ""));

        if (_params.size() > 1)
        {
            if (std::getline(keyStream, keyBuf, ','))
                nameKey.find(nameBuf)->second = keyBuf;
            keyBuf.clear();
        }
        nameBuf.clear();
    }

    // init parameters
    // std::vector<std::string> name;
    // std::vector<std::string> key;

    // std::string buf;
    // std::stringstream stream;
    // stream.str(_params[0]);

    // while(std::getline(stream, buf, ','))
    // {
    //     name.push_back(buf);
    //     buf.clear();
    // }

    // if (_params.size() > 1)
    // {
    //     stream.str(_params[1]);
    //     while (std::getline(stream, buf, ','))
    //     {
    //         key.push_back(buf);
    //         buf.clear();
    //     }
    // }

    for (std::map<std::string, std::string>::iterator it = nameKey.begin(); it != nameKey.end(); ++it)
    {
        // NO # sign - ERR_BADCHANMASK
        //:irc.local 476 <nick> tradis :Invalid channel name
        if (it->first[0] != '#')
        {
            sendReply(_sender->getSocket(), ERR_BADCHANMASK(_server->getName(), _sender->getNick(), it->first));
            continue;
        }

        // Join the channel
        Channel* channel = _server->getChannel(it->first);

        //reply
        //:zzz!root@127.0.0.1 JOIN :#yyyy
        std::string reply = ":" + _sender->getNick() + "!" + _sender->getUsername() + "@" + _sender->getIP() + " JOIN :" + it->first + "\r\n";

        if (channel == NULL)
        {
            Channel* newChannel = new Channel();
            newChannel->setName(it->first);
            newChannel->addUser(_sender);
            newChannel->addOperator(_sender);

            _server->addChannel(newChannel);
            _sender->addJoined(newChannel);
            channel = newChannel;
        }
        // Join already exist
        else
        {
            // TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO
            // TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO
            // if ( mode == +i ) - _sender.invited()?
            // if ( mode == +k ) - it->sec == channel->_password ?
            
            // if ( already in this channel ) ignore
            if (_sender->isJoined(it->first))
                continue;

            channel->addUser(_sender);
            _sender->addJoined(channel);
        }

        //Make user list for RPL_NAMEREPLY
        std::string userList = "";
        std::vector<User*> users = channel->getUsers();
        for (std::vector<User*>::iterator it = users.begin(); it != users.end(); ++it)
        {
            if (channel->isOperator(*it))
                userList += "@" + (*it)->getNick();
            else
                userList += (*it)->getNick();

            if (it != users.end() - 1)
                userList += " ";
        }

        // send
        channel->sendReply(reply);
        sendReply(_sender->getSocket(), RPL_NAMEREPLY(_server->getName(), _sender->getNick(), it->first, userList));
        sendReply(_sender->getSocket(), RPL_ENDOFNAMES(_server->getName(), _sender->getNick(), it->first));
    }
}