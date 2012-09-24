#ifndef __CANIBUS_SOCKET_H__
#define __CANIBUS_SOCKET_H__

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>

#include <string>
#include <queue>

class CanibusLogger;
class CanibusHandler;

class Socket
{
public:
	Socket(const char *hostname = "localhost", int port = 1234);
	~Socket();
	void setServer(const char *hostname);
	void setPort(int port);
	bool connect();
	void close();
	void ioWrite(std::string msg);
	std::string poll();
	bool isConnected() { return m_connected; }
	void enablePolling();
	std::string serverVersion() { return m_serverVersion; }
	void flushQ();
private:
	bool m_connected;
	int m_sockfd, m_port;
	struct hostent *m_server;
	CanibusLogger *logger;
	std::string m_serverVersion;
	std::string m_packet;
	std::queue<std::string> m_packetQ;
};

#endif
