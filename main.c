#include <stdio.h>
#include <conio.h>
#include "board.h"

int main() {
	Board board = newBoard();
	clrscr();
	board->draw(board, 10, 8);
	getch();
	return 0;
}