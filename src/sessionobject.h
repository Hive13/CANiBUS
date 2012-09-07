#ifndef __CANIBUSD_SESSIONOBJECT_H__
#define __CANIBUSD_SESSIONOBJECT_H__

#include <string>
#include <vector>

class SessionStringProperty;
class SessionIntProperty;
class SessionBoolProperty;
class SessionObject;
class HackSession;

class SessionObject
{
public:
	enum Type { Unknown = 0, SClient, SHack, SCan };
	SessionObject(int id, Type type = Unknown, HackSession *hax = 0);
	~SessionObject();

	HackSession *hacksession();

	void unsetPropertiesChanged();
	void setProperty(const std::string &key, const std::string &value, SessionObject *scope = 0);
	void setProperty(const std::string &key, int value, SessionObject *scope = 0);
	void setBoolProperty(const std::string &key, const bool &value, SessionObject *scope = 0);
	std::string getStringProperty(const std::string &key);
	int getIntProperty(const std::string &key);
	bool getBoolProperty(const std::string &key);
	void removeProperty(const std::string &key);

	std::string oldXMLUpdate(SessionObject *target, const bool &fullpdate = false);

	void addToScope(SessionObject *object);
	bool hasInScope(SessionObject *object);
	void removeFromScope(SessionObject *object);

	void setId(int id) { m_id = id; }
	int id();
	const enum Type type();
	std::string name();

protected:
	int m_id;
	Type m_type;
	HackSession *m_hacksession;

private:
	std::vector<SessionStringProperty *> m_stringProperties;
	std::vector<SessionIntProperty *> m_intProperties;
	std::vector<SessionBoolProperty *> m_boolProperties;
	std::vector<SessionObject *> m_scope;
};

#endif
