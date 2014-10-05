#ifndef __board_h
#define __board_h

#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include "space.h"

struct board {
	Space **spaces;
	Space (*getSpace)(struct board *, int, int);
	void (*draw)(struct board *, int, int);
};

typedef struct board *Board;

Board	newBoard();
Space	getSpace(Board, int, int);
void	draw(Board, int, int);

Board newBoard() {
	Board board = (Board)calloc(1, sizeof(struct board));
	int i, j;

	board->spaces = (Space **)calloc(6, sizeof(Space *));
	for(i = 0; i < 6; i++) {
		board->spaces[i] = (Space *)calloc(7, sizeof(Space));
		for(j = 0; j < 7; j++) {
			board->spaces[i][j] = newSpace();
		}
	}

	board->getSpace = getSpace;
	board->draw = draw;

	return board;
}

Space getSpace(Board b, int x, int y) {
	return b->spaces[x][y];
}

void draw(Board b, int offsetX, int offsetY) {
	int i, j;
	Space current;

	gotoxy(offsetX, offsetY);
	printf("+=+=+=+=+=+=+=+");
	for(i = 0; i < 6; i++) {
		gotoxy(offsetX, offsetY+(i*2)+1);
		printf("|");
		for(j = 0; j < 7; j++) {
			current = b->getSpace(b, i, j);
			gotoxy(offsetX+(j*2)+1, offsetY+i+1);
			printf("%c|", current->get(current));
		}
		gotoxy(offsetX, offsetY+i+2);
		printf("+=+=+=+=+=+=+=+");
	}
}

#endif