/* Canibus sample client */
#include "main.h"
#include "socket.h"
#include "handler.h"
#include "screen.h"
#include "state.h"
#include "canibusmsg.h"

#include <signal.h>
#include <ncurses.h>

#include <iostream>
#include <string>

using namespace std;
 
// Global to support signals
CanibusHandler *handler = 0;

void do_resize(int dummy)
{
  if(handler && handler->screen())  {
	handler->screen()->resize();
	handler->screen()->refreshScr();
  }
}

int main(int argc, char *argv[]) {
  bool quit = false;
  Socket *client = 0;
  CanibusMsg *msg = 0;
  std::string username;
  if (argc == 3) {
  	client = new Socket(argv[1], atoi(argv[2]));
  } else {
	client = new Socket();
  } 

  cout << "Username: ";
  cin >> username;
  signal(SIGWINCH, do_resize);
  std::string cmd;
  if(client->connect()) {
	client->enablePolling();
	handler = new CanibusHandler(client);
	Screen *scr = new Screen();
	handler->setScreen(scr);
	handler->nick(username);
	while(!quit) {
		if((msg = handler->poll())) {
			if(msg->type().compare("chat") == 0 ||
			   msg->type().compare("error") == 0)
				scr->addChat(msg);
		}
		scr->refreshScr();
		cmd=scr->getPrompt();
		   if(cmd[0] == '/') {
			if(cmd.compare("/quit") == 0) {
				handler->disconnect();
				quit = true;
			} else if(cmd.compare(0, 6, "/nick ") == 0)
				handler->nick(cmd.substr(6));
			else if(cmd.compare(0, 8, "/refresh") == 0)
				handler->refresh();
			else if(cmd.compare(0, 6, "/join ") == 0)
				handler->join(cmd.substr(6));
			else if(handler->status() == CanibusState::Config) {
				if(cmd.compare(0, 6, "/leave") == 0)
					handler->leave();
				else if(cmd.compare(0, 6, "/start") == 0)
					handler->start();
				else
					scr->addChat(handler->systemMsg("Invalid command"));
			} else if(handler->status() == CanibusState::Run) {
				if(cmd.compare(0, 6, "/leave") == 0)
					handler->leave();
				else if(cmd.compare(0, 6, "/start") == 0)
					handler->startMonitor();
				else if(cmd.compare(0, 5, "/stop") == 0)
					handler->stopMonitor();
				else
					scr->addChat(handler->systemMsg("Invalid command"));
			} else
				scr->addChat(handler->systemMsg("Invalid command"));
		   } else {
			handler->sendChat(cmd);
		   }
	}
	delete scr;
	delete handler;

	client->close();
	delete client;
  } else {
	printf("Not Connected\n");
  }
  return 0;
}
