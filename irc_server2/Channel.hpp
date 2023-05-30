#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "User.hpp"

#include <string>
#include <map>
#include <vector>

#define MODE_I 0x0001
#define MODE_K 0x0002
#define MODE_L 0x0004
#define MODE_T 0x0008


class User;

class Channel
{
private:
    std::string _name;
    // std::map<int, User*> _users; // Joined users
    std::vector<User*> _users; // Joined users
    std::vector<User*> _operators; // Operators of this channel

    std::string _topic; // Channel's topic
    
    int _mode;
    std::string _key;
    size_t _limit;
    
    
    /*
    MODE

    k : Set/remove the channel key (password).
    반드시 파라미터(패스워드)를 받아야 함.

    ! MODE #channel -k wrong-password 일 경우
    :irc.local 467 seunchoi #tradis :Channel key already set 라는 reply를 받음.

    -k 하려면 올바른 password를 파라미터로 받아야 함.

    key를 변경하려면 -k 했다가 다시 +k 해줘야함

    JOIN 할 때
    JOIN #channel [wrong-password | no-parameter]일 때
    :irc.local 475 zzz_ #tradis :Cannot join channel (incorrect channel key)를 받음.

    -------------------------------------------------------------------------

    o : Give/take channel operator privilege.
    채널 오퍼레이터라면 누구든지 +/- o 할 수 있음

    => 유효하지 않은 메시지는 reply하지 않음
    e.g. operator가 아닌 user에게 mode -o를 하는 경우
    */ 
   
public:
    Channel();
    // Channel(std::string name, User *creater);

    void sendReply(std::string& reply, User* except = NULL);
    User* getUser(std::string& nick);
    bool isOperator(User* user);
    bool empty();

    //setters
    void setName(std::string name);
    void setKey(std::string key);

    void addUser(User* user);
    void addOperator(User* user);
    // void removeUser(std::string nick);
    void removeUser(User* user);
    // void removeOperator(std::string nick);
    void removeOperator(User* user);
    void addMode(int mode);
    void removeMode(int mode);

    //getters
    std::string getName() const;
    std::vector<User*> getUsers();
    std::vector<User*> getOperators();
    int getMode() const;
    std::string getKey() const;
    size_t getLimit() const;
};

#endif