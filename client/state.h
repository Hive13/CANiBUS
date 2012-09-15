#ifndef __CANIBUS_STATE_H__
#define __CANIBUS_STATE_H__

#include <string>
#include <vector>

class CanibusClient;
class CanbusDevice;
class CanibusLogger;
class CanibusSession;

class CanibusState
{
public:
	CanibusState();
	~CanibusState();
	enum Status { Init, Lobby, Config, Run };
	void setStatus(Status status) { m_status = status; }
	Status status() { return m_status; }
	void setOurClient(CanibusClient *client) { m_me = client; }
	CanibusClient *me() { return m_me; }
	void setNick(std::string username);
	std::string nick() { return m_nick; }
	int updateClient(CanibusClient *cInfo);
	void delClient(CanibusClient *client);
	CanibusClient *findClientById(int id);
	int updateCanbusDevice(CanbusDevice *cbInfo);
	int updateSession(CanibusSession *sesionInfo);
	void delSession(int sessionId);
	CanbusDevice *findCanbusDeviceById(int id);
	CanibusSession *findSessionById(int id);
	void setActiveSession(CanibusSession *session) { m_activeSession = session; }
	CanibusSession *activeSession() { return m_activeSession; }
	void clearDevices() { m_canbusDevices.clear(); }
	std::vector<CanibusClient *>clients() { return m_clients; }
	std::vector<CanbusDevice *>devices() { return m_canbusDevices; }
	std::vector<CanibusSession *>sessions() { return m_sessions; }
private:
	Status m_status;
	CanibusClient *m_me;
	std::string m_nick;
	std::vector<CanibusClient *>m_clients;
	std::vector<CanbusDevice *>m_canbusDevices;
	std::vector<CanibusSession *>m_sessions;
	CanibusSession *m_activeSession;
	CanibusLogger *logger;
};

#endif
