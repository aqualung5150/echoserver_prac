#include "Server.hpp"

Server::Server(std::string& password)
: _password(password)
{
}

static void ft_bzero(void *s, size_t n)
{
    size_t	i;

    i = 0;
    while (i < n)
    {
        ((unsigned char *)s)[i] = 0;
        i++;
    }
}

void Server::startServer(int port)
{
    int listenSock, clientSock;
    struct sockaddr_in listenAddr, clientAddr;

    socklen_t clientAddrSize;
    int polled;

    listenSock = socket(PF_INET, SOCK_STREAM, 0);
    ft_bzero(&listenAddr, sizeof(listenAddr));
    listenAddr.sin_family = AF_INET;
    listenAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    listenAddr.sin_port = htons(port);

    bind(listenSock, (struct sockaddr*)&listenAddr, sizeof(listenAddr));
    listen(listenSock, 512);

    struct pollfd listenPoll;
    listenPoll.fd = listenSock;
    listenPoll.events = POLLIN;
    _pollFD.push_back(listenPoll);

    while (1)
    {
        polled = poll(&_pollFD[0], _pollFD.size(), 5000);

        if (_pollFD[0].revents & POLLIN)
        {
            clientAddrSize = sizeof(clientAddr);
            clientSock = accept(listenSock, (struct sockaddr *)&clientAddr, &clientAddrSize);
            fcntl(clientSock, F_SETFL, O_NONBLOCK);

            struct pollfd newPoll;
            newPoll.fd = clientSock;
            newPoll.events = POLLIN;
            _pollFD.push_back(newPoll);

            User *newUser = new User;

            newUser->setSocket(clientSock);
            newUser->setServer(this);
            _users.insert(std::pair<int, User*>(clientSock, newUser));

            std::cout << "connected : " << clientSock << std::endl;
            continue;
        }

        std::vector<struct pollfd>::iterator it = _pollFD.begin();
        ++it; // 서버소켓(client[0]) 건너뛰기
        while (it != _pollFD.end())
        {
            // 연결 종료
            if (it->revents & POLLHUP)
            {
                std::cout << "Client socket:" << it->fd << " shutdown." << std::endl;
                delete _users.at(it->fd);
                close(it->fd);
                _users.erase(it->fd);
                it = _pollFD.erase(it);
                continue;
            }

            // 소켓 읽기 및 command 실행
            if (it->revents & (POLLIN | POLLERR))
            {
                if (_users.at(it->fd)->readMessage(it->fd) <= 0)
                {
                    std::cout << "Client socket:" << it->fd << " can not read" << std::endl;
                    delete _users.at(it->fd);
                    close(it->fd);
                    _users.erase(it->fd);
                    it = _pollFD.erase(it);
                    continue;
                }
            }
            ++it;
        }
    }
}

std::map<int, User*>& Server::getUsers()
{
    return _users;
}

std::vector<Channel*>& Server::getChannels()
{
    return _channels;
}