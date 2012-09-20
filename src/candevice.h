#ifndef __CANIBUSD_CANDEVICE_H__
#define __CANIBUSD_CANDEVICE_H__

#include "sessionobject.h"

#include "vector"
#include "map"

#define B1_CHANGE (1 << 0)
#define B2_CHANGE (2 << 0)
#define B3_CHANGE (3 << 0)
#define B4_CHANGE (4 << 0)
#define B5_CHANGE (5 << 0)
#define B6_CHANGE (6 << 0)
#define B7_CHANGE (7 << 0)
#define B8_CHANGE (8 << 0)

class HackSession;
class CanPacket;

class CanDevice : public SessionObject
{
public:
	enum Model {Unknown = 0, SIM, ELM };
	CanDevice(int id, Model model = Unknown);
	~CanDevice();
	void init();
	std::vector<HackSession *>sessions() { return m_sessions; }
	void addSession(HackSession *session);
	void delSession(HackSession *session);
	virtual void prepareMonitor();
	virtual void enableMonitor();
	virtual void disableMonitor();
	std::string port();
	bool isMonitoring();
	virtual std::vector<CanPacket *>poll() {return m_packets;}
	const enum Model model();
	int getPacketChanges(CanPacket *pkt);
private:
	Model m_model;
	std::vector<HackSession *>m_sessions;
	std::vector<CanPacket *>m_packets;
	std::map<int, CanPacket *> m_packetGroup;
};

class CanbusSimulator : public CanDevice
{
public:
	CanbusSimulator(int id);
	void enableMonitor();
	std::vector<CanPacket *>poll();
private:
	long m_internal_seq;
	bool m_clockSynced;
	struct timespec m_lastTime;
	char hexToChar(std::string byte);
	std::vector<CanPacket *>::iterator m_it;
	std::vector<CanPacket *>m_packets;
};

#endif
