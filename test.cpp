#include <iostream>

int main()
{
    std::string s("this is string.");
    std::string sub;

    sub = s.substr(s.find('s'));

    std::cout << sub << std::endl;
    return 0;
}