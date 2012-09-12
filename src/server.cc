#include <stdio.h>
#include <stdarg.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <syslog.h>
#include <unistd.h>

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <libcapsinetwork/socket.h>

#include "io.h"
#include "hacksession.h"
#include "sessionobject.h"
#include "const.h"
#include "candevice.h"
#include "elmcandevice.h"
#include "client.h"
#include "server.h"
#include "serial.h"

using namespace std;

CanibusServer::CanibusServer() : SessionObject(0)
{
	m_port = 1234;
	m_nextClientId = 1;
	m_nextCanbusId = 1;
	m_nextHackSessionId = 1;

	initCanbusDevices();
}

void CanibusServer::setPort(int port)
{
	m_port = port;
}

CanibusServer::~CanibusServer()
{

}

bool CanibusServer::canInUse(std::string canPort)
{
	CanDevice *can;
	for(std::vector<CanDevice *>::iterator it = m_canbusDevices.begin() ; it != m_canbusDevices.end() && (can = *it) ; ++it) 
		if(can->port().compare(canPort) == 0)
			return true;
	return false;
}

void CanibusServer::initCanbusDevices()
{
	std::ostringstream oss;
	int devnum = 0;
	bool found, done = false;
	std::string response;
	int timeout_count;
	// If this is the first time then init the simulator
	if (m_canbusDevices.size() == 0)
	{
		CanbusSimulator *sim = new CanbusSimulator(m_nextCanbusId++);
		m_canbusDevices.push_back(sim);
	}
	// Serial devices should be linked to /dev/obdX
	while(!done) {
		oss << "/dev/obd" << devnum;
		std::ifstream serialDev(oss.str().c_str());
		if (serialDev.good()) {
			cerr << "Checking " << oss.str() << "...";
			if(canInUse(oss.str())) {
				cerr << "skipping" << endl;
				devnum++;
				continue;
			}
			found = false;
			// Check for ELM327
			Serial *serial = new Serial();
			serial->setPort(oss.str().c_str());
			serial->setBaud(38400);
			if(serial->open() > 1) {
				serial->ioWrite("ATZ\r");
				timeout_count = 1000;
				while(--timeout_count > 0) {
				  response = serial->readLine();
				  if(response.size() > 0) {
					if(response.compare(0, 3, "ELM") == 0) {
						cout << "found" << endl;
						found = true;
						ELMCanDevice *elm = new ELMCanDevice(m_nextCanbusId++);
						elm->setSerial(serial);
						elm->init();
						m_canbusDevices.push_back(elm);
						timeout_count = 0;
					}
				  }
				  usleep(5000);
				}
				devnum++;
			} else {
				done = true;
			}
			if (!found)
				cout << "not can" << endl;
		} else {
			done = true;
		}
	}
	// Scan USB devices (non-serial)
}

Client *CanibusServer::findClient(int clientId)
{
	Client *client = 0;
	for(std::vector<Client *>::iterator it = m_clients.begin(); it != m_clients.end() && (client = *it); ++it)
		if (client->id() == clientId)
			return client;
	return 0;
}

Client *CanibusServer::findClient(Socket *socket)
{
	Client *client = 0;
	for(std::vector<Client *>::iterator it = m_clients.begin(); it != m_clients.end() && (client = *it); ++it)
		if (client->socket() == socket)
			return client;
	return 0;
}

Client *CanibusServer::findClient(const std::string &name)
{
	Client *client = 0;
	for(std::vector<Client *>::iterator it = m_clients.begin(); it != m_clients.end() && (client = *it); ++it)
		if (client->getStringProperty("name") == name)
			return client;
	return 0;
}

// If a client disconnects
void CanibusServer::closedSocket(Socket *socket)
{
	Client *cInput = findClient(socket);
	if(!cInput)
	{
		delSocketTimeoutEvent( socket->fd() );
		return;
	}

	// TODO handle update

	cInput->setSocket( 0 );
}

void CanibusServer::delSocketTimeoutEvent(int socketFd)
{
  // TODO
}

int CanibusServer::processEvents()
{
  // TODO
}

void CanibusServer::initSocketTimeoutEvent(int socketFd)
{
	//TODO
}

void CanibusServer::exitClient(Client *client)
{
	Socket *socket = client->socket();
	socket->setStatus(Socket::Close);
	closedSocket(client->socket());
	delClient(client);
}

CanDevice *CanibusServer::findCanDeviceById(unsigned int id)
{
	CanDevice *can = 0;
	for(std::vector<CanDevice *>::iterator it = m_canbusDevices.begin(); it != m_canbusDevices.end() && (can = *it); ++it)
		if(can->id() == id)
			return can;
	return 0;
}

