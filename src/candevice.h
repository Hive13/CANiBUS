#ifndef __CANIBUSD_CANDEVICE_H__
#define __CANIBUSD_CANDEVICE_H__

#include "sessionobject.h"

#include "vector"

class HackSession;

class CanDevice : public SessionObject
{
public:
	CanDevice(int id);
	~CanDevice();
	void init();
	std::vector<HackSession *>sessions() { return m_sessions; }
	void addSession(HackSession *session);
	std::string port() { return m_port; }
private:
	std::vector<HackSession *>m_sessions;
	std::string m_port;
};

class CanbusSimulator : public CanDevice
{
public:
	CanbusSimulator(int id);
};

#endif
