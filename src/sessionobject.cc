#include <stdio.h>
#include <cassert>

#include "sessionobject.h"
#include "sessionproperty.h"
#include "io.h"

SessionObject::SessionObject(int id, Type type, HackSession *hax)
{
	m_id = id;
	m_type = type;
	m_hacksession = hax;
}

SessionObject::~SessionObject()
{

}

int SessionObject::id()
{
	return m_id;
}

std::string SessionObject::name()
{
	return escapeXML(getStringProperty("name"));
}

const enum SessionObject::Type SessionObject::type()
{
	return m_type;
}

HackSession *SessionObject::hacksession()
{
	return m_hacksession;
}

void SessionObject::addToScope(SessionObject *object)
{
	m_scope.push_back(object);
}

bool SessionObject::hasInScope(SessionObject *object)
{
	if (object == this)
		return true;

	for(std::vector<SessionObject *>::iterator it = m_scope.begin(); it != m_scope.end() && (*it) ; ++it)
	{
		if ((*it) == object || (*it)->hasInScope(object))
			return true;
	}
	return false;
}

void SessionObject::removeFromScope(SessionObject *object)
{
	SessionObject *oTmp = 0;
	for(std::vector<SessionObject *>::iterator it = m_scope.begin() ; it != m_scope.end() && (oTmp = *it) ; ++it)
		if (oTmp == object)
		{
			m_scope.erase(it);
			return;
		}
}

std::string SessionObject::oldXMLUpdate(SessionObject *target, const bool &fullUpdate)
{
	std::string xmlString;
	bool emptyUpdate = true;

	SessionStringProperty *stringProperty = 0;
	for(std::vector<SessionStringProperty *>::iterator it = m_stringProperties.begin() ; it != m_stringProperties.end() && (stringProperty = *it); ++it)
		if( (fullUpdate || stringProperty->changed()) && (stringProperty->scope()->hasInScope(target)) )
		{
			if (emptyUpdate)
			{
				if (m_type == SClient)
					xmlString = "<clientupdate clientid=\"" + itoa(m_id) + "\"";
				else if (m_type == SHack)
					xmlString = "<hacksessionupdate sessionid=\"" + itoa(m_id) + "\"";
				else if (m_type == SCan)
					xmlString = "<candevice canid=\"" + itoa(m_id) + "\"";
				else
					assert(0);
				emptyUpdate = false;
			}
			xmlString += std::string(" ") + stringProperty->key() + "=\"" + escapeXML(stringProperty->value()) + "\"";
		}

	SessionIntProperty *intProperty = 0;
	for(std::vector<SessionIntProperty *>::iterator it = m_intProperties.begin() ; it != m_intProperties.end() && (intProperty = *it) ; ++it)
		if ( (fullUpdate || intProperty->changed()) && (intProperty->scope()->hasInScope(target)) )
		{
			if (emptyUpdate)
			{
				if (m_type == SClient)
					xmlString = "<clientupdate clientid=\"" + itoa(m_id) + "\"";
				else if (m_type == SHack)
					xmlString = "<hacksessionupdate sessionid=\"" + itoa(m_id) + "\"";
				else if (m_type == SCan)
					xmlString = "<candevice canid=\"" + itoa(m_id) + "\"";
				else
					assert(0);
				emptyUpdate = false;
			}
			xmlString += std::string(" ") + intProperty->key() + "=\"" + itoa(intProperty->value()) + "\"";
		}

	SessionBoolProperty *boolProperty = 0;
	for(std::vector<SessionBoolProperty *>::iterator it = m_boolProperties.begin() ; it != m_boolProperties.end() && (boolProperty = *it) ; ++it)
		if ( (fullUpdate || boolProperty->changed()) && (boolProperty->scope()->hasInScope(target)) )
		{
			if (emptyUpdate)
			{
				if (m_type == SClient)
					xmlString = "<clientupdate clientid=\"" + itoa(m_id) + "\"";
				else if (m_type == SHack)
					xmlString = "<hacksessionupdate sessionid=\"" + itoa(m_id) + "\"";
				else if (m_type == SCan)
					xmlString = "<candevice canid=\"" + itoa(m_id) + "\"";
				else
					assert(0);
				emptyUpdate = false;
			}
			xmlString += std::string(" ") + boolProperty->key() + "=\"" + itoa(boolProperty->value()) + "\"";
		}

	if (!emptyUpdate)
		xmlString += "/>";
	return xmlString;
}

