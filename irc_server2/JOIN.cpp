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

        // make reply to everyone on the channel
        std::string reply = RPL_JOIN(_sender->getNick(), _sender->getUsername(), _sender->getIP(), it->first);

        // Create new channel as operator
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
            // if ( already in this channel ) ignore
            if (_sender->isJoined(it->first))
                continue;

            if (_sender->isInvited(channel))
            {
                //join regardless of channel's mode
                //even though the channel is full
            }
            else if (channel->getMode() & (MODE_I | MODE_K | MODE_L))
            {
                // +k
                if (channel->getMode() & MODE_K && channel->getKey().compare(it->second))
                // mode +k but incorrect key
                {
                    sendReply(_sender->getSocket(), ERR_BADCHANNELKEY(_server->getName(), _sender->getNick(), it->first));    
                    continue;
                }

                // +i
                if (channel->getMode() & MODE_I)
                // mode +i but not invited
                {
                    sendReply(_sender->getSocket(), ERR_INVITEONLYCHAN(_server->getName(), _sender->getNick(), it->first));
                    continue;
                }

                // +l
                if (channel->getMode() & MODE_L && (channel->getUsers().size() >= channel->getLimit()))
                // mode +l but the channel is full
                {
                    sendReply(_sender->getSocket(), ERR_CHANNELISFULL(_server->getName(), _sender->getNick(), it->first));
                    continue;
                }
            }

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