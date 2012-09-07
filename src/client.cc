#include <string>

#include <libcapsinetwork/socket.h>

#include "io.h"
#include "hacksession.h"
#include "client.h"

Client::Client(Socket *socket, int id) : SessionObject(id, SessionObject::SClient)
{
	m_socket = socket;
}

Client::~Client()
{

}

void Client::ioWrite(const char *fmt, ...)
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
                        ioWrite(ioStr);
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

void Client::ioWrite(std::string data)
{
        if (m_socket)
                m_socket->ioWrite(data);
}

void Client::ioInfo(const char *data, ...)
{
        va_list arg;
        char buf[2048];

        va_start(arg, data);
        vsnprintf(buf, sizeof(buf)-1, data, arg);
        va_end(arg);
        buf[sizeof(buf)-1] = 0;

        ioWrite("<canibusd><msg type=\"info\" value=\"%s\"/></canibusd>\n", buf);
}

void Client::ioInfo(const std::string data)
{
        ioWrite("<canibusd><msg type=\"info\" value=\"" + data + "\"/></canibusd>\n");
}

void Client::ioError(const char *data, ...)
{
        va_list arg;
        char buf[2048];

        va_start(arg, data);
        vsnprintf(buf, sizeof(buf)-1, data, arg);
        va_end(arg);
        buf[sizeof(buf)-1] = 0;

        ioWrite("<canibusd><msg type=\"error\" value=\"%s\"/></canibusd>\n", buf);
}

void Client::ioError(const std::string data)
{
        ioWrite("<canibusd><msg type=\"error\" value=\"" + data + "\"/></canibusd>\n");
}

void Client::ioNoSuchCmd(const std::string data)
{
	if (data.size())
		ioError("Command is unavailable in current state: " + data);
	else
		ioError("No such command.");
}

void Client::closeSocket()
{
	m_socket->setStatus(Socket::Close);
}

void Client::setSocket(Socket *socket)
{
	m_socket = socket;
	if(m_socket)
		setProperty("host", socket->fqdn() );
	else
		setProperty("host", "" );
}

void Client::setHackSession(HackSession *hax)
{
	if (m_hacksession != hax)
	{
		m_hacksession = hax;
		setProperty("hacksession", (m_hacksession ? m_hacksession->id() : -1));
	}
}

void Client::sendClientMsg()
{
	ioWrite(std::string("<canibusd><client clientid=\""+ itoa(m_id) + "\" cookie=\"" + getStringProperty("cookie").c_str() + "\"/></canibusd>\n"));
}

