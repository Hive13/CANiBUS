#include "canpacket.h"
#include "session.h"

CanibusSession::CanibusSession(int id)
{
	m_id = id;
	m_numClients = -1;
	m_isPrivate = false;
	m_canbusDevice = 0;
	m_masterId = -1;
	m_maxClients = 20;
}

CanibusSession::~CanibusSession()
{

}

void CanibusSession::setMasterId(int master_id)
{
	// Eventually lookup and validate we know the id
	m_masterId = master_id;
}

void CanibusSession::addPacket(CanPacket *pkt)
{
	m_packets[pkt->arbId()] = pkt;
}
