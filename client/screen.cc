#include <sys/ioctl.h>
#include <termios.h>
#include <stdio.h>
#include <ncurses.h>

#include <string>

#include "canibusmsg.h"
#include "screen.h"
#include "state.h"
#include "clients.h"
#include "logger.h"
#include "canbusdevice.h"
#include "canpacket.h"
#include "session.h"
#include "options.h"

Screen::Screen()
{
	logger = new CanibusLogger("screen_errlog.txt");
	m_historyIndex = -1;

	initscr();	// Starts curses mode
	keypad(stdscr, TRUE);	// Enable F1, F2..
	cbreak();	// disable line buffering
	halfdelay(1);	// non-blocking-ish
	noecho();	// We'll handle echo

	if(has_colors() != FALSE) {
		start_color();
		init_pair(1, COLOR_BLUE, -1);
		init_pair(2, COLOR_WHITE, -1);
		init_pair(3, COLOR_GREEN, -1);
	}
	
	m_promptWin = 0;
	m_chatWin = 0;
	m_lobbyWin = 0;
	m_configWin = 0;
	m_sessionWin = 0;
	resize();
}

Screen::~Screen()
{
	endwin();
	delete logger;
}

void Screen::resize()
{
	int row, col;
	struct winsize ts;
	ioctl(STDIN_FILENO, TIOCGWINSZ, &ts);
	resizeterm(ts.ws_row, ts.ws_col);
	getmaxyx(stdscr,row,col);
	m_row = row;
	m_col = col;
	if(m_promptWin) 
		mvwin(m_promptWin, row-1, 0);
	else
		m_promptWin = newwin(1, col, row-1, 0);
	if(m_chatWin)
		mvwin(m_chatWin, row-10, 0);
	else
		m_chatWin = newwin( 9, col, row-10, 0 );

	if(m_lobbyWin)
		wresize(m_lobbyWin, row-11, col);
	else
		m_lobbyWin = newwin( row-11, col, 1, 0 );
	if(m_configWin)
		wresize(m_configWin, row-10, col );
	else
		m_configWin = newwin( row-10, col, 0, 0 );
	if(m_sessionWin)
		wresize(m_sessionWin, row-10, col );
	else
		m_sessionWin = newwin( row-10, col, 0, 0 );

	promptUpdated = true;
	chatUpdated = true;
	lobbyUpdated = true;
	configUpdated = true;
	sessionUpdated = true;
}

void Screen::refreshScr()
{
	int row, col;
	switch(m_state->status())
	{
	case CanibusState::Lobby:
		if(lobbyUpdated) {
			updateLobbyTitle();
			wclear(m_lobbyWin);
			updateLobbyWindow();
			wmove(m_promptWin, 0, 10 + m_command.length());
			wrefresh(m_lobbyWin);
			lobbyUpdated = false;
			promptUpdated = true;
		}
		break;
	case CanibusState::Config:
		if(configUpdated) {
			wclear(m_configWin);
			updateConfigWindow();
			wrefresh(m_configWin);
			configUpdated = false;
			promptUpdated = true;
		}
		break;
	case CanibusState::Run:
		if(sessionUpdated) {
			wclear(m_sessionWin);
			updateSessionWindow();
			wrefresh(m_sessionWin);
			sessionUpdated = false;
			promptUpdated = true;
		}
		break;
	}
	/* Always on screen */
	if(chatUpdated) {
		wclear(m_chatWin);
		updateChatWindow();
		wmove(m_promptWin, 0, 10 + m_command.length());
		wrefresh(m_chatWin);
		chatUpdated = false;
		promptUpdated = true;
	}
	if(promptUpdated) {
		wclear(m_promptWin);
		displayPrompt();
		mvwprintw(m_promptWin,0, 10, m_command.c_str());
		wrefresh(m_promptWin);
		promptUpdated = false;
	}
}

