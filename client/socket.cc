#include "sys/types.h"
#include "sys/socket.h"
#include "socket.h"
#include "handler.h"
#include "logger.h"

Socket::Socket(const char *hostname, int port)
{
	setServer(hostname);
	setPort(port);

	logger = new CanibusLogger("socket_errlog.txt");
}

Socket::~Socket()
{
	delete logger;
}

void Socket::setServer(const char *hostname)
{
	m_server = gethostbyname(hostname);
}

void Socket::setPort(int port)
{
	m_port = port;
}

bool Socket::connect()
{
	struct sockaddr_in serv_addr;
	char buf[256];
	int n;
	m_connected = false;
	m_sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(m_sockfd < 0) {
		fprintf(stderr, "Could not open socket\n");
		return false;
	}
	if(m_server == NULL) {
		fprintf(stderr, "No such host\n");
		return false;
	}
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char *)m_server->h_addr, (char *)&serv_addr.sin_addr.s_addr, m_server->h_length);
	serv_addr.sin_port = htons(m_port);
	if (::connect(m_sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
		fprintf(stderr, "Error connecting\n");
		return false;
	}
	bzero(buf, 256);
	n = read(m_sockfd, buf, 255);
	if (n < 0) {
		fprintf(stderr, "Error reading from socket\n");
		return false;
	}
	CanibusHandler *handler = new CanibusHandler(this);
	if(handler->parseServerId(buf) == false) {
		fprintf(stderr, "Not a CANiBUS Server\n");
		return false;
	}
	m_serverVersion = handler->serverVersion();
	delete handler;

	m_connected = true;
	return true;
}

// Set to non-blocking mode
void Socket::enablePolling()
{
	int flags;
	flags = fcntl(m_sockfd, F_GETFL, 0);
	fcntl(m_sockfd, F_SETFL, flags | O_NONBLOCK);
}

void Socket::close()
{
	::close(m_sockfd);
	m_connected = false;
}

std::string Socket::poll()
{
	std::string packet;
	int n;
	char buf[255];

	n = read(m_sockfd, buf, 255);
	if(n > 0) {
		// read up to \n
		for(int i=0; i < n; i++) {
			if(buf[i] == '\n') {
				m_packetQ.push(m_packet);
				m_packet.clear();
			} else {
				if(buf[i] != 0)
					m_packet += buf[i];
			}
		}
	}
	if(m_packetQ.size() > 0) {
		packet = m_packetQ.front();
		m_packetQ.pop();
	}
	return packet;
}

void Socket::ioWrite(std::string msg)
{
	int n;
	msg += "\n";
	n = write(m_sockfd, msg.c_str(), msg.length());
}

