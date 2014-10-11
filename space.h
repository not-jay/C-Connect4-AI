#ifndef __space_h
#define __space_h

#include <stdlib.h>

struct space {
	int		owner;
	bool	winningTile;
	bool	(*isSpaceEmpty)(struct space *);
	void	(*isWinningTile)(struct space *, bool);
	bool	(*hasWinningTile)(struct space *);
	void	(*put)(struct space *, int);
	int		(*getOwner)(struct space *);
	void	(*destroySpace)(struct space *);
};

typedef struct space *Space;

Space	newSpace();
bool	isSpaceEmpty(Space);
void	isWinningTile(Space, bool);
bool	hasWinningTile(Space);
void	put(Space, int);
int		getOwner(Space);
void	destroySpace(Space);

Space newSpace() {
	Space space = (Space)calloc(1, sizeof(struct space));
	space->owner = -1;
	space->winningTile = FALSE;

	space->isSpaceEmpty = isSpaceEmpty;
	space->isWinningTile = isWinningTile;
	space->hasWinningTile = hasWinningTile;
	space->put = put;
	space->getOwner = getOwner;
	space->destroySpace = destroySpace;

	return space;
}

bool isSpaceEmpty(Space s) {
	return s->owner == -1;
}

void isWinningTile(Space s, bool isWinning) {
	s->winningTile = isWinning;
}

bool hasWinningTile(Space s) {
	return s->winningTile;
}

void put(Space s, int player) {
	s->owner = player;
}

int getOwner(Space s) {
	return s->owner;
}

void destroySpace(Space s) {
	free(s);
}

#endif