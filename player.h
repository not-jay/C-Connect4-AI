#ifndef __player_h
#define __player_h

#include "board.h"

typedef enum {PLAYER, AI_PLAYER} PlayerType;

struct player {
	int			id;
	PlayerType	pt;
	Board		b;
	int			pointer;
	int			(*getID)(struct player *);
	bool		(*doMove)(struct player *, int);
};

typedef struct player *Player;

Player		newPlayer(int, PlayerType, Board);
int			getID(Player);
bool		doMove(Player, int);

Player newPlayer(int id, PlayerType pt, Board b) {
	Player p = (Player)calloc(1, sizeof(struct player));

	p->id = id;
	p->pt = pt;
	p->b = b;
	p->pointer = 0;

	p->getID = getID;
	p->doMove = doMove;

	return p;
}

int getID(Player p) {
	return p->id;
}

bool doMove(Player p, int key) {
	move(7, 34); clrtoeol();
	switch(key) {
		case KEY_LEFT:
			p->pointer -= 2;
			if(p->pointer < 0) p->pointer = 12;
			break;
		case KEY_RIGHT:
			p->pointer += 2;
			if(p->pointer > 12) p->pointer = 0;
			break;
		case 10: /* Windows Enter */
			/* If column is full, allow player to choose another column */
			if(p->b->isColumnFull(p->b, p->pointer/2)) return FALSE;
			p->b->dropPiece(p->b, p->id, p->pointer/2);
			return TRUE;
	}
	mvaddch(7, 34 + p->pointer, ACS_DARROW);
	return FALSE;
}

#endif