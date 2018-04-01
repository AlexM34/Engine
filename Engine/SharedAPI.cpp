#include "SharedAPI.h"
// CGT.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <windows.h>
#include "defs.h"
#include "data.h"
#include "protos.h"
#include <stdio.h>
#include <io.h>
#include <dos.h>

MYSHARED_API int Move(int move, double time_left, int current_level)
{
	int m = 0;
	char input;
		
	if (move == -666)
	{
		takeback();
		moves--;
		return 0;
	}

	else if (move == 0)
	{
		increment = 0;

		/*for (double i = 0; i < 200; i += 2)
		{
		thinkingTime();
		max_time -= (stop_time - start_time) - increment;
		ply += 2;
		}*/

		initialise();
		init_hash();
		open_book();

		set_hash(sideToMove);
		path[0] = hashing;
		reps[0] = 1;
		repcount = 1;

		return 0;
	}

	if (ply - fifty >= 100)
	{
		return -100;
	}

	for (int i = 0; i < repcount; i++)
	{
		if (reps[i] >= 3)
		{
			return -100;
		}
	}

	if (current_level == -1)
	{
		bool legal = play(move / 100, move % 100);
		if (legal)
		{
			takeback();
			return 1;
		}

		else return 0;
	}

	if (time_left < 0)
	{
		bool legal = play(move / 100, move % 100);
		moves++;
		computer = 1 - computer;
		return 0;
	}

	max_time = time_left * current_level / 10;
	max_depth = current_level;
	
	if (move == -6 || move == -16) computer = WHITE;
	else if (move == -7 || move == -17) computer = BLACK;

	else if (move > 0)
	{
		bool legal = play(move / 100, move % 100);
		moves++;
	}

	legalmoves(sideToMove);
	if (possible[0][0] == -1)
	{
		if (checked(sideToMove)) return -101 + 2 * sideToMove;
		else return -100;
	}

	if (move == -5) return -10;		// game continues

	m = compmove(sideToMove, max_depth);
	moves++;

	if (move == -16 || move == -17)
	{
		takeback();
		moves--;
		computer = 1 - computer;
		return m;
	}

	return m;
	
	//close_book();
}

int parse(char input[64])
{
	if (input[0] == 'p') return -2;
	if (input[0] == 'x') return -3;
	if (input[0] == 'n') return -4;

	if (input[0] < 'a' || input[0] > 'h' || input[1] < '0' || input[1] > '9' ||
		input[2] < 'a' || input[2] > 'h' || input[3] < '0' || input[3] > '9')
	{
		return -1;
	}

	int from = input[0] - 'a';
	from += 8 * (8 - (input[1] - '0'));
	int to = input[2] - 'a';
	to += 8 * (8 - (input[3] - '0'));
	int piece_from = piece[from];

	if (!play(from, to)) return -1;
	else if (piece_from != piece[to])
	{
		if (input[4] == 'n') piece[to] = KNIGHT;
		else if (input[4] == 'b') piece[to] = BISHOP;
		else if (input[4] == 'r') piece[to] = ROOK;
	}

	last[ply - 1] = 100 * from + to;

	return 1;
}

char* notation(int m)
{
	char* str = new char[6];
	char c;

	if (false) {
		switch (m) {
		case KNIGHT:
			c = 'n';
			break;
		case BISHOP:
			c = 'b';
			break;
		case ROOK:
			c = 'r';
			break;
		default:
			c = 'q';
			break;
		}
		sprintf(str, "%c%d%c%d%c",
			file8(m / 100) + 'a',
			8 - rank8(m / 100),
			file8(m % 100) + 'a',
			8 - rank8(m % 100),
			c);
	}
	else
	{
		sprintf(str, "%c%c%c%c",
			file8(m / 100) + 'a',
			8 - rank8(m / 100) + '0',
			file8(m % 100) + 'a',
			8 - rank8(m % 100) + '0');
	}
	return str;
}

int compmove(int player, int d)
{
	for (int i = 0; i < repcount; i++)
	{
		//printf("%d. %d with count %d\n", i, hashes[i], reps[i]);
		if (reps[i] >= 3)
		{
			return -100;
		}
	}

	zeitnot = false;
	distToRoot = 0;
	for (int i = 0; i < 100; i++) dcount[i] = 0;
	thinkingTime();
	int square = think(d);
	int elapsed = clock() - start_time;
	max_time -= elapsed - increment;

	/*if (max_time < 0)
	{
		finish(4 * sideToMove - 2);
		return 0;
	}*/

	if (square == -12345 || square == 12345 || square == 0)
	{
		return -100 + (square / 12345);
	}

	if (ply - fifty >= 100)
	{
		return -100;
	}

	int from = square / 100;
	int to = square % 100;
	last[ply] = square;

	bool legal = play(from, to);
	if (!legal) printf("FALSEEEEEE\n");
	char comp_move[4];
	comp_move[0] = (char)(from % 8 + 'a');
	comp_move[1] = (char)((8 - from / 8) + '0');
	comp_move[2] = (char)(to % 8 + 'a');
	comp_move[3] = (char)((8 - to / 8) + '0');

	printf("play %d, valid %d, legal %d, capt %d\n", playcount, validcount, legalcount, captcount);
	playcount = 0, validcount = 0, legalcount = 0, captcount = 0;

	for (int i = 0; i < 10; i++) printf("%d. %d\n", i, dcount[i]);
	printf("\nPVcount: %d\n", pvcount);
	printf("hits: %d\n 1. %d 2. %d 3. %d\n", hits, t1, t2, t3);
	printf("The evaluation is %d and square is %d\n", evaluation, square);
	pvcount = 0;
	hits = 0;
	hashcount = 0;

	printf("ELAPSED: %d, REMAINING: %d\n", elapsed, max_time);
	printf("Computer's move: %d. %c%c%c%c %d %d\n", ply, comp_move[0], comp_move[1], comp_move[2], comp_move[3], (1 - 2 * sideToMove) * capturing(sideToMove), hashcount);
	return square;
}

int humanmove(int player)
{
	for (int i = 0; i < repcount; i++)
	{
		//printf("%d. %d with count %d\n", i, hashes[i], reps[i]);
		if (reps[i] >= 3)
		{
			finish(0);
			return 0;
		}
	}

	legalmoves(player);
	int m = -1;
	char input[64];
	if (possible[0][1] == -1)
	{
		if (checked(player)) finish(2 * player - 1);
		else finish(0);
		return 0;
	}

	if (ply - fifty >= 100)
	{
		finish(0);
		return 0;
	}

	while (m != 1)
	{
		printf("Your move: ");
		scanf("%64s", input);
		m = parse(input);
		if (m == -2) printf("The evaluation is %d\n", position());
		if (m == -3) printf("The capturing is %d\n", capturing(player * (1 - 2 * sideToMove)));
		if (m == -4) return 2;
	}

	return 1;
}

void print()
{
	printf("\n8 ");
	for (int i = 0; i < 64; i++)
	{
		if (color[i] == EMPTY) printf(" .");

		else if (color[i] == WHITE) printf(" %c", piece_letter[piece[i]]);

		else printf(" %c", piece_letter[piece[i]] + ('a' - 'A'));

		if ((i + 1) % 8 == 0 && i != 63) printf("\n%d ", 7 - i / 8);
	}
	printf("\n\n   a b c d e f g h\n\n");
}

SharedAPI::SharedAPI()
{
}


SharedAPI::~SharedAPI()
{
}
