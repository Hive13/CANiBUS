#include <stdio.h>
#include <stdarg.h>
#include <sstream>
#include <iostream>
#include <fstream>
using namespace std;

#include "logger.h"


CanibusLogger::CanibusLogger(const char *filename)
{
	m_logfile.open (filename);
	m_filename = std::string(filename);
	m_nextXmlId = 1;
	m_logcount = 0;
}

CanibusLogger::~CanibusLogger()
{
	m_logfile.close();
	if(m_logcount == 0)
		remove(m_filename.c_str());
}

void CanibusLogger::log(std::string msg)
{
	m_logfile << msg;
	m_logcount++;
}

void CanibusLogger::log(TiXmlDocument *doc)
{
	std::ostringstream oss;
	oss << m_filename << m_nextXmlId << ".xml";
	doc->SaveFile(oss.str());
	m_nextXmlId++;	
}

void CanibusLogger::log(const char *fmt,...)
{
        int n, size = 256;
        char *buf = new char[size];
        static std::string ioStr;
        va_list arg;

        buf[0] = 0;

        while (1)
        {
                va_start(arg, fmt);
                n = vsnprintf(buf, size, fmt, arg);
                va_end(arg);

                if (n > -1 && n < size)
                {
                        ioStr = buf;
                        delete[] buf;
                        m_logfile << ioStr;
			m_logcount++;
                        return;
                }

                if (n > -1)
                        size = n+1;
                else
                        size *= 2;

                delete[] buf;
                buf = new char[size];
        }
}
