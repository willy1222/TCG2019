// Copyright (C) 2019 Yueh-Ting Chen (eopXD)
/*! \file pukoban.cpp
  \brief defines the function declared in pukoban.hpp
  \author Yueh-Ting Chen (eopXD)
  \course Theory of Computer Game (TCG)
 */
#include <cstdio>
#include "pukoban.hpp"

bool get_board ( FILE *f_in, BOARD &b ) {
    if ( fscanf(f_in, "%d%d", &b.n, &b.m) != 2 ) {
        goto INPUT_FAIL;
    }
    for ( int i=0; i<b.n; ++i ) {
        fscanf(f_in, "%s", b.initial[i]);
    }
    b.construct();
    //INPUT_SUCCESS:
    return (1);
INPUT_FAIL:
    return (0);
}

int const dx[] = {-1, 1, 0, 0};
int const dy[] = {0, 0, -1, 1};

// out-of-bound or not
bool out ( BOARD &b, int x, int y ) { 
    return (x<0 or y<0 or x>=b.n or y>=b.m); 
}
// valid or not
bool valid ( BOARD &b, MOVE &m ) {
    int x, y;
    b.player(&x, &y);
    int xx = x+dx[m.dir];
    int yy = y+dy[m.dir];
    int _x = x-dx[m.dir];
    int _y = y-dy[m.dir];
    if ( out(b, xx, yy) or b.wall(xx, yy) ) { // (xx, yy) not available
        goto INVALID;
    }
    if ( m.drag ) { // drag move
        if ( !out(b, _x, _y) and b.occupied(_x, _y) and b.occupied(xx, yy) ) {
            goto INVALID;
        }
    }
    if ( b.occupied(xx, yy) ) { // push move
        int xxx = xx+dx[m.dir];
        int yyy = yy+dy[m.dir];
        if ( out(b, xxx, yyy) or b.wall(xxx, yyy) or b.occupied(xxx, yyy) ) {
            goto INVALID;
        }
    }
    //VALID:
    return (1);
INVALID:
    return (0);
}
// do MOVE to BOARD (successful move is pushed into history)
bool do_move ( BOARD &b, MOVE &m ) {
    int x, y;
    b.player(&x, &y);
    int _x = x-dx[m.dir];
    int _y = y-dy[m.dir];
    int xx = x+dx[m.dir];
    int yy = y+dy[m.dir];
    int xxx = xx+dx[m.dir];
    int yyy = yy+dy[m.dir];
    if ( !valid(b, m) ) {
        goto ILLEGAL_MOVE;
    }

    if ( m.drag and !out(b, _x, _y) and b.occupied(_x, _y) ) {
        m.pulled = 1;
    }
    if ( !out(b, xx, yy) and b.occupied(xx, yy) ) {
        m.pushed = 1;
    }
    assert(!(m.pulled and m.pushed));

    //LEGAL_MOVE:
    if ( m.pulled ) {
        b.move_player(xx, yy);
        b.move_box(_x, _y, x, y);		
    }
    else if ( m.pushed ) {
        b.move_box(xx, yy, xxx, yyy);
        b.move_player(xx, yy);
    }
    else {
        b.move_player(xx, yy);
    }
    b.moves.push_back(m); // NOTE!!!
    return (1);
ILLEGAL_MOVE:
    return (0);
}
// undo BOARD with last MOVE in b.moves
bool undo_move ( BOARD &b ) {
    if ( b.moves.empty() ) { // already at beginning
        return (0);
    }
    MOVE m = b.moves.back(); b.moves.pop_back();	

    int x, y;
    b.player(&x, &y);
    int _x = x-dx[m.dir];
    int _y = y-dy[m.dir];	
    int xx = x+dx[m.dir];
    int yy = y+dy[m.dir];
    if ( m.pulled ) {
        b.move_box(_x, _y, _x-dx[m.dir], _y-dy[m.dir]);
        b.move_player(_x, _y);
    }
    else if ( m.pushed ) {
        b.move_player(_x, _y);
        b.move_box(xx, yy, x, y);
    } else {
        b.move_player(_x, _y);
    }
    return (1);
}

unsigned char *STATE::goal_positions = nullptr;
int STATE::boxCount = 0;

bool switchstate( BOARD &b, STATE &s ){
    if (not s.pre_state){ //s is root state
        return 1;
    }
    else { 
        STATE bs = STATE(b); 
        if (hashCode(bs) == hashCode(s)) { 
            return 1;
        }
        if (hashCode(bs) == hashCode(*s.pre_state)) {
            b.moves.emplace_back(s.last_move);                
        }
        else {
            if (not b.moves.empty()) { //b is not initial board
                undo_move(b);
                switchstate(b, *(s.pre_state));
                b.moves.emplace_back(s.last_move);                
            }
            else { //s is one or two depth deeper than b
                if (not s.pre_state->pre_state){ // s.pre_state is root
                    b.moves.emplace_back(s.last_move);                
                }
                else if(not s.pre_state->pre_state->pre_state){
                    b.moves.emplace_back(s.pre_state->last_move);                
                }
                else return 0;
            }
        }
    }

    for (int i = 0; i < b.boxCount; ++i) {
        unsigned char &sbpos = s.box_positions[i], &bbpos = b.box_positions[i];
        unsigned char sx=bbpos/b.m, sy=bbpos%b.m, ex=sbpos/b.m, ey=sbpos%b.m;
        if (not(sbpos == bbpos)){
            if(b.px*b.m+b.py == sbpos){ // pulled
                b.move_player(s.player_position);
                b.move_box(sx, sy, ex, ey);
            }
            else if(b.px*b.m+b.py == bbpos){ // pushed
                b.move_box(sx, sy, ex, ey);
                b.move_player(s.player_position);
            }
            else b.move_box(sx, sy, ex, ey);

            bbpos = sbpos;
        }
    }
    b.move_player(s.player_position);

    return 1;
}

void hash_init(){
    std::random_device r;
    std::seed_seq      seed{r(), r(), r(), r(), r(), r(), r(), r()};
    std::mt19937       eng(seed); // a source of random data
    std::uniform_int_distribution<int> dist;

    for (int i = 0; i < 50; ++i) {
        for (int j = 0; j < 2; ++j) {
            table[i][j] = dist(eng);
        }
    }
}
int hashCode(STATE &s){
    int hash = 0;
    for (int i = 0; i < STATE::boxCount; ++i) {
        hash ^= table[s.box_positions[i]][0];
    }
    hash ^= table[s.player_position][1];
    return hash;
}
int hashCode(unsigned char *str){
    int hash = 0;
    for (int i = 0; i < STATE::boxCount; ++i) {
        hash ^= table[str[i]][0];
    }
    return hash;
}
