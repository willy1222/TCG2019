// Copyright (C) 2019 Yueh-Ting Chen (eopXD)
/*! \file game.cpp
\brief main program that runs the game
\author Yueh-Ting Chen (eopXD)
\course Theory of Computer Game (TCG)
*/
// C++
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <cctype>
// eopXD
#include "getch.hpp"
#include "pukoban.hpp"

int main ( int argc, char **argv ) {
/* argument parsing */
#define PUKOBAN_WRONG_ARG do{\
fprintf(stdout, "Usage: pukoban [-i file] [-o file] [-s start_stage]\n");\
fprintf(stdout, " -i filename: read the puzzles from \'file\'.\n");\
fprintf(stdout, " -o filename: if specified, write the solutions to\
\'file\'.\n");\
fprintf(stdout, " -s stage: if specified, start from Stage \'stage\'.\n");\
return (1);\
}while(0)
char *input_file = NULL, *output_file = NULL;
int start_stage = 0;
for ( int i=1; i<argc; ++i ) {
	if ( strcmp(argv[i], "-i") == 0 ) {
		++i;
		if ( i==argc or input_file ) {
			PUKOBAN_WRONG_ARG;
		}
		input_file = argv[i];
	}
	else if ( strcmp(argv[i], "-o") == 0 ) {
		++i;
		if ( i==argc or output_file ) {
			PUKOBAN_WRONG_ARG;
		}
		output_file = argv[i];
	}
	else if ( strcmp(argv[i], "-s") == 0 ) {
		++i;
		if ( i==argc or start_stage ) {
			PUKOBAN_WRONG_ARG;
		}
		start_stage = atoi(argv[i]);
		if ( start_stage <= 0 ) {
			PUKOBAN_WRONG_ARG;
		}
	}
	else {
		PUKOBAN_WRONG_ARG;
	}
}
if ( !input_file ) {
	PUKOBAN_WRONG_ARG;
}
if ( !start_stage ) {
	start_stage = 1;
}
#undef PUKOBAN_WRONG_ARG
/* check file existence */ 
FILE *f_in = fopen(input_file, "r");
if ( !f_in ) {
	fprintf(stderr, "Error in opening the file \'%s\'\n", input_file);
	return (1);
}
FILE *f_out = fopen(output_file, "w");
if ( output_file and !f_out ) {
	fprintf(stderr, "Error in opening the file \'%s\'\n", output_file);
	return (1);
}

BOARD b;

#define CLEAR_SCREEN fprintf(stdout, "\033[H\033[J")
#define CLEAR_LINE(line) for(int i=0;i<line; ++i) { fprintf(stdout, "\033[1F\033[2K"); }

