#ifndef __CANIBUS_OPTIONS_H__
#define __CANIBUS_OPTIONS_H__

#include <string>

class CanibusOption
{
public:
	enum Type { Unknown = 0, Bool, Int, String };
	CanibusOption(int session_id);
	~CanibusOption();
	int sessionId() { return m_sessionId; }
	void setType(std::string type);
	void setType(Type type) { m_type = type; }
	Type type() { return m_type; }
	void setTitle(std::string title) { m_title = title; }
	std::string title() { return m_title; }
	void setCommand(std::string cmd) { m_cmd = cmd; }
	std::string command() { return m_cmd; }
	void setValue(std::string value) { m_value = value; }
	std::string value() { return m_value; }
	void setCanEdit(bool canEdit) { m_canEdit = canEdit; }
	bool canEdit() { return m_canEdit; }
private:
	int m_sessionId;
	Type m_type;
	std::string m_title;
	std::string m_cmd;
	std::string m_value;
	bool m_canEdit;
};
#endif
