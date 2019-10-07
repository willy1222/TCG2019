// by Yunghsien Chung (hiiragi4000)

#include<assert.h>
#include<ctype.h>
#include<stdbool.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>

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

// If the format of the output file (solution) is invalid, the program terminates.
void Output_verifier(FILE *fin){
#define INVALID_OUTPUT do{\
    puts("The output file is invalid.");\
    exit(1);\
}while(0)
    char line1[20];
    for(int stage=1; fgets(line1, 20, fin); ++stage){
        int len = strlen(line1);
        if(len<2 || len>9 || line1[len-1]!='\n') INVALID_OUTPUT;
        line1[--len] = 0;
        int n = 0;
        for(int i=0; i<=len-1; ++i){
            if(!isdigit(line1[i]) || (len>1&&i==0&&line1[i]=='0')) INVALID_OUTPUT;
            n = 10*n+line1[i]-48;
        }
        char *buf = new char [n+2];
        if(!fgets(buf, n+2, fin)) INVALID_OUTPUT;
        if((int)strlen(buf)!=n+1 || buf[n]!='\n') INVALID_OUTPUT;
        for(int i=0; i<=n-1; ++i){
            if(!strchr("udlrUDLR^v<>", buf[i])) INVALID_OUTPUT;
        }
        free(buf);
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
    char *input=0, *output=0;
    int start_stage = 0;
    for ( int i=1; i<argc; ++i ) {
    	if ( strcmp(argv[i], "-i") == 0 ) {
    		++i;
    		if ( i==argc or input ) {
    			PUKOBAN_WRONG_ARG;
    		}
    		input = argv[i];
    	}
    	else if ( strcmp(argv[i], "-o") == 0 ) {
    		++i;
    		if ( i==argc or output ) {
    			PUKOBAN_WRONG_ARG;
    		}
    		output = argv[i];
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
    if ( !input ) {
    	PUKOBAN_WRONG_ARG;
    }
    if ( !start_stage ) {
    	start_stage = 1;
    }
    if(!input && !output) PUKOBAN_WRONG_ARG; 
    #undef PUKOBAN_WRONG_ARG
    FILE *fpi=0, *fpo=0;
    if(input){
        fpi = fopen(input, "r");
        if(!fpi){
            printf("Error in opening the file \'%s\'\n", input);
            return 1;
        }
        Input_verifier(fpi);
    }
    if(output){
        fpo = fopen(output, "r");
        if(!fpo){
            printf("Error in opening the file \'%s\'\n", output);
            return 1;
        }
        Output_verifier(fpo);
    }
    if(input && output){
        rewind(fpi);
        rewind(fpo);
        BOARD b;
        for(int stage=1; get_board(fpi, b); ++stage){
            printf("Stage #%d: ", stage);
            int n;
            if(fscanf(fpo, "%d ", &n)!=1 || n==0){
                puts("Wrong Answer");
                continue;
            }
            char *sol = new char [n+2];
            if(!fgets(sol, n+2, fpo)){
                // never happens
                assert(0);
            }
        	if ( stage < start_stage ) continue;
            bool ac = true;
        /* display */
            #define CLEAR_SCREEN fprintf(stdout, "\033[H\033[J")
        	CLEAR_SCREEN;
        	printf("\033[1;36mStage #%d:\033[m \n", stage);
            fprintf(stdout, "\n");
        	
        	b.display_board();
        	fprintf(stdout, "\nMove Count: \033[1m%3d\033[m\n", SZ(b.moves));
        	fprintf(stdout, "Moves: "); b.display_moves();
        	fprintf(stdout, "\n");
            sleep(4);

            for(int i=0; i<=n-1; ++i){
                MOVE m = MOVE(sol[i]);
                if ( !do_move(b, m) ) {
                	ac = false; break;
                }
        	/* display */
        		CLEAR_SCREEN;
        		printf("\033[1;36mStage #%d:\033[m \n", stage);
        		if ( m.drag ) {
        			fprintf(stdout, "\t\t\033[1;37;41mD\033[m\n");
        		}
        		else {
        			fprintf(stdout, "\n");
        		}			
        		
        		b.display_board();
        		fprintf(stdout, "\nMove Count: \033[1m%3d\033[m\n", SZ(b.moves));
        		fprintf(stdout, "Moves: "); b.display_moves();
        		fprintf(stdout, "\n");
                usleep(100000);
            }
            if ( !b.solved() ) {
            	ac = false;
            }

            if(ac){
                printf("Accepted: %d\n", n);
            }else puts("Wrong Answer");
            free(sol);
            sleep(2);
	        b.moves.clear(); // clear history
        }
        char tail[2];
        if(fgets(tail, 2, fpo)){
            puts("Output Limit Exceeded");
        }
    }
    if(fpi) fclose(fpi);
    if(fpo) fclose(fpo);
    return 0;
}
