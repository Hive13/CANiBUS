// CANiBUS
//
// (c) 2012 OpenGarages.org

#include <signal.h>
#include <syslog.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include <iostream>
#include <string>

#include <libcapsinetwork/listener.h>
#include <libcapsinetwork/socket.h>

#include "const.h"
#include "main.h"
#include "server.h"

CanibusListener::CanibusListener( CanibusServer *server )
 :      Listener(),
        m_server( server )
{
	if ( addListenPort( server->port() ) == -1 )
	{
		syslog( LOG_ERR, "could not bind port %d, exiting", server->port() );
		exit(1);
	}
	else
		syslog( LOG_NOTICE, "listener: port=[%d]", server->port() );
}

void CanibusListener::socketHandler( Socket *socket, const std::string &data )
{
	switch( socket->status() )
	{
		case Socket::New:
			syslog( LOG_INFO, "connection: fd=[%d], ip=[%s]", socket->fd(), socket->ipAddr().c_str() );
			socket->ioWrite( std::string("<canibusd><server version=\"") + CANIBUSD_VERSION_STRING + "\"/></canibusd>\n" );
			m_server->initSocketTimeoutEvent( socket->fd() );
			break;

		case Socket::Close:
		case Socket::Closed:
			syslog( LOG_INFO, "disconnect: fd=[%d], ip=[%s]", socket->fd(), socket->ipAddr().c_str() );
			m_server->closedSocket( socket );
			break;
		case Socket::Ok:
			m_server->processInput( socket, data );
			break;
	}
}

int main(int argc, char **argv)
{
	srand( (unsigned) time(0) );
	signal( SIGPIPE, SIG_IGN );

	openlog( "canibusd", LOG_PID, LOG_DAEMON );
	syslog( LOG_NOTICE, "canibusd %s started", CANIBUSD_VERSION_STRING );

	CanibusServer *server = new CanibusServer();
	if( argc > 1 )
		server->setPort( atoi(argv[1]) );

	CanibusListener *listener = new CanibusListener( server );

	fprintf(stderr, "Server is up and running\n");

	for(;;)
	{
		// Check for network events
		listener->checkActivity();

		// Check for scheduled events
		int fd = server->processEvents();
		if(fd != -1)
		{
			Socket *delSocket = listener->findSocket(fd);
			if (delSocket && !server->findClient(delSocket))
				delSocket->setStatus(Socket::Close);
		}
	}

	delete server;
	delete listener;

	return 0;
}

