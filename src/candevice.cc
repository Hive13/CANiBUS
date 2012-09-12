#include "serial.h"
#include "candevice.h"
#include "hacksession.h"

CanDevice::CanDevice(int id) : SessionObject(id, SessionObject::SCan)
{
	setProperty("name", "Unknown", this);
	setProperty("description", "No description available");
}

CanDevice::~CanDevice()
{

}

// Stub function
void CanDevice::init()
{

}

std::string CanDevice::port()
{
	return getStringProperty("port");
}

void CanDevice::addSession(HackSession *session)
{
	m_sessions.push_back(session);
	setProperty("sessions", m_sessions.size());
}

/*-------------------------------*/
CanbusSimulator::CanbusSimulator(int id) : CanDevice(id)
{
	setProperty("name", "CANSIM", this);
	setProperty("description", "CAN Simulation Module", this);
}

