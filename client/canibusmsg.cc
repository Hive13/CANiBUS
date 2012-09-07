#include "canibusmsg.h"

CanibusMsg::CanibusMsg()
{
	m_type = "Unknown";
	m_author = "Unknown";
	m_clientId = 0;
	m_value = "";
}

CanibusMsg::~CanibusMsg()
{

}

void CanibusMsg::setType(const char *type)
{
	m_type = std::string(type);
}

void CanibusMsg::setClientId(unsigned int clientId)
{
	m_clientId = clientId;
}

void CanibusMsg::setAuthor(const char *author)
{
	m_author = std::string(author);
}

void CanibusMsg::setValue(const char *msg)
{
	m_value = std::string(msg);
}
