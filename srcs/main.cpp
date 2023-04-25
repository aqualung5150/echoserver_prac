#include "EchoServer_copy.hpp"

int main(int argc, char** argv)
{
    if (argc != 2)
        return 1;

    EchoServer a;
    a.serverStart(atoi(argv[1]));

    return 0;
}