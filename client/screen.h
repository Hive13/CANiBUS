#ifndef __CANIBUS_SCREEN_H__
#define __CAINBUS_SCREEN_H__

#include "ncurses.h"

#include <vector>
#include <stack>

class CanibusMsg;
class CanibusState;
class CanibusLogger;
class CanbusDevice;
class CanibusState;
class CanibusSession;
class CanibusClient;

class Screen
{
public:
	Screen();
	~Screen();
	void write(std::string msg);
	void centerWrite(std::string msg);
	void displayPrompt();
	void updateChatWindow();
	void updateLobbyTitle();
	void updateLobbyWindow();
	void updateConfigWindow();
	void resize();
	void refreshScr();
	std::string getPrompt();
	void addChat(CanibusMsg *msg);
	void setState(CanibusState *state) { m_state = state; }
	CanibusState *state() { return m_state; }
	void updateLobby() { lobbyUpdated = true; }
	void updatePrompt() { promptUpdated = true; }
	void updateChat() { chatUpdated = true; }
	void updateConfig() { configUpdated = true; }
private:
	int m_row, m_col;
	bool promptUpdated, lobbyUpdated, chatUpdated, configUpdated;
	CanibusState *m_state;
	CanibusLogger *logger;
	std::string m_command;
	std::stack<CanibusMsg *>m_lobbyChatLogs;
	std::stack<CanibusMsg *>m_chatLogs;
	WINDOW *m_promptWin;
	WINDOW *m_chatWin;
	WINDOW *m_lobbyWin;
	WINDOW *m_configWin;
};

#endif