void CanibusServer::joinHackSession(Client *client, unsigned int canbusId)
{
	CanDevice *candevice = findCanDeviceById(canbusId);
	if(!candevice) {
		client->ioError("Invalid CAN device ID");
		return;
	}	
	// For now just allow one session per can device
	if(candevice->sessions().size() > 0) {
		HackSession *session = candevice->sessions().front();
		int maxClients = session->getIntProperty("maxclients");
		if(session->clients() >= maxClients)
		{
			client->ioError("This session is full (%d clients)", maxClients);
			return;
		}
		session->addClient(client);
		ioWrite(std::string("<canibusd><updatehacksessionlist type=\"edit\"><session id=\"") + itoa(session->id()) + "\" canbusid=\"" + itoa(session->candevice()->id()) + " clients=\"" + itoa(session->clients()) + "\" private=\"" + itoa(session->getBoolProperty("private")) + "\"/></updatehacksessionlist></canibusd>\n");
		return;
	}

	HackSession *session = new HackSession(m_nextHackSessionId++);
	// Set some default properties
	session->setProperty("master", -1, this);
	session->setProperty("description", "", this);
	session->setProperty("maxclients", 20, this);
	session->setProperty("private", false, this);

	session->setCanDevice(candevice);
	candevice->addSession(session);

	m_hacksessions.push_back(session);

	syslog( LOG_INFO, "new hack session: id=[%d] can=[%s] sessions=[%ld]", session->id(), candevice->getStringProperty("name").c_str(), m_hacksessions.size());

	session->addClient(client, true);

	ioWrite(std::string("<canibusd><updatehacksessionlist type=\"edit\"><session id=\"")+ itoa(session->id()) + "\" canbusid=\"" + itoa(session->candevice()->id()) + " clients=\"" + itoa(session->clients()) + "\" private=\"" + itoa(session->getBoolProperty("private")) + "\"/></updatehacksessionlist></canibusd>\n");
}	

Client *CanibusServer::newClient(Socket *socket, const std::string &name)
{
	delSocketTimeoutEvent( socket->fd() );

	Client *client = new Client(socket, m_nextClientId++);
	m_clients.push_back(client);
	addToScope(client);

	client->setProperty("session", -1, this);
	client->setProperty("host", socket->fqdn(), this);
	setClientName(client, name);

	client->sendClientMsg();
	sendHackSessionList(client);

	syslog( LOG_INFO, "new client: id=[%d], fd=[%d], name=[%s], clients=[%d]", client->id(), socket->fd(), name.c_str(), (int)m_clients.size() );
	client = 0;
	for(std::vector<Client *>::iterator it = m_clients.begin(); it != m_clients.end() && (client = *it) ; ++it)
		printf(" client %16s %16s session %d fd %d\n", client->name().c_str(), client->getStringProperty("host").c_str(), (client->hacksession() ? client->hacksession()->id() : -1), client->socket() ? (int)client->socket()->fd() : -1);

	return client;
}

void CanibusServer::sendHackSessionList(Client *cInput)
{
	cInput->ioWrite("<canibusd>");
	cInput->ioWrite("<updatehacksessionlist type=\"full\">");
	CanDevice *cTmp = 0;
	for(std::vector<CanDevice *>::iterator it = m_canbusDevices.begin(); it != m_canbusDevices.end() && (cTmp = *it) ; ++it)
		cInput->ioWrite("<canbus id=\"%d\" type=\"%s\" description=\"%s\"/>", cTmp->id(), cTmp->name().c_str(), cTmp->getStringProperty("description").c_str());

	HackSession *hsTmp = 0;
	for(std::vector<HackSession *>::iterator it = m_hacksessions.begin(); it!= m_hacksessions.end() && (hsTmp = *it) ; ++it)
		if (hsTmp->status() == HackSession::Config)
			cInput->ioWrite("<hacksession id=\"%ld\" clients=\"%d\" name=\"%s\" topic=\"%s\" private=\"%d\"/>", hsTmp->id(), hsTmp->clients(), hsTmp->name().c_str(), hsTmp->getStringProperty("topic").c_str(), hsTmp->getBoolProperty("private"));

	cInput->ioWrite("</updatehacksessionlist>");
	cInput->ioWrite("</canibusd>\n");
}

void CanibusServer::setClientName(Client *client, const std::string &name)
{
	std::string useName = ( name.size() ? name : "anonymous" );

	int i=1;
	while (findClient(useName))
		useName = ( name.size() ? name : "anonymous" ) + itoa( + i );

	client->setProperty("name", useName, this);

}

void CanibusServer::delClient(Client *client)
{
	ioWrite("<canibusd><deleteclient clientid=\"" + itoa(client->id()) + "\"/></canibusd>\n");
	for(std::vector<Client *>::iterator it = m_clients.begin(); it != m_clients.end() && (*it) ; ++it)
		if (*it == client)
		{
			removeFromScope(client);
			syslog( LOG_INFO, "del client: id=[%d], name=[%s], clients=[%ld]", client->id(), client->getStringProperty("name").c_str(), m_clients.size() - 1 );
			printf("delClient %d/%ld\n", client->id(), m_clients.size() - 1);
			delete client;
			m_clients.erase(it);
			client = 0;
			break;
		}
}

void CanibusServer::ioWrite(const char *fmt, ...)
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

void CanibusServer::ioWrite(const std::string &data, const bool &inLobby)
{
	Client *client = 0;
	for(std::vector<Client *>::iterator it = m_clients.begin(); it != m_clients.end() && (client = *it) ; ++it)
		if ( !(inLobby && client->hacksession()) )
			client->ioWrite(data);
}

