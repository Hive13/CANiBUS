#ifndef __SERIAL_H__
#define __SERIAL_H__

#include <string>
#include <queue>

class Serial
{
public:
	Serial();
	~Serial();
	void setPort(const char *port) { m_port = port; }
	void setBaud(unsigned int baurd) { m_baudrate = baurd; }
	const char *port() { return m_port; }
	int open();
	void close();
	void ioWrite(std::string);
	std::string readLine();
private:
	int m_baudrate;
	int m_fd;
	const char *m_port;
	std::string m_line;
	std::queue<std::string> m_lineQ;
};

#endif
