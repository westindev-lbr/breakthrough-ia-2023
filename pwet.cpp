// Je dois faire un programme qui joue au breakthrough et qui supporte le btp
#include <iostream>
#include "mybt.h"

bt_t B;
int boardwidth = 0;
int boardheight = 0;
bool white_turn = true;

#ifndef VERBOSE_RAND_PLAYER
#define VERBOSE_RAND_PLAYER
bool verbose = true;
bool showboard_at_each_move = false;
#endif

void help()
{
    fprintf(stderr, "  quit\n");
    fprintf(stderr, "  echo ON | OFF\n");
    fprintf(stderr, "  help\n");
    fprintf(stderr, "  name <PLAYER_NAME>\n");
    fprintf(stderr, "  newgame <NBCOL> <NBLINE>\n");
    fprintf(stderr, "  genmove\n");
    fprintf(stderr, "  play <L0C0L1C1>\n");
    fprintf(stderr, "  showboard\n");
}
void name()
{
    printf("= pwet\n\n");
}

void newgame()
{
    if ((boardheight < 1 || boardheight > 10) && (boardwidth < 1 || boardwidth > 10))
    {
        fprintf(stderr, "boardsize is %d %d ???\n", boardheight, boardwidth);
        printf("= \n\n");
        return;
    }
    B.init(boardheight, boardwidth);
    white_turn = true;
    if (verbose)
        fprintf(stderr, "ready to play on %dx%d board\n", boardheight, boardwidth);
    printf("= \n\n");
}

void showboard()
{
    B.print_board(stderr);
    printf("= \n\n");
}

void genmove()
{
    int ret = B.endgame();
    if (ret != EMPTY)
    {
        fprintf(stderr, "game finished\n");
        if (ret == WHITE)
            fprintf(stderr, "white player wins\n");
        else
            fprintf(stderr, "black player wins\n");
        printf("= \n\n");
        return;
    }
    bt_move_t m = B.get_rand_move();
    B.play(m);
    if (verbose)
    {
        m.print(stderr, white_turn, B.nbl);
        fprintf(stderr, "\n");
    }
    white_turn = !white_turn;
    printf("= %s\n\n", m.tostr(B.nbl).c_str());
}

int main(void)
{
    bool echo_on = false;
    setbuf(stdout, 0);
    setbuf(stderr, 0);
    if(verbose) fprintf(stderr, "rand_player started\n");
    std::cout << "Hello World" << std::endl;

    return 0;
}