#ifndef __space_h
#define __space_h

#include <stdlib.h>
#include "boolean.h"

struct space {
	char c;
	Boolean (*isEmpty)(struct space *);
	void (*put)(struct space *, char);
	char (*get)(struct space *);
};

typedef struct space *Space;

Space	newSpace();
Boolean	isEmpty(Space);
void	put(Space, char);
char	get(Space);

Space newSpace() {
	Space space = (Space)calloc(1, sizeof(struct space));
	space->c = ' ';

	space->isEmpty = isEmpty;
	space->put = put;
	space->get = get;

	return space;
}

Boolean isEmpty(Space s) {
	return s->c == ' ';
}

void put(Space s, char c) {
	s->c = c;
}

char get(Space s) {
	return s->c;
}

#endif