#include "EchoServer.hpp"

int main(int argc, char** argv)
{
    if (argc != 2)
        return 1;

    Server a;
    a.startServer(atoi(argv[1]));

    return 0;
}