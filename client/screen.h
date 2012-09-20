#ifndef __CANIBUS_SCREEN_H__
#define __CAINBUS_SCREEN_H__

#include <menu.h>
#include "ncurses.h"

#include <vector>
#include <stack>

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))
#define B1_CHANGE (1 << 0)
#define B2_CHANGE (2 << 0)
#define B3_CHANGE (3 << 0)
#define B4_CHANGE (4 << 0)
#define B5_CHANGE (5 << 0)
#define B6_CHANGE (6 << 0)
#define B7_CHANGE (7 << 0)
#define B8_CHANGE (8 << 0)

class CanibusMsg;
class CanibusState;
class CanibusLogger;
class CanbusDevice;
class CanibusState;
class CanibusSession;
class CanibusClient;
class CanPacket;

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
	void updateSessionWindow();
	void resize();
	void refreshScr();
	std::string getPrompt();
	std::string getHistory(int index);
	void addChat(CanibusMsg *msg);
	void clearChat();
	void setState(CanibusState *state) { m_state = state; }
	CanibusState *state() { return m_state; }
	void updateLobby() { lobbyUpdated = true; }
	void updatePrompt() { promptUpdated = true; }
	void updateChat() { chatUpdated = true; }
	void updateConfig() { configUpdated = true; }
	void updateSession() { sessionUpdated = true; }
private:
	int m_row, m_col, m_historyIndex;
	int m_file_choices;
	bool promptUpdated, lobbyUpdated, chatUpdated, configUpdated;
	bool sessionUpdated;
	CanibusState *m_state;
	CanibusLogger *logger;
	std::string m_command;
	std::stack<CanibusMsg *>m_lobbyChatLogs;
	std::stack<CanibusMsg *>m_chatLogs;
	std::stack<std::string>m_cmdHistory;
	WINDOW *m_promptWin;
	WINDOW *m_chatWin;
	WINDOW *m_lobbyWin;
	WINDOW *m_configWin;
	WINDOW *m_sessionWin;
};

#endif
