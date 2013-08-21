#include <stdio.h>
#include "elmcandevice.h"

ELMCanDevice::ELMCanDevice(int id) : CanDevice(id, CanDevice::ELM)
{
	setProperty("name", "ELM32x", this);
	setProperty("description", "ELM Based OBD Device", this);
	m_serial = 0;
};

void ELMCanDevice::init()
{
	int timeout_count;
	std::string response;
	if(!m_serial)
		return;

	// Disable echo
	m_serial->ioWrite("ATE0\r");
	timeout_count = 10000;
	while(--timeout_count > 0) {
		response = m_serial->readLine();
		if(response.compare(0,2, "OK") == 0)
			timeout_count = 0;
		usleep(500);
	}

	// Get version info
	m_serial->ioWrite("ATI\r");
	timeout_count = 1000;
	while(--timeout_count > 0) {
		response = m_serial->readLine();
		if(response.compare(0,4, ">ELM") == 0) {
			timeout_count = 0;
			setVersion(response.substr(1));
			setProperty("description", response.substr(1), this);
		}
		usleep(500);
	}
}

void ELMCanDevice::setSerial(Serial *serial)
{
	m_serial = serial;
	setProperty("port", m_serial->port(), this);
}

