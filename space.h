#ifndef __space_h
#define __space_h

#include <stdlib.h>

struct space {
	int		owner;
	bool	(*isSpaceEmpty)(struct space *);
	void	(*put)(struct space *, int);
	int		(*getOwner)(struct space *);
	void	(*destroySpace)(struct space *);
};

typedef struct space *Space;

Space	newSpace();
bool	isSpaceEmpty(Space);
void	put(Space, int);
int		getOwner(Space);
void	destroySpace(Space);

Space newSpace() {
	Space space = (Space)calloc(1, sizeof(struct space));
	space->owner = -1;

	space->isSpaceEmpty = isSpaceEmpty;
	space->put = put;
	space->getOwner = getOwner;
	space->destroySpace = destroySpace;

	return space;
}

bool isSpaceEmpty(Space s) {
	return s->owner == -1;
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