void Screen::updateSessionWindow()
{
	int row, col, maxRow, maxCol;
	char numbuf[25];
	getmaxyx(m_sessionWin, maxRow, maxCol);
	mvprintw(0, 0, "        ArbID  Name        B1 B2 B3 B4 B5 B6 B7 B8");
	mvchgat(0, 0, -1, A_REVERSE, 0, NULL);
	if(!m_state->activeSession())
		return;
	map<std::string, CanPacket*>::iterator it;
	map<std::string, CanPacket *>packets = m_state->activeSession()->packets();
	row = 1;
	for(it = packets.begin() ; it != packets.end() && row < maxRow ; ++it)
	{
		col = 1;
		snprintf(numbuf, 24, "%ld", it->second->getPacketCount());
		mvwprintw(m_sessionWin, row, col, numbuf);
		col += 8;
		snprintf(numbuf, 24, "%d", it->second->arbId());
		mvwprintw(m_sessionWin, row, col, numbuf);
		col += 4;
		mvwprintw(m_sessionWin, row, col, it->second->networkName().c_str());
		col += 11;
		int changed = it->second->changed();
		switch(it->second->size())
		{
		case 8:
			if(changed & B8_CHANGE) wattron(m_sessionWin, A_REVERSE);
			snprintf(numbuf, 24, "%02X", it->second->b8());
			mvwprintw(m_sessionWin, row, col+24, numbuf);
			if(changed & B8_CHANGE) wattroff(m_sessionWin, A_REVERSE);
		case 7:
			if(changed & B7_CHANGE) wattron(m_sessionWin, A_REVERSE);
			snprintf(numbuf, 24, "%02X", it->second->b7());
			mvwprintw(m_sessionWin, row, col+21, numbuf);
			if(changed & B7_CHANGE) wattroff(m_sessionWin, A_REVERSE);
		case 6:
			if(changed & B6_CHANGE) wattron(m_sessionWin, A_REVERSE);
			snprintf(numbuf, 24, "%02X", it->second->b6());
			mvwprintw(m_sessionWin, row, col+18, numbuf);
			if(changed & B6_CHANGE) wattroff(m_sessionWin, A_REVERSE);
		case 5:
			if(changed & B5_CHANGE) wattron(m_sessionWin, A_REVERSE);
			snprintf(numbuf, 24, "%02X", it->second->b5());
			mvwprintw(m_sessionWin, row, col+15, numbuf);
			if(changed & B5_CHANGE) wattroff(m_sessionWin, A_REVERSE);
		case 4:
			if(changed & B4_CHANGE) wattron(m_sessionWin, A_REVERSE);
			snprintf(numbuf, 24, "%02X", it->second->b4());
			mvwprintw(m_sessionWin, row, col+12, numbuf);
			if(changed & B4_CHANGE) wattroff(m_sessionWin, A_REVERSE);
		case 3:
			if(changed & B3_CHANGE) wattron(m_sessionWin, A_REVERSE);
			snprintf(numbuf, 24, "%02X", it->second->b3());
			mvwprintw(m_sessionWin, row, col+9, numbuf);
			if(changed & B3_CHANGE) wattroff(m_sessionWin, A_REVERSE);
		case 2:
			if(changed & B2_CHANGE) wattron(m_sessionWin, A_REVERSE);
			snprintf(numbuf, 24, "%02X", it->second->b2());
			mvwprintw(m_sessionWin, row, col+6, numbuf);
			if(changed & B2_CHANGE) wattroff(m_sessionWin, A_REVERSE);
		case 1:
			if(changed & B1_CHANGE) wattron(m_sessionWin, A_REVERSE);
			snprintf(numbuf, 24, "%02X", it->second->b1());
			mvwprintw(m_sessionWin, row, col+3, numbuf);
			if(changed & B1_CHANGE) wattroff(m_sessionWin, A_REVERSE);
		case 0:
			break;
		}
		row++;
	}
}

void Screen::updateConfigWindow()
{
	int row = 2;
	int configRow, configCol;
	getmaxyx(m_chatWin, configRow, configCol);
	mvwprintw(m_configWin, 0, 1, "Config Options");
	CanibusOption *option = 0;
	std::vector<CanibusOption *>options = m_state->activeSession()->options();
	for(std::vector<CanibusOption *>::iterator it = options.begin(); it != options.end() && (option = *it); ++it) {
		mvwprintw(m_configWin, row, 6, option->title().c_str());
		row++;
	}
	if(m_state->activeSession()->masterId() == m_state->me()->id()) {
		mvwprintw(m_configWin, configRow-1, 1, "Use /start when done configuring options");
	} else {
		mvwprintw(m_configWin, configRow-1, 1, "Waiting for session master to finish config options");
	}
}

void Screen::updateLobbyTitle()
{
	mvprintw(0, 0, "  #    Type         Description");
	mvchgat(0, 0, -1, A_REVERSE, 0, NULL);
}

void Screen::updateLobbyWindow()
{
	int row = 0;
	char numbuf[10];
	CanbusDevice *device = 0;
	std::vector<CanbusDevice *>devices = m_state->devices();
	for(std::vector<CanbusDevice *>::iterator it = devices.begin(); it != devices.end() && (device = *it); ++it)
	{
		snprintf(numbuf, 9, "%d", device->id());
		mvwprintw(m_lobbyWin, row, 2, numbuf);
		mvwprintw(m_lobbyWin, row, 7, device->type().c_str());
		mvwprintw(m_lobbyWin, row, 20, device->desc().c_str());
		row++;
	}
}

