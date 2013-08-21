#ifndef __CANIBUS_LOGGER_H__
#define __CANIBUS_LOGGER_H__

#include <tinyxml.h>

#include <iostream>
#include <fstream>
using namespace std;

class CanibusLogger
{
public:
	CanibusLogger(const char *filename);
	~CanibusLogger();
	void log(const char *fmt, ...);
	void log(std::string msg);
	void log(TiXmlDocument *doc);
	unsigned int nexXmlId() { return m_nextXmlId; }
	unsigned int logCount() { return m_logcount; }
private:
	unsigned int m_nextXmlId;
	unsigned int m_logcount;
	std::string m_filename;
	ofstream m_logfile;
};

#endif
