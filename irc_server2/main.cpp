#include "Server.hpp"
#include "User.hpp"
#include "Channel.hpp"
#include "Command.hpp"

int main(int argc, char** argv)
{
    if (argc != 3)
    {
        std::cout << "usage: ./ft_irc <port> <password>" << std::endl;
        return 1;
    }

    std::string password = argv[2];
    int port = atoi(argv[1]); // 에러 처리 필요??

    Server server(password);
    server.startServer(port);

    return 0;
}