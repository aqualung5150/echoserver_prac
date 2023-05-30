#ifndef REPLY_HPP
#define REPLY_HPP

//user connect
#define RPL_WELCOME(server, nick, name, ip)			":" + server + " 001 " + nick + " :Welcome to the Internet Relay Chat " + nick + "!" + name + "@" + ip + "\r\n"
#define RPL_YOURHOST(server, nick)					":" + server + " 002 " + nick + " :Your host is " + server + ", running version 1.0\r\n"
#define RPL_CREATED(server, nick)					":" + server + " 003 " + nick + " :This server was created on MacOS\r\n"
#define RPL_MYINFO(server, nick, usermode, channelmode) ":" + server + " 004 " + nick + " " + server + " version 1.0 " + usermode + " " + channelmode + "\r\n"
//unknown command
#define ERR_UNKNOWNCOMMAND(server, nick, command)	":" + server + " 421 " + nick + " " + command + " :Unknown command\r\n"
//need more parameter
#define ERR_NEEDMOREPARAMS(server, nick, command)         ":" + server + " 461 " + nick + " " + command + " :Not enough parameters.\r\n"
//already registered
#define ERR_ALREADYREGISTRED(server, nick)          ":" + server + " 462 " + nick + " :You may not reregister\r\n"
//not registered
#define ERR_NOTREGISTERED(server, nick, command)    ":" + server + " 451 " + nick + " " + command + " :You have not registered.\r\n"
//msg
#define RPL_PRIVMSG(nick, name, ip, receiver, message)	":" + nick + "!" + name + "@" + ip + " PRIVMSG " + receiver + " :" + message + "\r\n"
//notice
#define RPL_NOTICE(nick, name, ip, receiver, message)	":" + nick + "!" + name + "@" + ip + " NOTICE " + receiver + " :" + message + "\r\n"
//quit, exit
#define RPL_QUIT(nick, name, ip, message)				":" + nick + "!" + name + "@" + ip + " Quit :" + message + "\r\n"
// #define RPL_QUIT(nick, name, ip, message)				":" + nick + "!" + name + "@" + ip + " Quit :Quit: " + message + "\r\n"
#define RPL_ERROR(error, name, ip, message)             "ERROR :" + error + ": (" + name + "@" + ip + ") [" + message + "]\r\n"
//part
// #define RPL_PART(nick, name, ip, channel)				":" + nick + "!" + name + "@" + ip + " PART " + channel + "\r\n"
#define RPL_PARTWITHMSG(nick, name, ip, channel, message)	":" + nick + "!" + name + "@" + ip + " PART " + channel + " :" + message + "\r\n"
//pong
#define RPL_PONG(server, ip)						":" + server + " PONG " + server + ":" + server + "\r\n"
//password fail
#define ERR_PASSWDMISMATCH(ip, user)					":" + ip + " 464 " + user + " :Password incorrect.\r\n"
//join
#define RPL_JOIN(nick, name, ip, channel)				":" + nick + "!" + name + "@" + ip + " JOIN :" + channel + "\r\n"
#define RPL_NAMEREPLY(server, nick, channel, users)	":" + server + " 353 " + nick + " = " + channel + " :" + users + "\r\n"
#define RPL_ENDOFNAMES(server, nick, channel)		":" + server + " 366 " + nick + " " + channel + " :End of /NAMES list.\r\n"
//#define ERR_BANNEDFROMCHAN							": 474"
//#define ERR_BADCHANNELKEY
#define ERR_BADCHANMASK(server, nick, channel)      ":" + server + " 476 " + nick + " " + channel + " :Invalid channel name\r\n"
//invite
#define RPL_INVITING(nick, server, target, channel)	":" + server + " 341 " + nick + " " + target + " " + channel + "\r\n"
#define RPL_INVITED(nick, name, ip, target, channel)	":" + nick + "!" + name + "@" + ip + " INVITE " + target + " :" + channel + "\r\n"
//nick
#define RPL_NICK(nick, name, ip , newnick)				":" + nick + "!" + name + "@" + ip + " NICK :" + newnick + "\r\n"
#define ERR_NICKNAMEINUSE(server, nick, newnick)	":" + server + " 433 " + nick + " " + newnick + " :Nickname is already in use." + "\r\n"
//#define ERR_ERRONEUSNICKNAME(server, nick, newnick)
//kick
#define RPL_KICK(nick, name, ip, channel, target, message)		":" + nick + "!" + name + "@" + ip + " KICK " + channel + " " + target + " :" + message + "\r\n"
#define ERR_CHANOPRIVSNEEDED(server, nick, channel)	":" + server + " 482 " + nick + " " + channel + " :You must be a channel half-operator\r\n"
#define ERR_NOSUCHNICK(server, nick, target)		":" + server + " 401 " + nick + " " + target + " :No such nick/channel\r\n"
//kill
#define RPL_KILL(nick, name, ip, target, message)		":" + nick + "!" + name + "@" + ip + " KILL " + target + " " + message + "\r\n"
#define ERR_NOPRIVILEGES(server, name)				":" + server + " 481 " + name + " :Permission Denied - You do not have the required operator privileges\r\n"
//op
#define RPL_OP(nick, name, ip, channel, target)			":" + nick + "!" + name + "@" + ip + " MODE " + channel + " +o " + target + "\r\n"
//deop
#define RPL_DEOP(nick, name, ip, channel, target)		":" + nick + "!" + name + "@" + ip + " MODE " + channel + " -o " + target + "\r\n"
//mode_channel
#define RPL_CHANNELMODEMSG(nick, name, ip, channel, mode)	": :" + nick + "!" + name + "@" + ip + " MODE " + channel + " " + mode + "\r\n"
#define RPL_CHANNELMODEMSGPARAM(nick, name, ip, channel, mode, param)	": :" + nick + "!" + name + "@" + ip + " MODE " + channel + " " + mode + " " + param "\r\n"
#define ERR_UNKNOWNMODE(server, nick, mode)         ":" + server + " 472 " + nick + " " + mode + " :is not a recognised channel mode.\r\n"

//channel
#define ERR_NOSUCHCHANNEL(server, nick, channel)    ":" + server + " 403 " + nick + " " + channel + " :No such channel\r\n"
#define ERR_CANNOTSENDTOCHAN(server, nick, channel) ":" + server + " 404 " + nick + " " + channel + " :You cannot send external messages to this channel\r\n"
#define ERR_NOTONCHANNEL(server, nick, channel)     ":" + server + " 442 " + nick + " " + channel + " :You're not on that channel\r\n"
#define ERR_INVITEONLYCHAN(server, nick, channel)   ":" + server + " 473 " + nick + " " + channel + " :Cannot join channel (invite only)\r\n"
#define ERR_BADCHANNELKEY(server, nick, channel)    ":" + server + " 475 " + nick + " " + channel + " :Cannot join channel (incorrect channel key)\r\n"
#define ERR_CHANNELISFULL(server, nick, channel)    ":" + server + " 475 " + nick + " " + channel + " :Cannot join channel (channel is full)\r\n"

#endif
