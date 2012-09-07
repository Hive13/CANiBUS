#ifndef __CANBUSDEVICE_H__
#define __CANBUSDEVICE_H__

#include <string>

class CanbusDevice
{
public:
	CanbusDevice(int id);
	~CanbusDevice();
	int id() { return m_id; }
	void setType(std::string type) { m_type = type; }
	std::string type() { return m_type; }
	void setDesc(std::string desc) { m_desc = desc; }
	std::string desc() { return m_desc; }
private:
	int m_id;
	std::string m_type;
	std::string m_desc;
};

#endif
