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

	max_time = time_left;

	switch (current_level)
	{
		case 1: max_time /= 10000000; max_depth = 1; break;
		case 2: max_time /= 10000; max_depth = 1; break;
		case 3: max_time /= 100; max_depth = 2; break;
		case 4: max_time /= 25; max_depth = 2; break;
		case 5: max_time /= 10; max_depth = 3; break;
		case 6: max_time /= 5; max_depth = 4; break;
		case 7: max_time /= 3; max_depth = 5; break;
		case 8: max_time /= 2; max_depth = 7; break;
		case 9: max_time /= 1.5; max_depth = 10; break;
		default: max_depth = 20; break;
	}
	
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

	playcount = 0, validcount = 0, legalcount = 0, captcount = 0;
	pvcount = 0;
	hits = 0;
	hashcount = 0;

	return square;
}

SharedAPI::SharedAPI()
{
}


SharedAPI::~SharedAPI()
{
}
