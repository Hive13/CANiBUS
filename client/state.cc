#include "state.h"
#include "clients.h"
#include "canbusdevice.h"
#include "session.h"
#include "logger.h"

CanibusState::CanibusState()
{
	m_status = Init;
	m_me = 0;
	logger = new CanibusLogger("state_errlog.txt");
}

CanibusState::~CanibusState()
{
	delete logger;
}

void CanibusState::setNick(std::string username)
{
	m_nick = username;
}

int CanibusState::updateClient(CanibusClient *cInfo)
{
	int status = CLIENT_NO_CHANGE;
	CanibusClient *knownClient = findClientById(cInfo->id());
	if(knownClient) { // Update info
		if(knownClient->name().compare(cInfo->name()) != 0) {
			if (knownClient->name().size() > 0)
				status = CLIENT_NICK_CHANGE;
			knownClient->setName(cInfo->name());
		}
		if(cInfo->host().size() > 0 && 
                   knownClient->host().compare(cInfo->host()) != 0)
			knownClient->setHost(cInfo->host());
		if(cInfo->session() > -1)
			knownClient->setSession(cInfo->session());
	} else { // New client add
		m_clients.push_back(cInfo);
		status = CLIENT_ADDED;
	}
	return status;
}

CanibusClient *CanibusState::findClientById(int id)
{
	CanibusClient *client = 0;
	for(std::vector<CanibusClient *>::iterator it = m_clients.begin(); it != m_clients.end() && (client = *it); ++ it)
		if (client->id() == id)
			return client;
	return 0;
}

CanibusSession *CanibusState::findSessionById(int id)
{
	CanibusSession *session = 0;
	for(std::vector<CanibusSession *>::iterator it = m_sessions.begin(); it != m_sessions.end() && (session = *it); ++ it)
		if (session->id() == id)
			return session;
	return 0;
}

void CanibusState::delClient(CanibusClient *client)
{
	for(std::vector<CanibusClient *>::iterator it = m_clients.begin() ; it != m_clients.end() && (*it) ; ++it)
		if(*it == client)
			m_clients.erase(it);
}

int CanibusState::updateSession(CanibusSession *sessionInfo)
{
	CanibusSession *knownSession = findSessionById(sessionInfo->id());
	if(knownSession) {
		if(sessionInfo->status().size() > 0)
			knownSession->setStatus(sessionInfo->status());
		if(sessionInfo->clientCount() > -1)
			knownSession->setClientCount(sessionInfo->clientCount());
		if(sessionInfo->isPrivate()) // prob wont turn off
			knownSession->setPrivate(sessionInfo->isPrivate());
		if(sessionInfo->canbusDevice())
			knownSession->setCanbus(sessionInfo->canbusDevice());
		if(sessionInfo->masterId() > -1)
			knownSession->setMasterId(sessionInfo->masterId());
		if(sessionInfo->desc().size() > 0)
			knownSession->setDesc(sessionInfo->desc());
		if(sessionInfo->maxClients() != knownSession->maxClients())
			knownSession->setMaxClients(sessionInfo->maxClients());
	} else {
		m_sessions.push_back(sessionInfo);
	}
}

int CanibusState::updateCanbusDevice(CanbusDevice *cbInfo)
{
	CanbusDevice *knownCanbus = findCanbusDeviceById(cbInfo->id());
	if(knownCanbus) {
		if(cbInfo->type().size() > 0)
			knownCanbus->setType(cbInfo->type());
		if(cbInfo->desc().size() > 0)
			knownCanbus->setDesc(cbInfo->desc());
	} else {
		m_canbusDevices.push_back(cbInfo);
	}
}

CanbusDevice *CanibusState::findCanbusDeviceById(int id)
{
	CanbusDevice *canbus = 0;
	for(std::vector<CanbusDevice *>::iterator it = m_canbusDevices.begin(); it != m_canbusDevices.end() && (canbus = *it); ++ it)
		if (canbus->id() == id)
			return canbus;
	return 0;
}

void CanibusState::delSession(int sessionId)
{
	CanibusSession *session = 0;
	for(std::vector<CanibusSession *>::iterator it = m_sessions.begin(); it != m_sessions.end() && (session = *it); ++it)
		if(session && session->id() == sessionId) {
			m_sessions.erase(it);
			//delete session;
		}
}

bool CanibusState::isMaster(int id)
{
	if(m_activeSession && id && m_activeSession->masterId() == id)
		return true;
	return false;
}

