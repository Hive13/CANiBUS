/* Canibus sample client */
#include "main.h"
#include "socket.h"
#include "handler.h"
#include "screen.h"
#include "canibusmsg.h"

#include <ncurses.h>

#include <iostream>
#include <string>

using namespace std;

int main(int argc, char *argv[]) {
  bool quit = false;
  Socket *client = 0;
  CanibusHandler *handler = 0;
  CanibusMsg *msg = 0;
  std::string username;
  if (argc == 3) {
  	client = new Socket(argv[1], atoi(argv[2]));
  } else {
	client = new Socket();
  } 

  cout << "Username: ";
  cin >> username;
  std::string cmd;
  if(client->connect()) {
	client->enablePolling();
	handler = new CanibusHandler(client);
	Screen *scr = new Screen();
	handler->setScreen(scr);
	handler->nick(username);
	scr->centerWrite("Connected to CANiBUS Server v"+client->serverVersion());
	while(!quit) {
		if((msg = handler->poll())) {
			if(msg->type().compare("chat") == 0)
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
