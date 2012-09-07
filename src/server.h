#ifndef __CANIBUSD_SERVER_H__
#define __CANIBUSD_SERVER_H__

#include "sessionobject.h"

class HackSession;
class Client;
class CanDevice;
class Socket;

class CanibusServer : public SessionObject
{
public:
	CanibusServer();
	~CanibusServer();

	void setPort(int port);
	int port() { return m_port; }

	Client *findClient(int clientId);
	Client *findClient(Socket *socket);
	Client *findClient(const std::string &name);
	Client *newClient(Socket *socket, const std::string &name);
	void delClient(Client *client);
	void setClientName(Client *client, const std::string &name);
	void exitClient(Client *client);

	void sendHackSessionList(Client *client);
	void joinHackSession(Client *client, unsigned int canbusId);

	void initCanbusDevices();
	CanDevice *findCanDeviceById(unsigned int id);

	void sendXMLUpdates();
	void sendXMLUpdate(Client *cOutput, bool fullUpdate = false, bool excludeSelf = false);
	void sendXMLFinishedInit(Client *client);
	void ioWrite(const char *fmt, ...);
	void ioWrite(const std::string &data, const bool &inLobby = false);

	void delSocketTimeoutEvent(int socketFd);
	void closedSocket(Socket *socket);
	void initSocketTimeoutEvent(int socketFd);

	int processEvents();
	void processInput(Socket *Socket, const std::string data);
	void processCommands(Client *cInput, const std::string data);

private:
	int m_port;

	std::vector<Client *> m_clients;
	std::vector<HackSession *> m_hacksessions;
	std::vector<CanDevice *> m_canbusDevices;

	unsigned int m_nextClientId, m_nextCanbusId, m_nextHackSessionId;
};

#endif
