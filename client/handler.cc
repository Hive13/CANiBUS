#include <sstream>
#include "handler.h"
#include "socket.h"
#include "canibusmsg.h"
#include "state.h"
#include "screen.h"
#include "logger.h"
#include "clients.h"
#include "canbusdevice.h"
#include "canpacket.h"
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

void CanibusHandler::leave()
{
	m_socket->ioWrite(".x");
}

void CanibusHandler::start()
{
	m_socket->ioWrite(".ss");
}

void CanibusHandler::startMonitor()
{
	m_socket->ioWrite(".sm");
}

void CanibusHandler::stopMonitor()
{
	m_socket->ioWrite(".sM");
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
	std::string tmpName;
	CanibusMsg *msg = 0;
	if(!validatePacket(packet)) 
		return 0;

	TiXmlElement *elem = m_xmlRoot->FirstChild().Element();
	//logger->log(m_xmlDoc);
	for(elem; elem; elem = elem->NextSiblingElement()) {
		if(std::string(elem->Value()).compare("msg") == 0) {
			const char *type = elem->Attribute("type");
			const char *clientid = elem->Attribute("clientid");
			const char *author = elem->Attribute("author");
			const char *value = elem->Attribute("value");
			msg = new CanibusMsg();
			if(type)
				msg->setType(type);
			if(clientid)
				msg->setClientId(atoi(clientid));
			if(author)
				msg->setAuthor(author);
			if(value)
				msg->setValue(value);
			validXml = true;
		} 
		if(std::string(elem->Value()).compare("hacksessionupdate") == 0) {
			const char *sessionid = elem->Attribute("sessionid");
			const char *status = elem->Attribute("status");
			const char *desc = elem->Attribute("description");
			const char *master = elem->Attribute("master");
			const char *max = elem->Attribute("maxclients");
			const char *priv = elem->Attribute("private");
			const char *clients = elem->Attribute("clients");
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
			if(status && std::string(status).compare("init") == 0) {
				m_screen->centerWrite("Initializing...");
			}
			if(status && std::string(status).compare("run") == 0) {
				m_state->setStatus(CanibusState::Run);
				m_screen->updateSession();
				m_screen->updateChat();
			} else {
				m_screen->updateConfig();
			}
			validXml = true;
		}
		if(std::string(elem->Value()).compare("deletesession") == 0) {
			const char *sessionid=elem->Attribute("sessionid");
			// Removing for now, may not be needed
			//if(sessionid) 
				//m_state->delSession(atoi(sessionid));
			validXml = true;
		}
		if(std::string(elem->Value()).compare("client") == 0) {
			const char *clientid = elem->Attribute("clientid");
			const char *cookie = elem->Attribute("cookie");
			CanibusClient *cInfo = new CanibusClient(atoi(clientid));
			if(cookie)
				cInfo->setCookie(cookie);
			cInfo->setName(m_state->nick());
			m_state->setOurClient(cInfo);
			m_state->updateClient(cInfo);
			validXml = true;
		} 
		if(std::string(elem->Value()).compare("clientupdate") == 0) {
		   	int status, updates = 0;
		   	CanibusClient *oldUser = 0;
		   	CanibusClient *cInfo = 0;
			CanibusSession *newsession = 0;
			const char *clientid = elem->Attribute("clientid");
			const char *name = elem->Attribute("name");
			const char *host = elem->Attribute("host");
			const char *session = elem->Attribute("session");
			cInfo = new CanibusClient(atoi(clientid));
			if(name)
				cInfo->setName(name);
			if(host)
				cInfo->setHost(host);
			if(session) {
				newsession = m_state->findSessionById(atoi(session));
				cInfo->setSession(atoi(session));
			}
			oldUser = m_state->findClientById(cInfo->id());
			if(oldUser)
				tmpName = oldUser->name();
			status = m_state->updateClient(cInfo);
			if(oldUser == m_state->me() && cInfo->session() == -1 && oldUser->session() != -1)
			{ // We have left a session
				m_state->setStatus(CanibusState::Lobby);
				m_state->setActiveSession(0);
				m_screen->clearChat();
				m_screen->updateLobby();
				m_screen->updateChat();
			}
			if(oldUser == m_state->me() && newsession)
				m_state->setActiveSession(newsession);
			updates++;
		    if(updates == 1) {
			if(status == CLIENT_NICK_CHANGE && oldUser && tmpName.size() > 0 && cInfo->name().size() > 0) {
				msg = systemMsg(tmpName +" is now known as " + cInfo->name());
			} else if (status == CLIENT_ADDED && m_finishedInit) {
				if(m_state->status() == CanibusState::Lobby) {
					msg = systemMsg(cInfo->name() + " has joined.");
				}
			}
			if(m_state->activeSession() && oldUser && oldUser != m_state->me())
			{
				if(cInfo->session() == m_state->activeSession()->id())
					msg = systemMsg(tmpName + " has joined.");
				else if(session && cInfo->session() == -1)
					msg = systemMsg(tmpName + " has left.");
			} 

		    }
  		    validXml = true;
		}
		if(std::string(elem->Value()).compare("packet") == 0) {
			const char *seqNo = elem->Attribute("seq");
			const char *recvTime = elem->Attribute("receivetime");
			const char *relTime = elem->Attribute("relativetime");
			const char *error = elem->Attribute("error");
			const char *transmitted = elem->Attribute("transmitted");
			const char *networkname = elem->Attribute("networkname");
			const char *arbId = elem->Attribute("arbid");
			const char *extended = elem->Attribute("extended");
			const char *pktSize = elem->Attribute("size");
			const char *changed = elem->Attribute("changed");
			const char *b1 = elem->Attribute("b1");
			const char *b2 = elem->Attribute("b2");
			const char *b3 = elem->Attribute("b3");
			const char *b4 = elem->Attribute("b4");
			const char *b5 = elem->Attribute("b5");
			const char *b6 = elem->Attribute("b6");
			const char *b7 = elem->Attribute("b7");
			const char *b8 = elem->Attribute("b8");
			CanPacket *pkt = new CanPacket(atoi(seqNo));
			if(recvTime)
				pkt->setRecvTime(atof(recvTime));
			if(relTime)
				pkt->setRelTime(atof(relTime));
			if(error && atoi(error) > 0)
				pkt->setError();
			if(transmitted && atoi(transmitted) > 0)
				pkt->setTransmitted();
			if(networkname)
				pkt->setNetworkName(networkname);
			if(arbId)
				pkt->setArbId(atoi(arbId));
			if(extended && atoi(extended) > 0)
				pkt->setExtended();
			if(pktSize)
				pkt->setSize(atoi(pktSize));
			if(changed)
				pkt->setChanged(atoi(changed));
			if(b1)
				pkt->setB1(atoi(b1));
			if(b2)
				pkt->setB2(atoi(b2));
			if(b3)
				pkt->setB3(atoi(b3));
			if(b4)
				pkt->setB4(atoi(b4));
			if(b5)
				pkt->setB5(atoi(b5));
			if(b6)
				pkt->setB6(atoi(b6));
			if(b7)
				pkt->setB7(atoi(b7));
			if(b8)
				pkt->setB8(atoi(b8));
			m_state->activeSession()->addPacket(pkt);
			m_screen->updateSession();
			validXml = true;
		}
		if(std::string(elem->Value()).compare("deleteclient") == 0) {
			const char *clientid = elem->Attribute("clientid");
			CanibusClient *left = m_state->findClientById(atoi(clientid));
			if(left) {
				m_state->delClient(left);
				msg = systemMsg(left->name() + " has disconnected");
			}
			validXml = true;
		}
		if(std::string(elem->Value()).compare("configupdate") == 0) {
			const char *sessionid = elem->Attribute("sessionid");
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
			m_screen->clearChat();
			m_screen->updateChat();
			TiXmlElement *option = elem->FirstChildElement("option");
	
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
		if(std::string(elem->Value()).compare("updatehacksessionlist") == 0) {
			const char *updateType = elem->Attribute("type");
			if(updateType && std::string(updateType).compare("full") == 0)
				m_state->clearDevices();
			TiXmlElement *canbus = elem->FirstChildElement("canbus");
			TiXmlElement *session = elem->FirstChildElement("session");
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
		if(std::string(elem->Value()).compare("finishedinit") == 0) {
			m_finishedInit = true;
			msg = new CanibusMsg();
			msg->setType("chat");
			std::ostringstream oss;
			oss <<"There are " << m_state->clients().size() << " client(s) connected.";
			msg->setValue(oss.str());
			validXml = true;
		}

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

