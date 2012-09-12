#include <sys/ioctl.h>
#include <termios.h>
#include <stdio.h>
#include <ncurses.h>

#include <string>

#include "canibusmsg.h"
#include "screen.h"
#include "state.h"
#include "logger.h"
#include "canbusdevice.h"

Screen::Screen()
{
	logger = new CanibusLogger("screen_errlog.txt");

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

	promptUpdated = true;
	chatUpdated = true;
	lobbyUpdated = true;
	configUpdated = true;
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
		}
		break;
	case CanibusState::Config:
		if(configUpdated) {
			wclear(m_configWin);
			updateConfigWindow();
			wrefresh(m_configWin);
			configUpdated = false;
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
	}
	if(promptUpdated) {
		wclear(m_promptWin);
		displayPrompt();
		mvwprintw(m_promptWin,0, 10, m_command.c_str());
		wrefresh(m_promptWin);
		promptUpdated = false;
	}
}

void Screen::updateConfigWindow()
{
	mvwprintw(m_configWin, 0, 1, "Config Options");
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
			if(cmsg->author() != "Unknown") {
				if(has_colors() == FALSE) {
					fmsg = "< " + cmsg->author() + "> " + cmsg->value();
					mvwprintw(m_chatWin, row-2-i, 1, fmsg.c_str());
				} else {
					fmsg = "< " + cmsg->author() + "> ";
					attron(COLOR_PAIR(1));
					mvwprintw(m_chatWin, row-2-i, 1, fmsg.c_str());
					attroff(COLOR_PAIR(1));
					attron(COLOR_PAIR(2));
					mvwprintw(m_chatWin, row-2-i, 1+fmsg.size(), cmsg->value().c_str());
					attroff(COLOR_PAIR(2));
				}
			} else { // systme message
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

std::string Screen::getPrompt()
{
	std::string cmd;
	int c;
	c = getch();
	if(c != ERR) {
		if(c == '\n' || c == '\r') {
			cmd = m_command;
			m_command.clear();
		} else if(c == KEY_BACKSPACE) {
			if(m_command.size() > 0)
				m_command.erase(m_command.size()-1, 1);
		} else if(c == KEY_RESIZE) {
			resize();
			refreshScr();
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

