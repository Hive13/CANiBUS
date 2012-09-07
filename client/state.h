#ifndef __CANIBUS_STATE_H__
#define __CANIBUS_STATE_H__

#include <string>
#include <vector>

class CanibusClient;
class CanbusDevice;
class CanibusLogger;

class CanibusState
{
public:
	CanibusState();
	~CanibusState();
	void setNick(std::string username);
	std::string nick() { return m_nick; }
	int updateClient(CanibusClient *cInfo);
	void delClient(CanibusClient *client);
	CanibusClient *findClientById(int id);
	int updateCanbusDevice(CanbusDevice *cbInfo);
	CanbusDevice *findCanbusDeviceById(int id);
	void clearDevices() { m_canbusDevices.clear(); }
	std::vector<CanibusClient *>clients() { return m_clients; }
	std::vector<CanbusDevice *>devices() { return m_canbusDevices; }
private:
	std::string m_nick;
	std::vector<CanibusClient *>m_clients;
	std::vector<CanbusDevice *>m_canbusDevices;
	CanibusLogger *logger;
};

#endif