void SessionObject::setBoolProperty(const std::string &key, const bool &value, SessionObject *scope)
{
	SessionBoolProperty *boolProperty = 0;
	for(std::vector<SessionBoolProperty *>::iterator it = m_boolProperties.begin() ; it != m_boolProperties.end() && (boolProperty = *it) ; ++it)
	{
		if (boolProperty->key() == key)
		{
			boolProperty->setValue(value);
			return;
		}
	}
	boolProperty = new SessionBoolProperty(key, value, scope ? scope : this);
	m_boolProperties.push_back(boolProperty);
}

void SessionObject::unsetPropertiesChanged()
{
	SessionStringProperty *stringProperty = 0;
	for(std::vector<SessionStringProperty *>::iterator it = m_stringProperties.begin() ; it != m_stringProperties.end() && (stringProperty = *it) ; ++it)
		if (stringProperty->changed())
			stringProperty->setChanged(false);
	SessionIntProperty *intProperty = 0;
	for(std::vector<SessionIntProperty *>::iterator it = m_intProperties.begin() ; it != m_intProperties.end() && (intProperty = *it); ++it)
		if (intProperty->changed())
			intProperty->setChanged(false);

	SessionBoolProperty *boolProperty = 0;
	for(std::vector<SessionBoolProperty *>::iterator it = m_boolProperties.begin() ; it != m_boolProperties.end() && (boolProperty = *it) ; ++it)
		if (boolProperty->changed())
			boolProperty->setChanged(false);
}

void SessionObject::setProperty(const std::string &key, const std::string &value, SessionObject *scope)
{
	SessionStringProperty *stringProperty = 0;
	for(std::vector<SessionStringProperty *>::iterator it = m_stringProperties.begin(); it != m_stringProperties.end() && (stringProperty = *it) ; ++it)
		if (stringProperty->key() == key)
		{
			stringProperty->setValue(value);
			return;
		}

	stringProperty = new SessionStringProperty(key, value, scope ? scope : this);
	m_stringProperties.push_back(stringProperty);
}

void SessionObject::setProperty(const std::string &key, int value, SessionObject *scope)
{
	SessionIntProperty *intProperty = 0;
	for(std::vector<SessionIntProperty *>::iterator it = m_intProperties.begin(); it != m_intProperties.end() && (intProperty = *it) ; ++it)
		if (intProperty->key() == key)
		{
			intProperty->setValue(value);
			return;
		}

	intProperty = new SessionIntProperty(key, value, scope ? scope : this);
	m_intProperties.push_back(intProperty);
}

int SessionObject::getIntProperty(const std::string &key)
{
	SessionIntProperty *intProperty = 0;
	for(std::vector<SessionIntProperty *>::iterator it = m_intProperties.begin() ; it != m_intProperties.end() && (intProperty = *it) ; ++it)
		if (intProperty->key() == key)
			return intProperty->value();
	return 0;
}

bool SessionObject::getBoolProperty(const std::string &key)
{
	SessionBoolProperty *boolProperty = 0;
	for(std::vector<SessionBoolProperty *>::iterator it = m_boolProperties.begin() ; it != m_boolProperties.end() && (boolProperty = *it) ; ++it)
		if (boolProperty->key() == key)
			return boolProperty->value();
	return 0;
}

std::string SessionObject::getStringProperty(const std::string &key)
{
	SessionStringProperty *stringProperty = 0;
	for(std::vector<SessionStringProperty *>::iterator it = m_stringProperties.begin(); it != m_stringProperties.end() && (stringProperty = *it) ; ++it)
		if (stringProperty->key() == key)
			return stringProperty->value();
	return std::string();
}

void SessionObject::removeProperty(const std::string &key)
{
	SessionStringProperty *stringProperty = 0;
	for(std::vector<SessionStringProperty *>::iterator it = m_stringProperties.begin(); it != m_stringProperties.end() && (stringProperty = *it) ; ++it)
		if (stringProperty->key() == key)
		{
			m_stringProperties.erase(it);
			delete stringProperty;
			// TODO Update client via XML
			return;
		}
}
