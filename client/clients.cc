#include "clients.h"

CanibusClient::CanibusClient(int id)
{
	m_id = id;
}

CanibusClient::~CanibusClient()
{

}

void CanibusClient::setName(std::string newName)
{
	if(newName.size() > 0)
		m_name = newName;
}
