#include "canpacket.h"
#include "logger.h"
#include "session.h"

CanibusSession::CanibusSession(int id)
{
	m_id = id;
	m_numClients = -1;
	m_isPrivate = false;
	m_canbusDevice = 0;
	m_masterId = -1;
	m_maxClients = 20;
	logger = new CanibusLogger("session_errlog.txt");
}

CanibusSession::~CanibusSession()
{
	delete logger;
}

void CanibusSession::setMasterId(int master_id)
{
	// Eventually lookup and validate we know the id
	m_masterId = master_id;
}

void CanibusSession::addPacket(CanPacket *pkt)
{
	char buf[20];
	snprintf(buf, 19, "%d", pkt->arbId());
	std::string key = pkt->networkName() + buf;
	map<std::string, CanPacket *>::iterator it = m_packets.find(key);
	if(it != m_packets.end()) // Never seen
		pkt->setPacketCount(it->second->getPacketCount());
	else
		pkt->setPacketCount(0);
	pkt->incPacketCount();
	m_packets[key] = pkt;
}

void CanibusSession::clearPackets()
{
	for(map<std::string, CanPacket *>::iterator it = m_packets.begin() ; it != m_packets.end(); ++it)
		delete it->second;
	m_packets.clear();
}