void Screen::updateChatWindow()
{
	int row, col;
	getmaxyx(m_chatWin, row, col);
	CanibusMsg *cmsg = 0;
	std::string fmsg;
	wborder(m_chatWin, '|', '|', '=', '-', '/', '\\', '+', '+');
	// Last 7 messages
	// TODO: redo this nonsense
	std::stack<CanibusMsg *>tmpStack;
	for(int i = 0; i < 7; i++) {
		if(m_lobbyChatLogs.size() > 0) {
			cmsg = m_lobbyChatLogs.top();
			tmpStack.push(cmsg);
			m_lobbyChatLogs.pop();
			if(cmsg->id() > 0) {
				std::string author = "<";
				if(m_state->isMaster(cmsg->id()))
					author += "@";
				else
					author += " ";
				author += cmsg->author() + "> ";
				if(has_colors() == FALSE) {
					fmsg = author + cmsg->value();
					mvwprintw(m_chatWin, row-2-i, 1, fmsg.c_str());
				} else {
					fmsg = author;
					attron(COLOR_PAIR(1));
					mvwprintw(m_chatWin, row-2-i, 1, fmsg.c_str());
					attroff(COLOR_PAIR(1));
					attron(COLOR_PAIR(2));
					mvwprintw(m_chatWin, row-2-i, 1+fmsg.size(), cmsg->value().c_str());
					attroff(COLOR_PAIR(2));
				}
			} else { // system message
				mvwprintw(m_chatWin, row-2-i, 1, (" ** " + cmsg->value()).c_str());
			}
		}
	}
	// Replace the stack back
	while(!tmpStack.empty()) {
		m_lobbyChatLogs.push(tmpStack.top());
		tmpStack.pop();
	}
}

void Screen::displayPrompt()
{
	wattron(m_promptWin, A_BOLD);
	mvwprintw(m_promptWin, 0,0, "#CANiBUS>");
	wattroff(m_promptWin, A_BOLD);
}

void Screen::write(std::string msg)
{
	printw(msg.c_str());
	refresh();
}

void Screen::centerWrite(std::string msg)
{
	int row, col;
	getmaxyx(stdscr,row,col);
	mvprintw(row/2, (col-msg.length())/2, msg.c_str());
	refresh();
}

std::string Screen::getHistory(int atIndex)
{
	std::string cmd;
	std::string foundCmd;
	std::stack<std::string>tmp;
	int idx = 0;
	for(; !m_cmdHistory.empty() && idx <= atIndex; m_cmdHistory.pop()) {
		cmd = m_cmdHistory.top();
		tmp.push(cmd);
		if(idx == atIndex) 
			foundCmd = cmd;
		idx++;
	}
	for(; !tmp.empty(); tmp.pop()) 
		m_cmdHistory.push(tmp.top());
	return foundCmd;
}

std::string Screen::getPrompt()
{
	std::string cmd;
	int c;
	c = getch();
	if(c != ERR) {
		if(c == '\n' || c == '\r') {
			cmd = m_command;
			if(cmd.size() > 0)
				m_cmdHistory.push(m_command);
			m_historyIndex = -1;
			m_command.clear();
		} else if(c == KEY_BACKSPACE) {
			if(m_command.size() > 0)
				m_command.erase(m_command.size()-1, 1);
		} else if(c == KEY_RESIZE) {
			resize();
			refreshScr();
		} else if(c == KEY_UP) {
			m_historyIndex++;
			if(m_historyIndex >= m_cmdHistory.size())
				m_historyIndex = m_cmdHistory.size()-1;
			std::string tmp = getHistory(m_historyIndex);
			if(tmp.size() > 0)
				m_command = tmp;
		} else if(c == KEY_DOWN) {
			m_historyIndex--;
			if(m_historyIndex < 0) {
				m_historyIndex = -1;
				m_command.clear();
			}
			std::string tmp = getHistory(m_historyIndex);
			if(tmp.size() > 0)
				m_command = tmp;
		} else {
			m_command += c;
		}
		promptUpdated = true;
	}
	return cmd;
}

void Screen::addChat(CanibusMsg *msg)
{
	m_lobbyChatLogs.push(msg);
	chatUpdated = true;
}

void Screen::clearChat()
{
	std::stack<CanibusMsg *> empty;
	std::swap(m_lobbyChatLogs, empty);
}
