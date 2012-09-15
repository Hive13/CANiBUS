#include <sstream>
#include "handler.h"
#include "socket.h"
#include "canibusmsg.h"
#include "state.h"
#include "screen.h"
#include "logger.h"
#include "clients.h"
#include "canbusdevice.h"
#include "session.h"
#include "options.h"

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
	m_state->setStatus(CanibusState::Lobby);
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

CanibusMsg *CanibusHandler::systemMsg(std::string data)
{
	CanibusMsg *msg = new CanibusMsg;
	msg->setType("chat");
	msg->setValue(data);
	return msg;
}

CanibusMsg *CanibusHandler::processMsg(const char *packet)
{
	bool validXml = false;
	CanibusMsg *msg = 0;
	if(!validatePacket(packet)) 
		return 0;

	TiXmlElement *emsg = m_xmlRoot->FirstChild("msg").Element();
	TiXmlElement *clientUpdate = m_xmlRoot->FirstChild("clientupdate").Element();
	TiXmlElement *client = m_xmlRoot->FirstChild("client").Element();
	TiXmlElement *deleteclient = m_xmlRoot->FirstChild("deleteclient").Element();
	TiXmlElement *finishedinit = m_xmlRoot->FirstChild("finishedinit").Element();
	TiXmlElement *sessionupdate = m_xmlRoot->FirstChild("hacksessionupdate").Element();
	TiXmlElement *deletesession = m_xmlRoot->FirstChild("deletesession").Element();
	TiXmlHandle updatehacksessionlist = m_xmlRoot->FirstChild("updatehacksessionlist");
	TiXmlHandle configupdate = m_xmlRoot->FirstChild("configupdate");

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
		validXml = true;
	} 
	if(sessionupdate) {
		const char *sessionid = sessionupdate->Attribute("sessionid");
		const char *status = sessionupdate->Attribute("status");
		const char *desc = sessionupdate->Attribute("description");
		const char *master = sessionupdate->Attribute("master");
		const char *max = sessionupdate->Attribute("maxclients");
		const char *priv = sessionupdate->Attribute("private");
		const char *clients = sessionupdate->Attribute("clients");
		CanibusSession *session = new CanibusSession(atoi(sessionid));
		if(status)
			session->setStatus(status);
		if(desc)
			session->setDesc(desc);
		if(master)
			session->setMasterId(atoi(master));
		if(max)
			session->setMaxClients(atoi(max));
		if(priv)
			session->setPrivate(atoi(priv));
		if(clients)
			session->setClientCount(atoi(clients));
		m_state->updateSession(session);
		validXml = true;
	}
	if(deletesession) {
		const char *sessionid=deletesession->Attribute("sessionid");
		if(sessionid) 
			m_state->delSession(atoi(sessionid));
		validXml = true;
	}
	if(client) { // This is always us
		const char *clientid = client->Attribute("clientid");
		const char *cookie = client->Attribute("cookie");
		CanibusClient *cInfo = new CanibusClient(atoi(clientid));
		if(cookie)
			cInfo->setCookie(cookie);
		cInfo->setName(m_state->nick());
		m_state->setOurClient(cInfo);
		m_state->updateClient(cInfo);
		validXml = true;
	} 
	if(clientUpdate) {
	   int status, updates = 0;
	   CanibusClient *oldUser = 0;
	   CanibusClient *cInfo = 0;
	   for(clientUpdate; clientUpdate; clientUpdate = clientUpdate->NextSiblingElement()) {
		if(std::string(clientUpdate->Value()).compare("clientupdate") != 0)
			continue;
		const char *clientid = clientUpdate->Attribute("clientid");
		const char *name = clientUpdate->Attribute("name");
		const char *host = clientUpdate->Attribute("host");
		const char *session = clientUpdate->Attribute("session");
		cInfo = new CanibusClient(atoi(clientid));
		if(name)
			cInfo->setName(name);
		if(host)
			cInfo->setHost(host);
		if(session)
			cInfo->setSession(atoi(session));
		oldUser = m_state->findClientById(cInfo->id());
		status = m_state->updateClient(cInfo);
		updates++;
	    }
	    if(updates == 1) {
		std::string tmpName;
		if(oldUser)
			tmpName = oldUser->name();
		if(status == CLIENT_NICK_CHANGE && oldUser && tmpName.size() > 0) {
			msg = systemMsg(tmpName +" is now known as " + cInfo->name());
		} else if (status == CLIENT_ADDED && m_finishedInit) {
			msg = systemMsg(cInfo->name() + " has joined.");
		}
	    }
  	    validXml = true;
	}
	if (deleteclient) {
		const char *clientid = deleteclient->Attribute("clientid");
		CanibusClient *left = m_state->findClientById(atoi(clientid));
		if(left) {
			m_state->delClient(left);
			msg = new CanibusMsg();
			msg->setType("chat");
			msg->setValue(left->name() + " has disconnected");
		}
		validXml = true;
	}
	if(configupdate.ToElement()) {
		const char *sessionid = configupdate.ToElement()->Attribute("sessionid");
		if(!sessionid) {
			logger->log("Invalid session id");
			logger->log(m_xmlDoc);
			msg = systemMsg("Received invalid session id");
		} else {
		CanibusSession *session = m_state->findSessionById(atoi(sessionid));
		if(!session) {
			logger->log("Could not find session id: %s\n", sessionid);
			msg = systemMsg("Received an unknown session id");
		} else {
		m_state->setActiveSession(session);
		m_state->setStatus(CanibusState::Config);
		TiXmlElement *option = configupdate.FirstChild("option").Element();

		for(option; option; option = option->NextSiblingElement()) {
			if(std::string(option->Value()).compare("option") != 0)
				continue;
			const char *type = option->Attribute("type");
			const char *title = option->Attribute("title");
			const char *command = option->Attribute("command");
			const char *value = option->Attribute("value");
			const char *edit = option->Attribute("edit");
			CanibusOption *option = new CanibusOption(atoi(sessionid));
			if(type)
				option->setType(type);
			if(title)
				option->setTitle(title);
			if(command)
				option->setCommand(command);
			if(value)
				option->setValue(value);
			if(edit)
				option->setCanEdit(atoi(edit));
			session->addOption(option);
		}
		validXml = true;
		} // Need to fix nesting tabs indents
		}
	}
	if(updatehacksessionlist.ToElement()) {
		const char *updateType = updatehacksessionlist.ToElement()->Attribute("type");
		if(updateType && std::string(updateType).compare("full") == 0)
			m_state->clearDevices();
		TiXmlElement *canbus = updatehacksessionlist.FirstChild("canbus").Element();
		TiXmlElement *session = updatehacksessionlist.FirstChild("session").Element();
		for(canbus; canbus; canbus = canbus->NextSiblingElement()) {
			if(std::string(canbus->Value()).compare("canbus") != 0)
				continue;
			const char *id = canbus->Attribute("id");
			const char *type = canbus->Attribute("type");
			const char *desc = canbus->Attribute("description");
			CanbusDevice *canbus = new CanbusDevice(atoi(id));
			canbus->setType(type);
			canbus->setDesc(desc);
			m_state->updateCanbusDevice(canbus);
		}
		for(session; session; session = session->NextSiblingElement()) {
			if(std::string(session->Value()).compare("session") != 0)
				continue;
			const char *id = session->Attribute("id");
			const char *status = session->Attribute("status");
			const char *canbusid = session->Attribute("canbusid");
			const char *clients = session->Attribute("clients");
			const char *priv = session->Attribute("private");
			CanibusSession *hax = new CanibusSession(atoi(id));
			if(status)
				hax->setStatus(status);
			if(clients)
				hax->setClientCount(atoi(clients));
			if(priv)
				hax->setPrivate(atoi(priv));
			if(canbusid) {
				CanbusDevice *can = m_state->findCanbusDeviceById(atoi(canbusid));
				if(can)
					hax->setCanbus(can);
			}
			m_state->updateSession(hax);
		}
		m_screen->updateLobby();
		validXml = true;
	} 
	if (finishedinit) {
		m_finishedInit = true;
		msg = new CanibusMsg();
		msg->setType("chat");
		std::ostringstream oss;
		oss <<"There are " << m_state->clients().size() << " client(s) connected.";
		msg->setValue(oss.str());
		validXml = true;
	}
	if(!validXml) 
		logger->log(m_xmlDoc);
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

void CanibusHandler::join(std::string canbusid)
{
	m_socket->ioWrite(".sj"+canbusid);
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

int CanibusHandler::status()
{
	if(!m_state)
		return 0;
	return m_state->status();
}

