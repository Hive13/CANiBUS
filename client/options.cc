#include "options.h"

CanibusOption::CanibusOption(int session_id)
{
	m_sessionId = session_id;
	m_canEdit = false;
}

CanibusOption::~CanibusOption()
{

}

void CanibusOption::setType(std::string type)
{
	if(type.compare("bool"))
		m_type = Bool;
	else if(type.compare("int"))
		m_type = Int;
	else if(type.compare("string"))
		m_type = String;
}