#define PUKOBAN_HEADER do{\
fprintf(stdout, "\033[1;36mLegend:\n\n");\
fprintf(stdout, "\033[1;31;41m#\033[m\tWall\n\n");\
fprintf(stdout, "\033[1;35m@\033[m\tPlayer\n\n");\
fprintf(stdout, "\033[1;35;42m+\033[m\tPlayer on Goal Square\n\n");\
fprintf(stdout, "\033[1;33m$\033[m\tBox\n\n");\
fprintf(stdout, "\033[1;33;42m*\033[m\tBox on Goal Square\n\n");\
fprintf(stdout, "\033[1;30;42m.\033[m\tGoal Square\n\n");\
fprintf(stdout, "-\tFloor\n\n");\
fprintf(stdout, "\033[1;37;41mD\033[m\tDrag Activated\n\n");\
fprintf(stdout, "Use \033[1;35mW\033[m, \033[1;35mA\033[m, \033[1;35mS\
\033[m, \033[1;35mD\033[m, or \033[1;35marrow keys\033[m to move.\n");\
fprintf(stdout, "Press \033[1;35mQ\033[m to drag box.\n");\
fprintf(stdout, "Press \033[1;35mP\033[m to pause.\n");\
fprintf(stdout, "Press \033[1;35mU\033[m to undo.\n\n");\
}while(0)
#define PUKOBAN_OUTPUT_SOLUTION do{\
fprintf(f_out, "%d\n", SZ(b.moves));\
for ( int i=0; i<SZ(b.moves); ++i ) {\
	int o = b.moves[i].pushed+(b.moves[i].pulled<<1);\
	assert(o < 3);\
	fprintf(f_out, "%c", o_char[o][b.moves[i].dir]);\
}\
fprintf(f_out, "\n");\
}while(0)
#define BANG do{\
fprintf(stdout, "\033[1;32mThanks for Playing! > <\033[m\n");\
if ( f_in ) {\
	fclose(f_in);\
}\
if ( f_out ) {\
	fclose(f_out);\
}\
return (0);\
}while(0)
/*
possible idea: 
- activating drag counts as a step
- cannot drag and push at the same time

*/
for ( int stage=1; get_board(f_in, b); ++stage ) {
	bool drag = 0; // dragging- bit
	bool undo = 0;
	bool moved = 0;
	if ( stage < start_stage ) {
		goto new_stage;
	}
	while ( 1 ) {
	/* display */
		CLEAR_SCREEN;
		PUKOBAN_HEADER;		
		printf("\033[1;36mStage #%d:\033[m \n", stage);
		if ( drag ) {
			fprintf(stdout, "\t\t\033[1;37;41mD\033[m\n");
		}
		else {
			fprintf(stdout, "\n");
		}			
		
		b.display_board();
		fprintf(stdout, "\nMove Count: \033[1m%3d\033[m\n", SZ(b.moves));
		fprintf(stdout, "Moves: "); b.display_moves();
		fprintf(stdout, "\n");
		if ( b.solved() ) {
			break;
		}
	/* read keystroke */
		char cmd = tolower(getch());
		MOVE m;
		switch ( cmd ) {
			case 27 : // ESC character
				getch();
				cmd = getch();
				switch ( cmd ) { // handling arrow key
					case 'A':
						m.dir = UP; break;
					case 'B':
						m.dir = DOWN; break;
					case 'C':
						m.dir = RIGHT; break;
					case 'D':
						m.dir = LEFT; break;								
				}
				break;
			case 'w' :
				m.dir = UP; break;
			case 'a' :
				m.dir = LEFT; break;		
			case 's' :
				m.dir = DOWN; break;
			case 'd' :
				m.dir = RIGHT; break;
			case 'q' : // flip dragging-bit
				drag = !drag; break;
			case 'u' : // undo
				undo = 1; break;
			case 'p' : // pause
				fprintf(stdout, "re(\033[1;35mS\033[m)ume\n");
				fprintf(stdout, "res(\033[1;35mT\033[m)art\n");
				fprintf(stdout, "(\033[1;35mN\033[m)ext Stage\n");
				fprintf(stdout, "(\033[1;35mQ\033[m)uit the Game\n");
				fflush(stdout);
				cmd = tolower(getch());
				while ( !strchr("stnq", cmd) ) {
					fprintf(stdout, "\a");
					cmd = tolower(getch());
				}
				switch ( cmd ) {
					case 's' : // re(S)ume
						for ( int i=0; i<5; ++i ) { // clear lines
							fprintf(stdout, "\033[1F\033[2K");
						}
						fflush(stdout);
						break;
					case 't' : // res(T)art
						b.moves.clear();
						b.construct(); // reconstruct initial board
						break;
					case 'n' : // (N)ext Stage
						goto new_stage;
						break;
					case 'q' : // (Q)uit
						BANG;
						break;
				}
				break;
			default :
				fprintf(stdout, "\a");
				m.dir = STAY;
				break;
		}
		m.drag = drag;
		if ( undo ) {
			moved = undo_move(b);
		}
		if ( m.dir != STAY ) { // do_move
			moved = do_move(b, m);
		}
	/* reset move */
		undo = moved = 0;
	}
	fprintf(stdout, "\033[1;33mStage Clear!\033[m\n");
	fprintf(stdout, "\033[1;33mPress any key to continue\033[m\n");
	getchar();
	if ( f_out ) {
		PUKOBAN_OUTPUT_SOLUTION;
	}
new_stage:
	b.moves.clear(); // clear history
}
BANG;
#undef PUKOBAN_OUTPUT_SOLUTION
#undef PUKOBAN_HEADER
#undef CLEAR_SCREEN
return (0);
}