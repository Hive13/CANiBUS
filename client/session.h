#ifndef __CANIBUS_SESSION_H__
#define __CANIBUS_SESSION_H__

#include <string>

class CanbusDevice;

class CanibusSession
{
public:
	CanibusSession(int id);
	~CanibusSession();
	int id() { return m_id; }
	void setStatus(std::string status) { m_status = status; }
	std::string status() { return m_status; }
	void setClientCount(int numClients) { m_numClients = numClients; }
	int clientCount() { return m_numClients; }
	void setPrivate(bool isPrivate) { m_isPrivate = isPrivate; }
	bool isPrivate() { return m_isPrivate; }
	void setCanbus(CanbusDevice *can) { m_canbusDevice = can; }
	CanbusDevice *canbusDevice() { return m_canbusDevice; }
	void setDesc(std::string desc) { m_desc = desc; }
	std::string desc() { return m_desc; }
	void setMasterId(int id);
	int masterId() { return m_masterId; }
	void setMaxClients(int max) { m_maxClients = max; }
	int maxClients() { return m_maxClients; }
private:
	int m_id;
	std::string m_status;
	int m_numClients;
	bool m_isPrivate;
	CanbusDevice *m_canbusDevice;
	std::string m_desc;
	int m_masterId;
	int m_maxClients;
};

#endif
