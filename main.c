#include <curses.h>
#include "board.h"
#include "player.h"

void boardDraw(Board);
void statusDraw(WINDOW *, Player);

int main() {
	Board board = newBoard();
	Player player = newPlayer(1, PLAYER, board),
		   ai = newPlayer(2, AI, board),
		   current;
	WINDOW *status;
	bool isRunning = TRUE;
	int key;

	initscr();				/* Start curses mode */
	start_color();			/* Color mode */
	cbreak();				/* Disable line-break mode */
	keypad(stdscr, TRUE);	/* Enable getch for function keys */
	curs_set(0);			/* Remove cursor */
	noecho();				/* Disable echo */
	init_pair(1, COLOR_YELLOW, COLOR_BLACK);
	init_pair(2, COLOR_RED, COLOR_BLACK);
	init_pair(3, COLOR_YELLOW, COLOR_YELLOW);
	init_pair(4, COLOR_RED, COLOR_RED);

	status = newwin(25, 20, 0, 0);
	current = player;

	while(isRunning) {
		boardDraw(board);
		statusDraw(status, current);
		if(current == player) {
			if(player->doMove(player, key)) {
				/* Pass turn to AI */
				current = ai;
				ungetch('a'); /* Skip the next keystroke and instantly go to the AI */
			}
		} else {
			if(ai->doMove(ai, key)) {
				/* Pass turn to AI */
				current = player;
				ungetch('a'); /* Skip the next keystroke and instantly go to the AI */
			}
		}
		if(board->checkForWin(board, current->getID(current))) {
			int pid = current->getID(current);

			wmove(status, 13, 1); wclrtoeol(status);
			wattron(status, A_BOLD | COLOR_PAIR(pid));
			wprintw(status, "%s", (pid == 1)?"Player":"AI");
			wattroff(status, A_BOLD | COLOR_PAIR(pid));
			wprintw(status, " won!");

			isRunning = FALSE;
			getch();
		}

		if((key = getch()) == KEY_F(1)) {
			isRunning = FALSE;
			break;
		}
	}

	board->destroyBoard(board);
	free(player);
	free(ai);
	endwin();
	return 0;
}

void boardDraw(Board b) {
	mvwprintw(stdscr, 1, 36, "Connect4");
	b->draw(stdscr, b, 8, 33);
}

void statusDraw(WINDOW *win, Player p) {
	bool isPlayer = p->getID(p) == 1;

	wattron(win, A_BOLD);
	mvwprintw(win, 1, 1, "Versus AI: Hard");
	mvwprintw(win, 5, 1, "Turn: ");
	wclrtoeol(win);
	wattron(win, COLOR_PAIR(isPlayer?1:2));
	wprintw(win, "%s", isPlayer?"Player":"AI");
	wattroff(win, COLOR_PAIR(isPlayer?1:2));
	mvwprintw(win, 9, 1, "AI Status:\n");
	wmove(win, 10, 1); wclrtoeol(win);
	//AI status printing
	wattroff(win, A_BOLD);
	wrefresh(win);
}