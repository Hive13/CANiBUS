#ifndef __CANIBUSMSG_H__
#define __CANIBUSMSG_H__

#include <string>

class CanibusMsg
{
public:
	CanibusMsg();
	~CanibusMsg();

	void setType(const char *type);
	std::string type() { return m_type; }
	void setClientId(unsigned int clientId);
	unsigned int id() { return m_clientId; }
	void setAuthor(const char *author);
	std::string author() { return m_author; }
	void setValue(const char *msg);
	void setValue(std::string msg) { m_value = msg; }
	std::string value() { return m_value; }
private:
	std::string m_type;
	unsigned int m_clientId;
	std::string m_author;
	std::string m_value;
};

#endif
