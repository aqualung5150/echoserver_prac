#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "User.hpp"

#include <string>
#include <map>
#include <vector>

class User;

class Channel
{
private:
    std::string _name;
    std::map<int, User*> _users; // Joined users
    std::vector<User*> _operators; // Operators of this channel

    std::string _topic; // Channel's topic

    // MODE
    bool _inviteOnly;       // default : false (MODE i)
    bool _restrictedTopic;  // defualt : true (MODE t)
    std::string _password;  // defualt : ""(false) - empty string (MODE k [password])
    
    /*
    MODE

    k : Set/remove the channel key (password).
    반드시 파라미터(패스워드)를 받아야 함.

    ! MODE #channel -k wrong-password 일 경우
    :irc.local 467 seunchoi #tradis :Channel key already set 라는 reply를 받음.

    -k 하려면 올바른 password를 파라미터로 받아야 함.

    -------------------------------------------------------------------------

    o : Give/take channel operator privilege.
    채널 오퍼레이터라면 누구든지 +/- o 할 수 있음

    => 유효하지 않은 메시지는 reply하지 않음
    e.g. operator가 아닌 user에게 mode -o를 하는 경우
    */ 
   
public:
    Channel(User *creater);
};

#endif