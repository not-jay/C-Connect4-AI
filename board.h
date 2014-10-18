#ifndef __board_h
#define __board_h

#include <stdio.h>
#include <curses.h>
#include <stdlib.h>
#include <stdint.h>
#include "space.h"

struct board {
	Space		 **spaces;
	bool		(*isBoardFull)(struct board *);
	Space		(*getSpace)(struct board *, int, int);
	bool		(*isColumnFull)(struct board *, int);
	void		(*dropPiece)(struct board *, int, int);
	void		(*draw)(WINDOW *, struct board *, int, int);
	bool		(*checkForWin)(struct board *, int);
	void		(*destroyBoard)(struct board *);
	uint64_t	(*getBitBoard)(struct board *, int);
};

typedef struct board *Board;

Board		newBoard();
bool		isBoardFull(Board);
Space		getSpace(Board, int, int);
bool		isColumnFull(Board, int);
void		dropPiece(Board, int, int);
void		draw(WINDOW *, Board, int, int);
bool		checkForWin(Board, int);
void		destroyBoard(Board);
uint64_t	getBitBoard(Board, int);

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

	board->isBoardFull = isBoardFull;
	board->getSpace = getSpace;
	board->isColumnFull = isColumnFull;
	board->dropPiece = dropPiece;
	board->draw = draw;
	board->checkForWin = checkForWin;
	board->destroyBoard = destroyBoard;
	board->getBitBoard = getBitBoard;

	return board;
}

bool isBoardFull(Board b) {
	int x;

	for(x = 0; x < 7; x++) {
		if(!isColumnFull(b, x)) return FALSE;
	}

	return TRUE;
}

Space getSpace(Board b, int x, int y) {
	if(x < 0 || x > 5 || y < 0 || y > 6) return NULL;
	return b->spaces[x][y];
}

bool isColumnFull(Board b, int col) {
	int x;
	Space current;

	if(col < 0 || col > 6) return FALSE;
	for(x = 5; x >= 0; x--) {
		current = b->getSpace(b, x, col);
		if(current->isSpaceEmpty(current)) return FALSE;
	}

	return TRUE;
}

void dropPiece(Board b, int player, int col) {
	int x;
	Space current;

	for(x = 5; x >= 0; x--) {
		current = b->getSpace(b, x, col);
		if(current->isSpaceEmpty(current)) {
			current->put(current, player);
			break;
		}
	}
}

/* Here be dragons */
void draw(WINDOW * win, Board b, int offsetY, int offsetX) {
	int i, j;
	Space current;

	wmove(win, offsetY, offsetX); clrtoeol();
	addch(ACS_ULCORNER); addch(ACS_HLINE); addch(ACS_TTEE); addch(ACS_HLINE);
	addch(ACS_TTEE); addch(ACS_HLINE); addch(ACS_TTEE); addch(ACS_HLINE); addch(ACS_TTEE);
	addch(ACS_HLINE); addch(ACS_TTEE); addch(ACS_HLINE); addch(ACS_TTEE); addch(ACS_HLINE);
	addch(ACS_URCORNER);
	for(i = 0; i < 6; i++) {
		wmove(win, offsetY+i+1, offsetX); clrtoeol(); addch(ACS_VLINE);
		for(j = 0; j < 7; j++) {
			current = b->getSpace(b, i, j);
			wattron(win, COLOR_PAIR(current->getOwner(current)+2));
			wmove(win, offsetY+i+1, offsetX+(j*2)+1); clrtoeol(); addch(' ');
			wattroff(win, COLOR_PAIR(current->getOwner(current)+2));
			addch(ACS_VLINE);
		}
	}
	wmove(win, offsetY+i+1, offsetX); clrtoeol();
	addch(ACS_LLCORNER); addch(ACS_HLINE); addch(ACS_BTEE); addch(ACS_HLINE);
	addch(ACS_BTEE); addch(ACS_HLINE); addch(ACS_BTEE); addch(ACS_HLINE); addch(ACS_BTEE);
	addch(ACS_HLINE); addch(ACS_BTEE); addch(ACS_HLINE); addch(ACS_BTEE); addch(ACS_HLINE);
	addch(ACS_LRCORNER);
	wrefresh(win);
}

/**
 *	Board checking algorithm by John Tromp
 *	https://github.com/qu1j0t3/fhourstones/blob/master/Game.c
 *	
 */
bool checkForWin(Board b, int player) {
	uint64_t bitboard = b->getBitBoard(b, player);
	
	uint64_t diag1 = bitboard & (bitboard >> 6);
	uint64_t hori = bitboard & (bitboard >> 7);
	uint64_t diag2 = bitboard & (bitboard >> 8);
	uint64_t vert = bitboard & (bitboard >> 1);
	return ((diag1 & (diag1 >> 12)) |
			(hori & (hori >> 14)) |
			(diag2 & (diag2 >> 16)) |
			(vert & (vert >> 2))) != 0;
}

void destroyBoard(Board b) {
	Space current;
	int x, y;

	for(x = 0; x < 6; x++) {
		for(y = 0; y < 7; y++) {
			current = b->getSpace(b, x, y);
			current->destroySpace(current);
			b->spaces[x][y] = NULL;
		}
		free(b->spaces[x]);
		b->spaces[x] = NULL;
	}

	free(b->spaces);
	b->spaces = NULL;
}

uint64_t getBitBoard(Board b, int player) {
	uint64_t bitboard = 0;
	int x, y, count = 0;
	Space current;

	for(y = 0; y < 7; y++) {
		for(x = 5; x >= 0; x--) {
			current = b->getSpace(b, x, y);
			if(current->getOwner(current) == player) {
				bitboard |= (uint64_t)1 << count;
			}
			count++;
		}
		count++;
	}

	return bitboard;
}

#endif