#ifndef REPLY_HPP
#define REPLY_HPP

//user connect
#define RPL_WELCOME(servername, nick, name, ip)			":" + servername + " 001 " + nick + " :Welcome to the Internet Relay Chat " + nick + "!" + name + "@" + ip + "\r\n"
#define RPL_YOURHOST(servername, nick)					":" + servername + " 002 " + nick + " :Your host is " + servername + ", running version 1.0\r\n"
#define RPL_CREATED(servername, nick)					":" + servername + " 003 " + nick + " :This server was created on MacOS\r\n"
// #define RPL_MYINFO(nick)	":IRC 004" + nick + ":<servername> <version> <available user modes> <available channel modes>"
//unknown command
#define ERR_UNKNOWNCOMMAND(servername, nick, command)	":" + servername + " 421 " + nick + " " + command + ":Unknown command\r\n"
//msg
#define RPL_PRIVMSG(nick, name, ip, receiver, message)	":" + nick + "!" + name + "@" + ip + " PRIVMSG " + receiver + " :" + message + "\r\n"
//notice
#define RPL_NOTICE(nick, name, ip, receiver, message)	":" + nick + "!" + name + "@" + ip + " NOTICE " + receiver + " :" + message + "\r\n"
//quit, exit
#define RPL_QUIT(nick, name, ip, message)				":" + nick + "!" + name + "@" + ip + " Quit :Quit: " + message + "\r\n"
#define RPL_ERROR(error ,name, ip, message)				":ERROR :" + error + ":" + " (" name + "@" + ip + ") " + "[" + message + "]" + "\r\n"
//part
#define RPL_PART(nick, name, ip, channel)				":" + nick + "!" + name + "@" + ip + " PART " + channel + "\r\n"
#define RPL_PARTWITHMSG(nick, name, ip, channel, message)	":" + nick + "!" + name + "@" + ip + " PART " + channel + " :" + message + "\r\n"
//pong
#define RPL_PONG(servername, ip)						":" + servername + " PONG " + servername + ":" + servername + "\r\n"
//password fail
#define ERR_PASSWDMISMATCH(ip, user)					":" + ip + " 464 " + user + " :Password incorrect.\r\n"
//join
#define RPL_JOIN(nick, name, ip, channel)				":" + nick + "!" + name + "@" + ip + " JOIN :" + channel + "\r\n"
#define RPL_NAMEREPLY(nick, servername, channel, users)	":" + servername + " 353 " + nick + " = " + channel + " :" + users + "\r\n"
#define RPL_ENDOFNAMES(nick, servername, channel)		":" + servername + " 366 " + nick + " " + channel + " :End of /NAMES list.\r\n"
//#define ERR_BANNEDFROMCHAN							": 474"
//#define ERR_BADCHANNELKEY
//invite
#define RPL_INVITING(nick, servername, target, channel)	":" + servername + " 341 " + nick + " " + target + " " + channel + "\r\n"
#define RPL_INVITED(nick, name, ip, target, channel)	":" + nick + "!" + name + "@" + ip + " INVITE " + target + " :" + channel + "\r\n"
//nick
#define RPL_NICK(nick, name, ip , newnick)				":" + nick + "!" + name + "@" + ip + " NICK " + newnick + "\r\n"
#define ERR_NICKNAMEINUSE(servername, nick, newnick)	":" + servername + " 433 " + nick + " " + newnick + " :Nickname is already in use." + "\r\n"
//#define ERR_ERRONEUSNICKNAME(servername, nick, newnick)
//kick
#define RPL_KICK(nick, name, ip, channel, target, message)		":" + nick + "!" + name + "@" + ip + " KICK " + channel + " " + target + " :" + message + "\r\n"
#define ERR_CHANOPRIVSNEEDED(servername, nick, channel)	":" + servername + " 482 " + nick + " " + channel + " ::You must be a channel half-operator\r\n"
#define ERR_NOSUCHNICK(servername, nick, target)		":" + servername + " 401 " + nick + " " + target + " :No such nick/channel\r\n"
//kill
#define RPL_KILL(nick, name, ip, target, message)		":" + nick + "!" + name + "@" + ip + " KILL " + target + " " + message + "\r\n"
#define ERR_NOPRIVILEGES(servername, name)				":" + servername + " 481 " + name + " :Permission Denied - You do not have the required operator privileges\r\n"
//op
#define RPL_OP(nick, name, ip, channel, target)			":" + nick + "!" + name + "@" + ip + " MODE " + channel + " +o " + target + "\r\n"
//deop
#define RPL_DEOP(nick, name, ip, channel, target)		":" + nick + "!" + name + "@" + ip + " MODE " + channel + " -o " + target + "\r\n"
//mode_channel
#define RPL_CHANNELMODEMSG(nick, name, ip, channel, mode)	": :" + nick + "!" + name + "@" + ip + " MODE " + channel + " " + mode + "\r\n"
#define RPL_CHANNELMODEMSGPARAM(nick, name, ip, channel, mode, param)	": :" + nick + "!" + name + "@" + ip + " MODE " + channel + " " + mode + " " + param "\r\n"

#endif
