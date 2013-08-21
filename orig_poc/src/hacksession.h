#ifndef __CANIBUSD_HACKSESSION_H__
#define __CANIBUSD_HACKSESSION_H__

#include <vector>
#include <string>

#include "sessionobject.h"

class Client;
class CanDevice;
class CanPacket;

class HackSession : public SessionObject
{
public:
	HackSession(int id);
	virtual ~HackSession();

	enum Status { Config, Init, Run, End };
	void setStatus(Status status) { m_status = status; }
	Status status() { return m_status; }

	unsigned int clients();

        void ioWrite(const char *data, ...);
        void ioWrite(const std::string data);
        void ioInfo(const char *data, ...);
        void ioInfo(const std::string data);
        void ioError(const std::string data);
	void unsetChildProperties();
	bool sendChildXMLUpdate(Client *cOutput, bool emptyUpdate);
	void sendFullUpdate(Client *client, bool userRequest = false);
	const std::string statusLabel();
	void sendPackets(std::vector<CanPacket *>pkts);
	void sendStatus(Client *client);
	void sendConfiguration(Client *client);
	void sendClientList(Client *client);

	Client *addClient(Client *client, bool isMaster = false);
	void delClient(Client *client);
	void electNewMaster();
	void start(Client *client);
	void addMonitor(Client *client);
	void delMonitor(Client *client);

	void setCanDevice(CanDevice *can) { m_candevice = can; }
	CanDevice *candevice() { return m_candevice; }
	Client *master() { return m_master; }
	std::vector<Client *>monitors() { return m_monitoring; }
private:
	Status m_status;
	Client *m_master;
	CanDevice *m_candevice;
	std::vector<Client *> m_clients;
	std::vector<Client *> m_monitoring;
	std::vector<SessionObject *> m_configOptions;
};

#endif
