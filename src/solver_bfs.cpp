// Try bfs solver, modified from game.cpp 
// by Guan-Yu Chen

#include<cstdio>
#include<cstdlib>
#include<cstring>
#include<ctype.h>
#include<queue>

// by eopXD
#include"pukoban.hpp"

// If the format of the input file (puzzle) is invalid, the program terminates.
void Input_verifier(FILE *fin){
#define INVALID_INPUT do{\
    puts("The input file is invalid.");\
    exit(1);\
}while(0)
char s[N_MAX][M_MAX+2];
while(fgets(s[0], sizeof(s[0]), fin)){
    int len = strlen(s[0]);
    if(len>5 || s[0][len-1]!='\n'){
        INVALID_INPUT;
    }
    s[0][--len] = 0;
    char *sp = strchr(s[0], ' ');
    if(!sp || strchr(sp+1, ' ')) {
        INVALID_INPUT;
    }
    int n = 0;
    for(char *it=s[0]; it!=sp; ++it){
        if(!isdigit(*it) || (it==s[0]&&*it=='0')) {
            INVALID_INPUT;
        }
        n = 10*n+(*it)-48;
    }
    int m = 0;
    for(char *it=sp+1; *it; ++it){
        if(!isdigit(*it) || (it==sp+1&&*it=='0')) {
            INVALID_INPUT;
        }
        m = 10*m+(*it)-48;
    }
    if(n > N_MAX || m > M_MAX || n*m > AREA_MAX) {
        INVALID_INPUT;
    }
    for(int i=0; i<=n-1; ++i){
        if(!fgets(s[i], sizeof(s[i]), fin)) INVALID_INPUT;
        if((int)strlen(s[i])!=m+1 || s[i][m]!='\n') {
            INVALID_INPUT;
        }
        for(int j=0; j<=m-1; ++j){
            if(!strchr("#@+$*.-", s[i][j])) {
                INVALID_INPUT;
            }
        }
    }
    int bcnt=0, gcnt=0, pcnt=0;
    for(int i=0; i<=n-1; ++i) for(int j=0; j<=m-1; ++j){
        bcnt += (s[i][j] == '$');
        gcnt += (s[i][j] == '.' || s[i][j] == '+');
        pcnt += (s[i][j] == '@' || s[i][j] == '+');
    }
    if(!bcnt || bcnt!=gcnt || pcnt!=1) {
        INVALID_INPUT;
    }
}
}

extern const int dx[];
extern const int dy[];
static char inv_o_char[3][5] = { "durl", "v^><", "DURL" };

void feasible_moves(BOARD& b, MOVE* moves){
    int i = 0;
    int x, y;
    b.player(&x, &y);
    char last_move = '\0';
    if (!b.moves.empty()){
        last_move = b.moves.back().to_char();
    }
    for (int dir = 0; dir < 4; ++dir) {
        int o = 0;
        int xx = x+dx[dir];
        int yy = y+dy[dir];
        int _x = x-dx[dir];
        int _y = y-dy[dir];
        int xxx = xx+dx[dir];
        int yyy = yy+dy[dir];
        if ( out(b, xx, yy) or b.wall(xx, yy)) { // (xx, yy) not available
            continue;
        }
        else if ( b.occupied(xx, yy) ) { // push move
            if ( out(b, xxx, yyy) or b.wall(xxx, yyy) or b.occupied(xxx, yyy) ) {
                continue;
            }
            else o = 1;
        }
        if (b.moves.empty() or inv_o_char[o][dir] != last_move){
            moves[i++] = MOVE(o_char[o][dir]);
        }
        if ( !b.occupied(xx, yy) and !out(b, _x, _y) and b.occupied(_x, _y) ) { // pull move
            o = 2;
            if (b.moves.empty() or inv_o_char[o][dir] != last_move){
                moves[i++] = MOVE(o_char[o][dir]);
            }
        }
    }
}

int main(int argc, char **argv){
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

#define CLEAR_SCREEN fprintf(stdout, "\033[H\033[J")
#define CLEAR_LINE(line) for(int i=0;i<line; ++i) { fprintf(stdout, "\033[1F\033[2K"); }
            BOARD b;
#define PRIME_N 7000013
            hash_init();
            for(int stage=1; get_board(f_in, b); ++stage){
                bool visited[PRIME_N] = {false};
                if ( stage < start_stage ) continue;
                printf("Stage #%d: \n", stage);
                //bfs
                STATE::init(b);
                std::queue<STATE*> open;
                open.emplace(new STATE(b));
                while(!open.empty()){
                    MOVE actions[5];
                    STATE* curState = open.front();
                    assert(switchstate(b, *curState));
                    feasible_moves(b, actions);
                    for(int i=0; i<5; ++i){
                        if(actions[i].dir == STAY) break;
                        assert(do_move(b, actions[i]));
                        STATE nextState = STATE(b);
                        if(nextState.solved()){
                            goto SOLVED;
                        }
                        else {
                            int hash = hashCode(nextState) % PRIME_N;
                            if( not visited[hash] ) { //not yet searched
                                open.emplace(new STATE(*curState, b));
                                visited[hash] = true;
                            }
                            undo_move(b);
                        }
                    }
                    // b.display_board(); open.back(); // for debug 
                    open.pop();
                }
SOLVED:
                printf("%d\n", SZ(b.moves));
                for ( int i=0; i<SZ(b.moves); ++i ) {
                    printf("%c", b.moves[i].to_char());
                }
                printf("\n");

                if(f_out) {
                    fprintf(f_out, "%d\n", SZ(b.moves));
                    for ( int i=0; i<SZ(b.moves); ++i ) {
                        fprintf(f_out, "%c", b.moves[i].to_char());
                    }
                    fprintf(f_out, "\n");
                } 
                b.moves.erase(b.moves.begin(), b.moves.end());
            }
            if(f_in) fclose(f_in);
            if(f_out) fclose(f_out);
            return 0;
}

