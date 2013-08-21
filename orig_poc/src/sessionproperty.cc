#include "sessionobject.h"
#include "sessionproperty.h"

SessionProperty::SessionProperty(const std::string &key, SessionObject *scope)
{
	m_key = key;
	m_scope = scope;
	m_changed = true;
}

std::string SessionProperty::key() const
{
	return m_key;
}

SessionObject *SessionProperty::scope()
{
	return m_scope;
}

void SessionProperty::setChanged(const bool &changed)
{
	m_changed = changed;
}

bool SessionProperty::changed() const
{
	return m_changed;
}

SessionIntProperty::SessionIntProperty(const std::string &key, int value, SessionObject *scope) : SessionProperty(key, scope)
{
	m_value = value;
}

bool SessionIntProperty::setValue(int value)
{
	if (m_value != value)
	{
		m_value = value;
		m_changed = true;
	}
	return m_changed;
}

int SessionIntProperty::value()
{
	return m_value;
}

SessionStringProperty::SessionStringProperty(const std::string &key, const std::string &value, SessionObject *scope) : SessionProperty(key, scope)
{
	m_value = value;
}

bool SessionStringProperty::setValue(const std::string &value)
{
	if (m_value != value)
	{
		m_value = value;
		m_changed = true;
	}
	return m_changed;
}

std::string SessionStringProperty::value() const
{
	return m_value;
}

SessionBoolProperty::SessionBoolProperty(const std::string &key, const bool &value, SessionObject *scope) : SessionProperty(key, scope)
{
	m_value = value;
}

bool SessionBoolProperty::setValue(const bool &value)
{
	if (m_value != value)
	{
		m_value = value;
		m_changed = true;
	}
	return m_changed;
}

bool SessionBoolProperty::value() const
{
	return m_value;
}

