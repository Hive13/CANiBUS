#ifndef __CANIBUS_CLIENTS_H__
#define __CANIBUS_CLIENTS_H__

#include <string>

#define CLIENT_NO_CHANGE	0
#define CLIENT_NICK_CHANGE	1
#define CLIENT_ADDED		2

class CanibusClient
{
public:
	CanibusClient(int id);
	~CanibusClient();
	int id() { return m_id; }
	void setName(std::string name);
	std::string name() { return m_name; }
	void setHost(std::string host) { m_host = host; }
	std::string host() { return m_host; }
	void setSession(int session) { m_session = session; }
	int session() { return m_session; }
	void setCookie(std::string cookie) { m_cookie = cookie; }
	std::string cookie() { return m_cookie; }
private:
	int m_id;
	std::string m_name;
	std::string m_host;
	int m_session;
	std::string m_cookie;
};

#endif
