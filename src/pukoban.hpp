// Copyright (C) 2019 Yueh-Ting Chen (eopXD)
/*! \file weak_pukoban.hpp
    \brief game-related structure
	\author Yueh-Ting Chen (eopXD)
	\course Theory of Computer Game (TCG)
*/
#ifndef PUKOBAN_HPP
#define PUKOBAN_HPP

// C++
#include <cstdio>
#include <cstdlib>
#include <cassert>
// STL
#include <vector>

#define N_MAX 15
#define M_MAX 15
#define AREA_MAX 50

#define SZ(x) (int)((x).size())
typedef unsigned long long ULL;

enum _direction { UP, DOWN, LEFT, RIGHT, STAY };
typedef _direction DIRECTION;

/*
move	push	pull
u 		U 		^
d 		D 		v
l		L		<
r		R		>
*/
static char o_char[3][5] = { "udlr", "UDLR", "^v<>" };
struct _move {
	DIRECTION dir;
	bool drag; // dragging-bit
	bool pushed, pulled; // pulled/pushed in the move
	_move () { 
		drag = 0;
		pulled = pushed = 0;
		dir = STAY;
	}
	_move ( char c ) {
		drag = pushed = pulled = 0;
		if ( c == 'u' ) {
			dir = UP;	
		}
		else if ( c == 'd' ) {
			dir = DOWN;
		}
		else if ( c == 'l' ) {
			dir = LEFT;
		}
		else if ( c == 'r' ) {
			dir = RIGHT;
		}
		else if ( c == 'U' ) {
			dir = UP;	
			pushed = 1;
		}
		else if ( c == 'D' ) {
			dir = DOWN;
			pushed = 1;
		}
		else if ( c == 'L' ) {
			dir = LEFT;
			pushed = 1;
		}
		else if ( c == 'R' ) {
			dir = RIGHT;
			pushed = 1;
		}
		else if ( c == '^' ) {
			dir = UP;	
			pulled = drag = 1;
		}
		else if ( c == 'v' ) {
			dir = DOWN;
			pulled = drag = 1;
		}
		else if ( c == '<' ) {
			dir = LEFT;
			pulled = drag = 1;
		}
		else if ( c == '>' ) {
			dir = RIGHT;
			pulled = drag = 1;
		}
		else {
			/* wrong character inserted */ 
			drag = 0;
			pulled = pushed = 0;
			dir = STAY;
			fprintf(stderr, "illegal character when initializing\n");
			exit(1);
		}
	}
	void display () {
		fprintf(stdout, "move: %c, drag: %d\n", o_char[0][dir], drag);
		fprintf(stdout, "pulled: %d, pushed: %d\n", pulled, pushed);
	}
    char to_char() {
        int o = pushed+(pulled<<1);
        assert(o < 3);
        return o_char[o][dir];
    }
};
typedef _move MOVE;
typedef std::vector<MOVE> HISTORY;

struct _board { // information of a board
	int n, m;
	char initial[N_MAX+5][M_MAX+5];
	char state[N_MAX+5][M_MAX+5];
	int px, py; // player
	HISTORY moves;
	_board () {}
	void construct () {
		assert(n<=N_MAX and m<=M_MAX and n*m<=AREA_MAX);
		for ( int i=0; i<n; ++i ) {
			for ( int j=0; j<m; ++j ) {
				state[i][j] = initial[i][j];
				if ( state[i][j]=='@' or state[i][j]=='+' ) {
					px = i, py = j;
				}
			}
		}
	}
	void reset () {
		construct();		
	}
	void move_player ( int x, int y ) {
		assert(x>=0 and y>=0 and x<n and y<m);
		if ( state[px][py] == '+' ) {
			state[px][py] = '.';
		}
		if ( state[px][py] == '@' ) {
			state[px][py] = '-';
		}
		px = x, py = y;
		if ( state[px][py] == '-' ) {
			state[px][py] = '@';
		}
		if ( state[px][py] == '.' ) {
			state[px][py] = '+';
		}
	}
	void move_box ( int sx, int sy, int ex, int ey ) {
		assert(sx>=0 and sy>=0 and sx<n and sy<m);
		assert(ex>=0 and ey>=0 and ex<n and ey<m);
		if ( state[sx][sy] == '*' ) {
			state[sx][sy] = '.';
		}
		if ( state[sx][sy] == '$' ) {
			state[sx][sy] = '-';
		}
		if ( state[ex][ey] == '.' ) {
			state[ex][ey] = '*';
		}
		if ( state[ex][ey] == '-' ) {
			state[ex][ey] = '$';
		}
	}

	bool wall ( int x, int  y) {
		assert(x>=0 and y>=0 and x<n and y<m);
		return (state[x][y] == '#');

	}
	bool goal ( int x, int y) {
		assert(x>=0 and y>=0 and x<n and y<m);
		return (state[x][y]=='.' or state[x][y]=='*' or state[x][y]=='+');
	}
	bool occupied ( int x, int y ) { // if (x,y) is occupied by box
		assert(x>=0 and y>=0 and x<n and y<m);
		return (state[x][y]=='$' or state[x][y]=='*');
	}
	void player ( int *player_x, int *player_y ) { // write-back
		*(player_x) = px;
		*(player_y) = py;
	}
	bool solved () {
		for ( int i=0; i<n; ++i ) {
			for ( int  j=0; j<m; ++j ) {
				if ( state[i][j]=='.' or state[i][j]=='+' ) {
					return (0);
				}

			}
		}
		return (1);
	}
	void display_moves () {
		if ( !SZ(moves) ) {
			fprintf(stdout, "\033[1mNo history yet ~ OAO\033[m\n");
			return ;
		}
		fprintf(stdout, "\033[1m");
		for ( int i=0; i<SZ(moves); ++i ) {
			MOVE m = moves[i];
			int o = m.pushed + (m.pulled<<1);
			assert(o<3); // pushed and pulled don't co-exist
			fprintf(stdout, "%c", o_char[o][m.dir]);
		}
		fprintf(stdout, "\033[m\n");
	}
	void display_board () {
		for ( int i=0; i<n; ++i ) {
			for ( int j=0; j<m; ++j ) {
				switch ( state[i][j] ) {
					case '#' :
						fprintf(stdout, "\033[1;31;41m#\033[m"); break;
					case '*' :
						fprintf(stdout, "\033[1;33;42m*\033[m"); break;
					case '.' :
						fprintf(stdout, "\033[1;30;42m.\033[m"); break;
					case '+' :
						fprintf(stdout, "\033[1;35;42m+\033[m"); break;
					case '-' :
						fprintf(stdout, "-"); break;
					case '$' :
						fprintf(stdout, "\033[1;33m$\033[m"); break;
					case '@' :
						fprintf(stdout, "\033[1;35m@\033[m"); break;
				}
			}
			fprintf(stdout, "\n");
		}
	}
};
typedef _board BOARD;

extern bool get_board ( FILE *f_in, BOARD &b );
extern bool out ( BOARD &b, int x, int y );
extern bool do_move ( BOARD &b, MOVE &m );
extern bool undo_move ( BOARD &b );

extern const int dx[];
extern const int dy[];

#endif
