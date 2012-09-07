#ifndef __CANIBUS_SESSIONPROPERTY_H__
#define __CANIBUS_SESSIONPROPERTY_H__

class SessionObject;

class SessionProperty
{
public:
	SessionProperty(const std::string &key, SessionObject *m_scope);
	std::string key() const;
	SessionObject *scope();
	void setChanged(const bool &changed);
	bool changed() const;

protected:
	std::string m_key;
	SessionObject *m_scope;
	bool m_changed;
};

class SessionIntProperty : public SessionProperty
{
public:
	SessionIntProperty(const std::string &key, int value, SessionObject *scope);
	bool setValue(int value);
	int value();
private:
	int m_value;
};

class SessionStringProperty : public SessionProperty
{
public:
	SessionStringProperty(const std::string &key, const std::string &value, SessionObject *scope);
	bool setValue(const std::string &value);
	std::string value() const;
private:
	std::string m_value;
};

class SessionBoolProperty : public SessionProperty
{
public:
	SessionBoolProperty(const std::string &key, const bool &value, SessionObject *scope);
	bool setValue(const bool &value);
	bool value() const;
private:
	bool m_value;
};

#endif
