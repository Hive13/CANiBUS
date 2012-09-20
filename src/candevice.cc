#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include "serial.h"
#include "candevice.h"
#include "canpacket.h"
#include "hacksession.h"

using namespace std;

CanDevice::CanDevice(int id, Model model) : SessionObject(id, SessionObject::SCan)
{
	setProperty("name", "Unknown", this);
	setProperty("description", "No description available");
	m_model = model;
}

CanDevice::~CanDevice()
{

}

const enum CanDevice::Model CanDevice::model()
{
	return m_model;
}

int CanDevice::getPacketChanges(CanPacket *pkt)
{
	int changes = 0;
	CanPacket *oldPkt = 0;
	map<int, CanPacket*>::iterator it = m_packetGroup.find(pkt->arbId());
	if(it == m_packetGroup.end()) { // New arbId
		m_packetGroup[pkt->arbId()] = pkt;
		changes = 255;
	} else {
		oldPkt = it->second;
		if(oldPkt->b1() != pkt->b1())
			changes |= B1_CHANGE;
		if(oldPkt->b2() != pkt->b2())
			changes |= B2_CHANGE;
		if(oldPkt->b3() != pkt->b3())
			changes |= B3_CHANGE;
		if(oldPkt->b4() != pkt->b4())
			changes |= B4_CHANGE;
		if(oldPkt->b5() != pkt->b5())
			changes |= B5_CHANGE;
		if(oldPkt->b6() != pkt->b6())
			changes |= B6_CHANGE;
		if(oldPkt->b7() != pkt->b7())
			changes |= B7_CHANGE;
		if(oldPkt->b8() != pkt->b8())
			changes |= B8_CHANGE;
		m_packetGroup[pkt->arbId()] = pkt;
	}
	return changes;
}

// Stub function
void CanDevice::init()
{

}

std::string CanDevice::port()
{
	return getStringProperty("port");
}

void CanDevice::addSession(HackSession *session)
{
	m_sessions.push_back(session);
	setProperty("sessions", m_sessions.size());
}

// Method for any monitorying or config init routines
void CanDevice::prepareMonitor()
{

}

void CanDevice::enableMonitor()
{
	setBoolProperty("monitor", true);
}

void CanDevice::disableMonitor()
{
	setBoolProperty("monitor", false);
}

bool CanDevice::isMonitoring()
{
	return getBoolProperty("monitor");
}

void CanDevice::delSession(HackSession *session)
{
	if(!session)
		return;
	for(std::vector<HackSession *>::iterator it = m_sessions.begin() ; it != m_sessions.end() && (*it) ; ++it)
		if(*it == session)
			m_sessions.erase(it);
	setProperty("sessions", m_sessions.size());
}

/*-------------------------------*/
CanbusSimulator::CanbusSimulator(int id) : CanDevice(id, CanDevice::SIM)
{
	setProperty("name", "CANSIM", this);
	setProperty("description", "CAN Simulation Module", this);
	m_clockSynced = false;
	m_internal_seq = 1;
}

void CanbusSimulator::enableMonitor()
{
	int packetSize = 0;
	std::string line;
	string seqNo, absTime, relTime, status, error, transmit;
	string desc, network, node, arbid, remote, extended;
	string b1,b2,b3,b4,b5,b6,b7,b8,value,trigger,signals;
	ifstream file( "sample-can-traffic.csv" );
	while ( file.good() )
	{
		getline(file, line);
		istringstream buf(line);
		getline(buf, seqNo, ',');
		getline(buf, absTime, ',');
		getline(buf, relTime, ',');
		getline(buf, status, ',');
		getline(buf, error, ',');
		getline(buf, transmit, ',');
		getline(buf, desc, ',');
		getline(buf, network, ',');
		getline(buf, node, ',');
		getline(buf, arbid, ',');
		getline(buf, remote, ',');
		getline(buf, extended, ',');
		getline(buf, b1, ',');
		getline(buf, b2, ',');
		getline(buf, b3, ',');
		getline(buf, b4, ',');
		getline(buf, b5, ',');
		getline(buf, b6, ',');
		getline(buf, b7, ',');
		getline(buf, b8, ',');
		getline(buf, value, ',');
		getline(buf, trigger, ',');
		getline(buf, signals);
		CanPacket *pkt = new CanPacket(atoi(seqNo.c_str()));
		pkt->setRecvTime(atof(absTime.c_str()));
		pkt->setRelTime(atof(relTime.c_str()));
		if(error.compare("T") == 0)
			pkt->setError();
		if(transmit.compare("T") == 0)
			pkt->setTransmitted();
		pkt->setNetworkName(network);
		pkt->setArbId(atoi(arbid.c_str()));
		if(extended.compare("T") == 0)
			pkt->setExtended();
		if(b1.size() > 0) {
			packetSize = 1;
			pkt->setB1(hexToChar(b1));
		}
		if(b2.size() > 0) {
			packetSize = 2;
			pkt->setB2(hexToChar(b2));
		}
		if(b3.size() > 0) {
			packetSize = 3;
			pkt->setB3(hexToChar(b3));
		}
		if(b4.size() > 0) {
			packetSize = 4;
			pkt->setB4(hexToChar(b4));
		}
		if(b5.size() > 0) {
			packetSize = 5;
			pkt->setB5(hexToChar(b5));
		}
		if(b6.size() > 0) {
			packetSize = 6;
			pkt->setB6(hexToChar(b6));
		}
		if(b7.size() > 0) {
			packetSize = 7;
			pkt->setB7(hexToChar(b7));
		}
		if(b8.size() > 0) {
			packetSize = 8;
			pkt->setB8(hexToChar(b8));
		}
		pkt->setSize(packetSize);
		int change = getPacketChanges(pkt);
		pkt->setChanged(change);
		m_packets.push_back(pkt);
	}
	m_it = m_packets.begin();
	m_clockSynced = false;
	fprintf(stderr, "SIM Loaded %ld packets into buffer\n", m_packets.size());
	setBoolProperty("monitor", true);
}

char CanbusSimulator::hexToChar(std::string b)
{
	char byte;
	istringstream iss(b);
	iss >> hex >> byte;
	return byte;
}

std::vector<CanPacket *>CanbusSimulator::poll()
{
	struct timespec currentTime;
	time_t seconds;
	long nseconds;
	double relDiff;
	bool synced;
	std::vector<CanPacket *>packet_q;

	if(!m_clockSynced) {
		clock_gettime(CLOCK_REALTIME, &m_lastTime);
		m_clockSynced = true;
	}
	clock_gettime(CLOCK_REALTIME, &currentTime);
	seconds = currentTime.tv_sec - m_lastTime.tv_sec;
	nseconds = currentTime.tv_nsec - m_lastTime.tv_nsec;
	relDiff = seconds + (nseconds / 1000000000.0);

	synced = false;
	CanPacket *pkt = 0;
	while(!synced) {
		if(m_it == m_packets.end())
			m_it = m_packets.begin();
		m_it++;
		pkt = *m_it;
		if(pkt) {
			if(relDiff > pkt->relTime()) {
				relDiff -= pkt->relTime();
				// Adjust seq
				pkt->setSeqNo(m_internal_seq++);
				packet_q.push_back(pkt);
			} else {
				synced = true;
			}
		}
	}
	clock_gettime(CLOCK_REALTIME, &m_lastTime);
	return packet_q;
}
