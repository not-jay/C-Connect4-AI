#ifndef __ai_h
#define __ai_h

#include <string.h>
#include "player.h"
#include "board.h"

typedef enum {NO_THREAT, ODD, EVEN} Parity;

struct thought {
	int		threats[7];
	Parity	parity[7];
};

struct ai {
	Player			player;
	struct thought	*t;
	char			status[80];
	int				(*doAIMove)(struct ai *);
	int				(*findThreats)(struct ai *, int *);
	void			(*setStatus)(struct ai *, char *);
	char *			(*getStatus)(struct ai *);
	void			(*destroyAI)(struct ai *);
};

typedef struct thought *Thoughts;
typedef struct ai *AI;

AI		newAI(Board);
int		doAIMove(AI);
int		findThreats(AI, int *);
void	setStatus(AI, char *);
char *	getStatus(AI);
void	destroyAI(AI);
/* Heuristic Functions */
bool	testHorizontal(Board, int, int, int);
bool	testVertical(Board, int, int, int);
bool	testPosDiagonal(Board, int, int, int);
bool	testNegDiagonal(Board, int, int, int);
/* Board Helper Functions */
int		getBottomMostSpace(Board, int);
/* Function Macros */
#define validateSpace(c, p) (c->isSpaceEmpty(c) || c->getOwner(c) != p)
#define setParity(ai, col, x) (ai->t->parity[col] = ((7-(x+1))%2 == 0)?EVEN:ODD)

AI newAI(Board b) {
	AI ai = (AI)calloc(1, sizeof(struct ai));
	Player player = newPlayer(2, AI_PLAYER, b);
	Thoughts t = (Thoughts)calloc(1, sizeof(struct thought));

	ai->player = player;
	ai->t = t;

	ai->doAIMove = doAIMove;
	ai->findThreats = findThreats;
	ai->setStatus = setStatus;
	ai->getStatus = getStatus;

	return ai;
}

int doAIMove(AI ai) {
	int x, threatsFound;
	char temp[3];

	threatsFound = findThreats(ai, ai->t->threats);

	strcpy(ai->status, "");
	for(x = 0; x < 7; x++) {
		strcat(ai->status, itoa(ai->t->threats[x], temp, 10));
		strcat(ai->status, " ");
	}
	strcat(ai->status, "F: ");
	strcat(ai->status, itoa(threatsFound, temp, 10));

	return 0;
}

int findThreats(AI ai, int *columns) {
	int i, found = 0, open;
	Board b = ai->player->b;

	for(i = 0; i < 7; i++) {
		ai->t->parity[i] = NO_THREAT;
		open = getBottomMostSpace(b, i);

		if(open < 0) continue;
		if(testHorizontal(b, 1, open, i)) columns[i] += 1;
		if(testVertical(b, 1, open, i)) columns[i] += 2;
		if(testPosDiagonal(b, 1, open, i)) columns[i] += 4;
		if(testNegDiagonal(b, 1, open, i)) columns[i] += 8;
		if(columns[i] != 0) {
			setParity(ai, i, open);
			found++;
		}
	}

	return found;
}

void setStatus(AI ai, char *str) {
	strcpy(ai->status, str);
}

char * getStatus(AI ai) {
	return ai->status;
}

void destroyAI(AI ai) {
	free(ai->player);
	free(ai->t);
}

bool testHorizontal(Board b, int player, int x, int y) {
	int left, right, i;
	Space current = b->getSpace(b, x, y);

	if(current == NULL || !current->isSpaceEmpty(current)) return FALSE;

	for(i = 1, left = 0; i <= 3; i++) {
		current = b->getSpace(b, x, y-i);
		if(current == NULL) break;
		if(validateSpace(current, player)) break;
		left++;
	}
	for(i = 1, right = 0; i <= 3; i++) {
		current = b->getSpace(b, x, y+i);
		if(current == NULL) break;
		if(validateSpace(current, player)) break;
		right++;
	}

	/**
	 *	Possible horizontal combinations
	 *	(x = empty space under test, o = piece belonging to same player):
	 *	xooo	oxoo
	 *	
	 *	ooxo	ooox
	 */
	return (left+right >= 3);
}

bool testVertical(Board b, int player, int x, int y) {
	int down, i;
	Space current = b->getSpace(b, x, y);

	if(current == NULL || !current->isSpaceEmpty(current)) return FALSE;

	for(i = 1, down = 0; i <= 3; i++) {
		current = b->getSpace(b, x+i, y);
		if(current == NULL) break;
		if(validateSpace(current, player)) break;
		down++;
	}

	/**
	 *	Possible vertical combinations
	 *	(x = empty space under test, o = piece belonging to same player):
	 *	x
	 *	o
	 *	o
	 *	o
	 */
	return down >= 3;
}

bool testPosDiagonal(Board b, int player, int x, int y) {
	int ur, ll, i;
	Space current = b->getSpace(b, x, y);

	if(current == NULL || !current->isSpaceEmpty(current)) return FALSE;

	for(i = 1, ur = 0; i <= 3; i++) {
		current = b->getSpace(b, x-i, y+i);
		if(current == NULL) break;
		if(validateSpace(current, player)) break;
		ur++;
	}
	for(i = 1, ll = 0; i <= 3; i++) {
		current = b->getSpace(b, x+i, y-i);
		if(current == NULL) break;
		if(validateSpace(current, player)) break;
		ll++;
	}

	/**
	 *	Possible positive diagonal combinations
	 *	(x = empty space under test, o = piece belonging to same player
	 *	 ? = any piece):
	 *	   x    o    o    o
	 *	  o?   x?   o?   o?
	 *	 o??  o??  x??  o??
	 *	o??? o??? o??? x???
	 */
	 return (ur+ll) >= 3;
}

bool testNegDiagonal(Board b, int player, int x, int y) {
	int ul, lr, i;
	Space current = b->getSpace(b, x, y);

	if(current == NULL || !current->isSpaceEmpty(current)) return FALSE;

	for(i = 1, ul = 0; i <= 3; i++) {
		current = b->getSpace(b, x-i, y-i);
		if(current == NULL) break;
		if(validateSpace(current, player)) break;
		ul++;
	}
	for(i = 1, lr = 0; i <= 3; i++) {
		current = b->getSpace(b, x+i, y+i);
		if(current == NULL) break;
		if(validateSpace(current, player)) break;
		lr++;
	}

	/**
	 *	Possible positive diagonal combinations
	 *	(x = empty space under test, o = piece belonging to same player
	 *	 ? = any piece):
	 *	x    o    o    o
	 *	?o   ?x   ?o   ?o
	 *	??o  ??o  ??x  ??o
	 *	???o ???o ???o ???x
	 */
	 return (ul+lr) >= 3;
}

int getBottomMostSpace(Board b, int col) {
	int x, space = -1;
	Space current;

	for(x = 5; x >= 0; x--) {
		current = b->getSpace(b, x, col);
		if(current->isSpaceEmpty(current)) {
			space = x;
			break;
		};
	}

	return space;
}

#endif