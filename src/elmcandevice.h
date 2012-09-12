#ifndef __ELM_CANDEVICE_H__
#define __ELM_CANDEVICE_H__

#include "serial.h"
#include "candevice.h"

class Serial;

class ELMCanDevice : public CanDevice
{
public:
	ELMCanDevice(int id);
	void init();
	void setSerial(Serial *serial);
	Serial *serial() { return m_serial; }
	void setVersion(std::string version) { m_version = version; }
	std::string version() { return m_version; }
	std::string port();
private:
	Serial *m_serial;
	std::string m_version;
};

#endif