// This packet is just a flag to the client that any new packets are updates
void CanibusServer::sendXMLFinishedInit(Client *client)
{
	client->ioWrite("<canibusd><finishedinit /></canibusd>\n");
}

void CanibusServer::sendXMLUpdates()
{
	// Send XML to all clients
	for(std::vector<Client *>::iterator it = m_clients.begin(); it != m_clients.end() && (*it) ; ++it)
		sendXMLUpdate((*it));
	// Reset propertiesChanged for all clients and hack sessions
	for(std::vector<Client *>::iterator it = m_clients.begin(); it != m_clients.end() && (*it) ; ++it)
		(*it)->unsetPropertiesChanged();
	for(std::vector<HackSession *>::iterator it = m_hacksessions.begin(); it != m_hacksessions.end() && (*it) ; ++it)
	{
		(*it)->unsetPropertiesChanged();
		// Hack sessions can reset children
		(*it)->unsetChildProperties();
	}
}

void CanibusServer::sendXMLUpdate(Client *cOutput, bool fullUpdate, bool excludeSelf)
{
	bool updateEmpty = true;

	// Send updates *about* all players ..
	Client *cUpdate = 0;
	for(std::vector<Client *>::iterator uit = m_clients.begin(); uit != m_clients.end() && (cUpdate = *uit) ; ++uit)
	{
		// .. but only when changed and in scope
		if (!excludeSelf || cUpdate != cOutput)
		{
			std::string updateXML = cUpdate->oldXMLUpdate(cOutput, fullUpdate);
			if (updateXML.size())
			{
				if (updateEmpty)
				{
					cOutput->ioWrite("<canibusd> ");
					updateEmpty = false;
				}
				cOutput->ioWrite("%s", updateXML.c_str());
			}
		}
	}

	// Send updates *about* all games ..
	HackSession *hUpdate = 0;
	for(std::vector<HackSession *>::iterator uit = m_hacksessions.begin(); uit != m_hacksessions.end() && (hUpdate = *uit) ; ++uit)
	{
		// .. but only when changed and in scope
		std::string updateXML = hUpdate->oldXMLUpdate(cOutput, fullUpdate);
		if(updateXML.size())
		{
			if (updateEmpty)
			{
				cOutput->ioWrite("<canibusd> ");
				updateEmpty =false;
			}
			cOutput->ioWrite("%s", updateXML.c_str());
		}
	}

	// let hack session handle updates about its objects
	if (HackSession *hax = cOutput->hacksession())
		updateEmpty = hax->sendChildXMLUpdate(cOutput, updateEmpty);

	if (!updateEmpty)
		cOutput->ioWrite("</canibusd>\n");
}

void CanibusServer::processInput(Socket *socket, const std::string data)
{
	Client *cInput = findClient(socket);
	if(!cInput)
	{
		if (data[0] == '.')
		{
			Client *cNew = 0;
			switch(data[1])
			{
			case 'n':
				cNew = newClient(socket, data.substr(2));
				sendXMLUpdates();
				sendXMLUpdate(cNew, true, true);
				sendXMLFinishedInit(cNew);
				return;
			case 'R':
				// TODO: Reconnect a client
				return;
			}
		}
		return;

	}

	if (data[0] == '.')
	{
		processCommands(cInput, data.substr(1));
		sendXMLUpdates();
		return;
	}

	if (data.size() > 256)
		cInput->ioError("Chat messages are limited to 256 characters");
	else
	{
		if (HackSession *hax = cInput->hacksession())
			hax->ioWrite("<canibusd><msg type=\"chat\" clientid=\"%d\" author=\"%s\" value=\"%s\"/></canibusd>\n", cInput->id(), cInput->name().c_str(), escapeXML(data).c_str());
		else
			ioWrite("<canibusd><msg type=\"chat\" clientid=\"" + itoa(cInput->id()) + "\" author=\"" + cInput->name() + "\" value=\"" + escapeXML(data) + "\"/></canibusd>\n", true);
	}
}

void CanibusServer::processCommands(Client *cInput, const std::string data2)
{
        char *data = (char *)data2.c_str();

	// The following commands are _always_ available.
	switch(data[0])
	{
	case 'n':
		setClientName(cInput, std::string(data+1));
		return;
	case 'X':
		exitClient(cInput);
		return;
	}

	// Commands while in the lobby
	HackSession *hax = cInput->hacksession();
	if(!hax)
	{
		switch(data[0])
		{
		case 's':  // Session related commands
			switch(data[1])
			{
				case 'l':
					sendHackSessionList(cInput);
					return;
				case 'r':
					initCanbusDevices();
					sendHackSessionList(cInput);
					return;
				case 'j':
					joinHackSession(cInput, atol(data2.substr(2).c_str()));
					return;
				default:
					cInput->ioNoSuchCmd("You are not within a HackSession");
			}
			break;
		default:
			cInput->ioNoSuchCmd("you are not within a HackSession");
		}
		// The rest of the commands are on in HackSessions
		return;
	}

	// This commands are always available in a HackSession
	// TODO:
}

