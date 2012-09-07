#ifndef __CANIBUSD_MAIN_H__
#define __CANIBUSD_MAIN_H__

class CanibusServer;
class Socket;

#include <string>
#include <libcapsinetwork/listener.h>

class CanibusListener : public Listener
{
public:
	CanibusListener( CanibusServer *server );
	void socketHandler( Socket *socket, const std::string &data = "" );
private:
	Listener *m_listener;
	CanibusServer *m_server;
};

int main(int, char **);
#endif
