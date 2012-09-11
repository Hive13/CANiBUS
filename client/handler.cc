#include <sstream>
#include "handler.h"
#include "socket.h"
#include "canibusmsg.h"
#include "state.h"
#include "screen.h"
#include "logger.h"
#include "clients.h"
#include "canbusdevice.h"

CanibusHandler::CanibusHandler(Socket *socket)
{
	m_socket = socket;
	m_xmlDoc = new TiXmlDocument();
	m_xmlRoot = 0;
	m_finishedInit = false;

	m_state = new CanibusState();
	logger = new CanibusLogger("handler_errlog.txt");
}

CanibusHandler::~CanibusHandler()
{
	delete logger;
	delete m_state;
}

void CanibusHandler::setScreen(Screen *screen)
{
	m_screen = screen;
	m_screen->setState(m_state);
}

void CanibusHandler::disconnect()
{
	m_socket->ioWrite(".X");
}

// Parses network packet into XML and ensures from CANiBUSd
bool CanibusHandler::validatePacket(const char *packet)
{
	m_xmlDoc = new TiXmlDocument();
	m_xmlDoc->Parse(packet);
	m_xmlRoot = 0;
	TiXmlHandle hDoc(m_xmlDoc);
	TiXmlElement *elem = hDoc.FirstChildElement().Element();
	if(elem == NULL) {
		fprintf(stderr, "No first element.  Probably malformed packet.\n");
		m_xmlDoc->Print();
		logger->log(m_xmlDoc);
		return false;

	}
	if(std::string(elem->Value()).compare("canibusd") != 0) {
		fprintf(stderr, "XML Packet not from CANiBUS Server\n");
		m_xmlDoc->Print();
		logger->log(m_xmlDoc);
		return false;
	}
	m_xmlRoot = new TiXmlHandle(elem);
	return true;
}

// Parses a packet for server ID info
// returns true if valid
bool CanibusHandler::parseServerId(const char *packet)
{
	const char *version = 0;
	if(!validatePacket(packet)) 
		return false;

	TiXmlElement *server = m_xmlRoot->FirstChild("server").Element();
	if(server)  {
	  version = server->Attribute("version");
	  if(!version) {
		fprintf(stderr, "No server version specified.  Invalid server\n");
		return false;
	  }
	} else {
	  fprintf(stderr, "Not a server identification packet\n");
	  m_xmlDoc->Print();
	  return false;
	}

	m_serverVersion = std::string(version);
	return true;
}

CanibusMsg *CanibusHandler::processMsg(const char *packet)
{
	CanibusMsg *msg = 0;
	if(!validatePacket(packet)) 
		return 0;

	TiXmlElement *emsg = m_xmlRoot->FirstChild("msg").Element();
	TiXmlElement *clientUpdate = m_xmlRoot->FirstChild("clientupdate").Element();
	TiXmlElement *client = m_xmlRoot->FirstChild("client").Element();
	TiXmlElement *deleteclient = m_xmlRoot->FirstChild("deleteclient").Element();
	TiXmlElement *finishedinit = m_xmlRoot->FirstChild("finishedinit").Element();
	TiXmlHandle updatehacksessionlist = m_xmlRoot->FirstChild("updatehacksessionlist");

	if(emsg) {
		const char *type = emsg->Attribute("type");
		const char *clientid = emsg->Attribute("clientid");
		const char *author = emsg->Attribute("author");
		const char *value = emsg->Attribute("value");
		msg = new CanibusMsg();
		msg->setType(type);
		msg->setClientId(atoi(clientid));
		msg->setAuthor(author);
		msg->setValue(value);
	} else if(client) { // This is always us
		const char *clientid = client->Attribute("clientid");
		const char *cookie = client->Attribute("cookie");
		CanibusClient *cInfo = new CanibusClient(atoi(clientid));
		if(cookie)
			cInfo->setCookie(cookie);
		cInfo->setName(m_state->nick());
		m_state->updateClient(cInfo);
	} else if(clientUpdate) {
		const char *clientid = clientUpdate->Attribute("clientid");
		const char *name = clientUpdate->Attribute("name");
		const char *host = clientUpdate->Attribute("host");
		const char *session = clientUpdate->Attribute("session");
		CanibusClient *cInfo = new CanibusClient(atoi(clientid));
		if(name)
			cInfo->setName(name);
		if(host)
			cInfo->setHost(host);
		if(session)
			cInfo->setSession(atoi(session));
		CanibusClient *oldUser = m_state->findClientById(cInfo->id());
		std::string tmpName;
		if(oldUser)
			tmpName = oldUser->name();
		int status = m_state->updateClient(cInfo);
		if(status == CLIENT_NICK_CHANGE && oldUser) {
			msg = new CanibusMsg();
			msg->setType("chat");
			msg->setValue(tmpName +" is now known as " + cInfo->name());
		} else if (status == CLIENT_ADDED && m_finishedInit) {
			msg = new CanibusMsg();
			msg->setType("chat");
			msg->setValue(cInfo->name() + " has joined.");
		}
	} else if (deleteclient) {
		const char *clientid = deleteclient->Attribute("clientid");
		CanibusClient *left = m_state->findClientById(atoi(clientid));
		if(left) {
			m_state->delClient(left);
			msg = new CanibusMsg();
			msg->setType("chat");
			msg->setValue(left->name() + " has disconnected");
		}
	} else if(updatehacksessionlist.ToElement()) {
		m_state->clearDevices();
		TiXmlElement *canbus = updatehacksessionlist.FirstChild("canbus").Element();
		for(canbus; canbus; canbus = canbus->NextSiblingElement()) {
			const char *id = canbus->Attribute("id");
			const char *type = canbus->Attribute("type");
			const char *desc = canbus->Attribute("description");
			CanbusDevice *canbus = new CanbusDevice(atoi(id));
			canbus->setType(type);
			canbus->setDesc(desc);
			m_state->updateCanbusDevice(canbus);
		}
		m_screen->updateLobby();
	} if (finishedinit) {
		m_finishedInit = true;
		msg = new CanibusMsg();
		msg->setType("chat");
		std::ostringstream oss;
		oss <<"There are " << m_state->clients().size() << " client(s) connected.";
		msg->setValue(oss.str());
	} else {
		logger->log(m_xmlDoc);
	}
	return msg;
}

CanibusMsg *CanibusHandler::poll()
{
	CanibusMsg *msg = 0;
	std::string packet = m_socket->poll();
	if(packet.size() > 0) 
		msg = processMsg(packet.c_str());
	return msg;
}

void CanibusHandler::nick(std::string username)
{
	// TODO remove whitespaces after first word
	m_state->setNick(username);
	m_socket->ioWrite(".n"+username);
}

void CanibusHandler::refresh()
{
	m_socket->ioWrite(".sr");
}

void CanibusHandler::setState(CanibusState *state)
{
	if(m_screen)
		m_screen->setState(state);
	m_state = state;
}

void CanibusHandler::sendChat(std::string msg)
{
	m_socket->ioWrite(msg);
}
