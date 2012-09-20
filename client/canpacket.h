#ifndef __CANPACKET_H__
#define __CANPACKET_H__

#include "time.h"
#include "strings.h"
#include <string>

class CanPacket
{
public:
	CanPacket(unsigned int seqNo);
	~CanPacket();
	void setSeqNo(unsigned int seq) { m_seqno = seq; }
	unsigned int seqNo() { return m_seqno; }
	void setSize(int sz) { m_packet_size = sz; }
	int size() { return m_packet_size; }
	void setRecvTime(double t) { m_time_recv = t; }
	double recvTime() { return m_time_recv; }
	void setRelTime(double t) { m_rel_time = t; }
	double relTime() { return m_rel_time; }
	void setError() { m_error = true; }
	bool hasError() { return m_error; }
	bool setTransmitted() { m_transmit = true; }
	bool isTransmitted() { return m_transmit; }
	void setNetworkName(std::string name) { m_network_name = name; }
	std::string networkName() { return m_network_name; }
	void setArbId(unsigned int arbId) { m_arb_id = arbId; }
	unsigned int arbId() { return m_arb_id; }
	void setExtended() { m_extended = true; }
	bool isExtended() { return m_extended; }
	void setChanged(int change) { m_changed = change; }
	int changed() { return m_changed; }
	void setBytes(const char bytes[8]);
	const char *bytes() { return m_byte; }
	void setB1(char b) { m_byte[0] = b; }
	void setB2(char b) { m_byte[1] = b; }
	void setB3(char b) { m_byte[2] = b; }
	void setB4(char b) { m_byte[3] = b; }
	void setB5(char b) { m_byte[4] = b; }
	void setB6(char b) { m_byte[5] = b; }
	void setB7(char b) { m_byte[6] = b; }
	void setB8(char b) { m_byte[7] = b; }
	char b1() { return m_byte[0]; }
	char b2() { return m_byte[1]; }
	char b3() { return m_byte[2]; }
	char b4() { return m_byte[3]; }
	char b5() { return m_byte[4]; }
	char b6() { return m_byte[5]; }
	char b7() { return m_byte[6]; }
	char b8() { return m_byte[7]; }
private:
	unsigned int m_seqno;
	double m_time_recv;
	double m_rel_time;
	int m_packet_size;
	bool m_error;
	bool m_transmit;
	std::string m_network_name;
	unsigned int m_arb_id;
	bool m_extended;
	int m_changed;
	char m_byte[8];
};

#endif
