#include <iostream>

int main()
{
    std::cout << strlen(":irc.local NOTICE sdaf :*** Could not resolve your hostname: Request timed out; using your IP address (127.0.0.1) instead.:irc.local 001 sdaf :Welcome to the Localnet IRC Network sdaf!root@127.0.0.1:irc.local 002 sdaf :Your host is irc.local, running version InspIRCd-3:irc.local 003 sdaf :This server was created 03:23:51 May 17 2023:irc.local 004 sdaf irc.local InspIRCd-3 iosw biklmnopstv :bklov:irc.local 005 sdaf AWAYLEN=200 CASEMAPPING=rfc1459 CHANLIMIT=#:20 CHANMODES=b,k,l,imnpst CHANNELLEN=64 CHANTYPES=# ELIST=CMNTU HOSTLEN=64 KEYLEN=32 KICKLEN=255 LINELEN=512 MAXLIST=b:100 :are supported by this server:irc.local 005 sdaf MAXTARGETS=20 MODES=20 NAMELEN=128 NETWORK=Localnet NICKLEN=30 PREFIX=(ov)@+ SAFELIST STATUSMSG=@+ TOPICLEN=307 USERLEN=10 USERMODES=,,s,iow WHOX :are supported by this server:irc.local 251 sdaf :There are 0 users and 2 invisible on 1 servers:irc.local 253 sdaf 1 :unknown connections:irc.local 254 sdaf 1 :channels formed:irc.local 255 sdaf :I have 2 clients and 0 servers:irc.local 265 sdaf :Current local users: 2  Max: 2:irc.local 266 sdaf :Current global users: 2  Max: 2:irc.local 375 sdaf :irc.local message of the day:irc.local 372 sdaf : **************************************************:irc.local 372 sdaf : *             H    E    L    L    O              *:irc.local 372 sdaf : *  This is a private irc server. Please contact  *:irc.local 372 sdaf : *  the admin of the server for any questions or  *:irc.local 372 sdaf : *  issues.                                       *:irc.local 372 sdaf : **************************************************:irc.local 372 sdaf : *  The software was provided as a package of     *:irc.local 372 sdaf : *  Debian GNU/Linux <https://www.debian.org/>.   *:irc.local 372 sdaf : *  However, Debian has no control over this      *:irc.local 372 sdaf : *  server.                                       *:irc.local 372 sdaf : **************************************************:irc.local 372 sdaf : (The sysadmin possibly wants to edit </etc/inspircd/inspircd.motd>):irc.local 376 sdaf :End of message of the day.") << std::endl;
    return 0;
}