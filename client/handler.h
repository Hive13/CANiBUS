#ifndef __CANIBUS_HANDLER_H__
#define __CANIBUS_HANDLER_H__

#include <string>
#include <tinyxml.h>

class Socket;
class Screen;
class CanibusMsg;
class CanibusState;
class CanibusOption;
class CanibusLogger;

class CanibusHandler
{
public:
	CanibusHandler(Socket *socket);
	~CanibusHandler();
	bool validatePacket(const char *packet);
	bool parseServerId(const char *packet);
	CanibusMsg *processMsg(const char *packet);
	CanibusMsg *poll();
	std::string serverVersion() { return m_serverVersion; }
	void nick(std::string username);
	void join(std::string canbusid);
	void setState(CanibusState *state);
	CanibusState *state() { return m_state; }
	void setScreen(Screen *screen);
	Screen *screen() { return m_screen; }
	void sendChat(std::string msg);
	CanibusMsg *systemMsg(std::string msg);
	void start();
	void startMonitor();
	void stopMonitor();
	void leave();
	void disconnect();
	void refresh();
	int status();
private:
	bool m_finishedInit;
	Socket *m_socket;
	Screen *m_screen;
	CanibusState *m_state;
	CanibusLogger *logger;
	std::string m_serverVersion;
	TiXmlDocument *m_xmlDoc;
	TiXmlHandle *m_xmlRoot;
};

#endif
