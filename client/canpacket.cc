#include "canpacket.h"

CanPacket::CanPacket(unsigned int seqNum)
{
	m_seqno = seqNum;
	m_extended = false;
	m_error = false;
	m_transmit = false;
	m_changed = 0;
	bzero(&m_byte, 8);
}

CanPacket::~CanPacket()
{

}

void CanPacket::setBytes(const char bytes[8])
{

}
