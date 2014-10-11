#ifndef __board_h
#define __board_h

#include <stdio.h>
#include <curses.h>
#include <stdlib.h>
#include "space.h"

struct board {
	Space	 **spaces;
	bool	(*isBoardEmpty)(struct board *);
	Space	(*getSpace)(struct board *, int, int);
	bool	(*isColumnFull)(struct board *, int);
	void	(*dropPiece)(struct board *, int, int);
	void	(*draw)(WINDOW *, struct board *, int, int);
	bool	(*checkForWin)(struct board *, int);
	void	(*destroyBoard)(struct board *);
	unsigned long long int
			(*getBitBoard)(struct board *, int);
};

typedef struct board *Board;

Board	newBoard();
bool	isBoardEmpty(Board);
Space	getSpace(Board, int, int);
bool	isColumnFull(Board, int);
void	dropPiece(Board, int, int);
void	draw(WINDOW *, Board, int, int);
bool	checkForWin(Board, int);
void	destroyBoard(Board);
unsigned long long int
		getBitBoard(Board, int);

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
	board->isColumnFull = isColumnFull;
	board->dropPiece = dropPiece;
	board->draw = draw;
	board->checkForWin = checkForWin;
	board->destroyBoard = destroyBoard;
	board->getBitBoard = getBitBoard;

	return board;
}

bool isBoardEmpty(Board b) {
	bool empty = TRUE;
	Space current;
	int x, y;

	for(x = 5; x >= 0; x--) {
		for(y = 6; y >= 0; y--) {
			current = b->getSpace(b, x, y);
			if(!current->isSpaceEmpty(current)) return FALSE;
		}
	}

	return TRUE;
}

Space getSpace(Board b, int x, int y) {
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
	int i, j, temp;
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
			if(current->hasWinningTile(current)) wattron(win, A_BLINK);
			wattron(win, COLOR_PAIR(current->getOwner(current)+2));
			wmove(win, offsetY+i+1, offsetX+(j*2)+1); clrtoeol(); addch(' ');
			wattroff(win, COLOR_PAIR(current->getOwner(current)+2));
			if(current->hasWinningTile(current)) wattroff(win, A_BLINK);
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
 *	Board checking algorithm by Christian Ammer 
 *	http://stackoverflow.com/a/7053051
 *
 *	Full code @ GitHub
 *	
 */
bool checkForWin(Board b, int player) {
	unsigned long long int bitboard = b->getBitBoard(b, player);
	bitboard &= bitboard >> 6;

	if (bitboard & (bitboard >> 2 * 6))     // check \ diagonal
        return TRUE;
    bitboard &= bitboard >> 7;
    if (bitboard & (bitboard >> 2 * 7))     // check horizontal
        return TRUE;
    bitboard &= bitboard >> 8;
    if (bitboard & (bitboard >> 2 * 8))     // check / diagonal
        return TRUE;
    bitboard &= bitboard >> 1;
    if (bitboard & (bitboard >> 2))         // check vertical
        return TRUE;
    return FALSE;
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

unsigned long long int getBitBoard(Board b, int player) {
	unsigned long long int bitboard = 0;
	int x, y, count = 0;
	Space current;

	for(y = 0; x < 7; y++) {
		for(x = 5; y >= 0; x--) {
			current = b->getSpace(b, x, y);
			if(current->getOwner(current) == player) {
				bitboard |= 1 >> count;
			}
			count++;
		}
		count++;
	}
}

#endif