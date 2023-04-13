#ifndef MYBT_H
#define MYBT_H

#include <cstdio>
#include <cstdlib>
#include <random>
#include <iostream>
#include <chrono>
#include <vector>

#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))

#define WHITE 0
#define BLACK 1
#define EMPTY 2
#define INF 999
#define DEPTH_MAX 13

// Chars of board
char* cboard = ( char* ) "o@.";

// Pion location
struct bt_piece_t {
    int line;
    int col;
};

// Struct with methods
struct bt_move_t {
    int line_i;
    int col_i;
    int line_f;
    int col_f;
    bool eaten; // A mangé un pion

    // all moves are printed without ambiguity
    // white in its color
    // black in red color
    void print( FILE* _fp, bool _white, int _nbl ) {
        if ( _white ) {
            fprintf( _fp, "%d%c%d%c", _nbl - line_i, 'a' + col_i, _nbl - line_f, 'a' + col_f );
        }
        else {
            fprintf( _fp, "\x1B[31m%d%c%d%c\x1B[0m", _nbl - line_i, 'a' + col_i, _nbl - line_f, 'a' + col_f );
        }
    }
    std::string tostr( int _nbl ) {
        char ret[16];
        snprintf( ret, sizeof( ret ), "%d%c%d%c", _nbl - line_i, 'a' + col_i, _nbl - line_f, 'a' + col_f );
        return std::string( ret );
    }
};

// alloc default 10x10
// standard game in 8x8
#define MAX_LINES 10
#define MAX_COLS 10

// rules reminder :
// pieces moves from 1 square in diag and in front
// pieces captures only in diag
// i.e. to go forward, square must be empty

struct bt_t {
    int nbl;
    int nbc;
    int board[MAX_LINES][MAX_COLS];
    int turn;

    bt_piece_t white_pieces[2 * MAX_LINES];
    int nb_white_pieces;
    bt_piece_t black_pieces[2 * MAX_LINES];
    int nb_black_pieces;
    bt_move_t moves[3 * 2 * MAX_LINES];
    int nb_moves;
    // last turn of moves update
    int turn_of_last_moves_update;

    // Pile de mouvements
    std::vector<bt_move_t> move_stack;

    void init( int _nbl, int _nbc );
    void init_pieces();
    void print_board( FILE* _fp );
    void print_turn_and_moves( FILE* _fp );
    void update_moves();
    void update_moves( int _color );

    bool white_can_move_right( int _line, int _col );
    bool white_can_move_forward( int _line, int _col );
    bool white_can_move_left( int _line, int _col );
    bool black_can_move_right( int _line, int _col );
    bool black_can_move_forward( int _line, int _col );
    bool black_can_move_left( int _line, int _col );

    bt_move_t get_rand_move();
    bt_move_t get_best_move();
    bool can_play( bt_move_t _m );
    void play( bt_move_t _m );
    int endgame();
    double score( int _color );
    void playout( bool _log );
    std::string mkH1();
    std::string mkH2();
    long long int mkH3();

    // déclarées mais non définies
    double eval();
    bt_move_t minimax( double _sec );
    bt_move_t alphabeta( double _sec );
    double alphabeta_search( int _depth, double _alpha, double _beta );
    bt_move_t mcts( double _sec );
    bt_move_t mcts_ppa( double _sec );
    bt_move_t nmcs( double _sec );
    bt_move_t nrpa( double _sec );

    void add_move( int _li, int _ci, int _lf, int _cf ) {
        moves[nb_moves].line_i = _li;
        moves[nb_moves].col_i = _ci;
        moves[nb_moves].line_f = _lf;
        moves[nb_moves].col_f = _cf;
        nb_moves++;
    }

    void push_move( bt_move_t move ) {
        move_stack.push_back( move );
    }

    void pop_move() {
        bt_move_t last_move = move_stack.back();
        undo( last_move );
        move_stack.pop_back();
    }

    void undo( bt_move_t last_move );
};

void bt_t::init( int _nbl, int _nbc ) {
    if ( _nbl > MAX_LINES || _nbc > MAX_COLS ) {
        fprintf( stderr, "ERROR : MAX_LINES or MAX_COLS exceeded\n" );
        exit( 0 );
    }
    nbl = _nbl;
    nbc = _nbc;
    turn = 0;
    turn_of_last_moves_update = -1;
    for ( int i = 0; i < nbl; i++ )
        for ( int j = 0; j < nbc; j++ ) {
            if ( i <= 1 ) {
                board[i][j] = BLACK;
            }
            else if ( i < _nbl - 2 ) {
                board[i][j] = EMPTY;
            }
            else {
                board[i][j] = WHITE;
            }
        }
    init_pieces();
    update_moves();
}
void bt_t::init_pieces() {
    nb_white_pieces = 0;
    nb_black_pieces = 0;
    for ( int i = 0; i < nbl; i++ )
        for ( int j = 0; j < nbc; j++ ) {
            if ( board[i][j] == WHITE ) {
                white_pieces[nb_white_pieces].line = i;
                white_pieces[nb_white_pieces].col = j;
                nb_white_pieces++;
            }
            else if ( board[i][j] == BLACK ) {
                black_pieces[nb_black_pieces].line = i;
                black_pieces[nb_black_pieces].col = j;
                nb_black_pieces++;
            }
        }
}
// again print black in red (as bg is black... black is printed in red)
void bt_t::print_board( FILE* _fp = stderr ) {
    fprintf( _fp, "   \x1B[34m" );
    for ( int j = 0; j < nbc; j++ ) {
        fprintf( _fp, "%c ", 'a' + j );
    }
    fprintf( _fp, "\x1B[0m\n" );
    for ( int i = 0; i < nbl; i++ ) {
        fprintf( _fp, "\x1B[34m%2d\x1B[0m ", ( nbl - i ) );
        for ( int j = 0; j < nbc; j++ ) {
            if ( board[i][j] == BLACK )
                fprintf( _fp, "\x1B[31m%c\x1B[0m ", cboard[board[i][j]] );
            else
                fprintf( _fp, "%c ", cboard[board[i][j]] );
        }
        fprintf( _fp, "\n" );
    }
}

void bt_t::print_turn_and_moves( FILE* _fp = stderr ) {
    fprintf( _fp, "turn:%d\nmoves:", turn );
    for ( int i = 0; i < nb_moves; i++ ) {
        moves[i].print( _fp, turn % 2 == 1, nbl );
        fprintf( _fp, " " );
    }
    fprintf( _fp, "\n" );
}

void bt_t::update_moves() {
    if ( turn % 2 == 0 )
        update_moves( WHITE );
    else
        update_moves( BLACK );
}

void bt_t::update_moves( int _color ) {
    if ( turn_of_last_moves_update == turn )
        return; // MAJ ever done
    turn_of_last_moves_update = turn;
    nb_moves = 0;
    if ( _color == WHITE ) {
        for ( int i = 0; i < nb_white_pieces; i++ ) {
            int li = white_pieces[i].line;
            int ci = white_pieces[i].col;
            if ( white_can_move_right( li, ci ) )
                add_move( li, ci, li - 1, ci + 1 );
            if ( white_can_move_forward( li, ci ) )
                add_move( li, ci, li - 1, ci );
            if ( white_can_move_left( li, ci ) )
                add_move( li, ci, li - 1, ci - 1 );
        }
    }
    else if ( _color == BLACK ) {
        for ( int i = 0; i < nb_black_pieces; i++ ) {
            int li = black_pieces[i].line;
            int ci = black_pieces[i].col;
            if ( black_can_move_right( li, ci ) )
                add_move( li, ci, li + 1, ci + 1 );
            if ( black_can_move_forward( li, ci ) )
                add_move( li, ci, li + 1, ci );
            if ( black_can_move_left( li, ci ) )
                add_move( li, ci, li + 1, ci - 1 );
        }
    }
}
bool bt_t::white_can_move_right( int _line, int _col ) {
    if ( _line == 0 )
        return false;
    if ( _col == nbc - 1 )
        return false;
    if ( board[_line - 1][_col + 1] != WHITE )
        return true;
    return false;
}
bool bt_t::white_can_move_forward( int _line, int _col ) {
    if ( _line == 0 )
        return false;
    if ( board[_line - 1][_col] == EMPTY )
        return true;
    return false;
}
bool bt_t::white_can_move_left( int _line, int _col ) {
    if ( _line == 0 )
        return false;
    if ( _col == 0 )
        return false;
    if ( board[_line - 1][_col - 1] != WHITE )
        return true;
    return false;
}
bool bt_t::black_can_move_right( int _line, int _col ) {
    if ( _line == nbl - 1 )
        return false;
    if ( _col == nbc - 1 )
        return false;
    if ( board[_line + 1][_col + 1] != BLACK )
        return true;
    return false;
}
bool bt_t::black_can_move_forward( int _line, int _col ) {
    if ( _line == nbl - 1 )
        return false;
    if ( board[_line + 1][_col] == EMPTY )
        return true;
    return false;
}
bool bt_t::black_can_move_left( int _line, int _col ) {
    if ( _line == nbl - 1 )
        return false;
    if ( _col == 0 )
        return false;
    if ( board[_line + 1][_col - 1] != BLACK )
        return true;
    return false;
}

bt_move_t bt_t::get_rand_move() {
    // Modif
    srand( time( NULL ) );
    update_moves();
    int r = ( ( int ) rand() ) % nb_moves;
    return moves[r];
}

bt_move_t bt_t::get_best_move() {
    //update_moves();
    return alphabeta( 1.0 );
}

bool bt_t::can_play( bt_move_t _m ) {
    int dx = abs( _m.col_f - _m.col_i );
    if ( dx > 1 )
        return false;
    int dy = abs( _m.line_f - _m.line_i );
    if ( dy > 1 )
        return false;
    if ( _m.line_i < 0 || _m.line_i >= nbl )
        return false;
    if ( _m.line_f < 0 || _m.line_f >= nbl )
        return false;
    if ( _m.col_i < 0 || _m.col_i >= nbc )
        return false;
    if ( _m.col_f < 0 || _m.col_f >= nbc )
        return false;
    int color_i = board[_m.line_i][_m.col_i];
    int color_f = board[_m.line_f][_m.col_f];
    if ( color_i == EMPTY )
        return false;
    if ( color_i == color_f )
        return false;
    if ( turn % 2 == 0 && color_i == BLACK )
        return false;
    if ( turn % 2 == 1 && color_i == WHITE )
        return false;
    if ( _m.col_i == _m.col_f && color_f != EMPTY )
        return false;
    return true;
}

void bt_t::play( bt_move_t _m ) {
    int color_i = board[_m.line_i][_m.col_i];
    int color_f = board[_m.line_f][_m.col_f];
    board[_m.line_f][_m.col_f] = color_i;
    board[_m.line_i][_m.col_i] = EMPTY;
    _m.eaten = false;
    if ( color_i == WHITE ) {
        for ( int i = 0; i < nb_white_pieces; i++ ) {
            if ( white_pieces[i].line == _m.line_i && white_pieces[i].col == _m.col_i ) {
                white_pieces[i].line = _m.line_f;
                white_pieces[i].col = _m.col_f;
                break;
            }
        }
        if ( color_f == BLACK ) {
            for ( int i = 0; i < nb_black_pieces; i++ ) {
                if ( black_pieces[i].line == _m.line_f && black_pieces[i].col == _m.col_f ) {
                    black_pieces[i] = black_pieces[nb_black_pieces - 1];
                    nb_black_pieces--;
                    _m.eaten = true;
                    break;
                }
            }
        }
    }
    else if ( color_i == BLACK ) {
        for ( int i = 0; i < nb_black_pieces; i++ ) {
            if ( black_pieces[i].line == _m.line_i &&
                black_pieces[i].col == _m.col_i ) {
                black_pieces[i].line = _m.line_f;
                black_pieces[i].col = _m.col_f;
                break;
            }
        }
        if ( color_f == WHITE ) {
            for ( int i = 0; i < nb_white_pieces; i++ ) {
                if ( white_pieces[i].line == _m.line_f &&
                    white_pieces[i].col == _m.col_f ) {
                    white_pieces[i] = white_pieces[nb_white_pieces - 1];
                    nb_white_pieces--;
                    _m.eaten = true;
                    break;
                }
            }
        }
    }
    turn++;
}

int bt_t::endgame() {
    for ( int i = 0; i < nbc; i++ ) {
        if ( board[0][i] == WHITE )
            return WHITE;
    }
    for ( int i = 0; i < nbc; i++ ) {
        if ( board[nbl - 1][i] == BLACK )
            return BLACK;
    }
    return EMPTY;
}

double bt_t::score( int _color ) {
    int state = endgame();
    if ( state == EMPTY )
        return 0.0;
    if ( _color == state )
        return 1.0;
    return -1.0;
}

double bt_t::eval() {
    double score = 0.0;
    int black_home_row = 0;
    int white_home_row = nbl - 1;
    int white_pawns = 0;
    int black_pawns = 0;

    // Compte le nombre de pions et évalue leur distance par rapport à la ligne de départ adverse
    for ( int i = 0; i < nb_white_pieces; i++ ) {
        white_pawns++;
        int row_distance = white_home_row - white_pieces[i].line;
        score += row_distance;
    }

    for ( int i = 0; i < nb_black_pieces; i++ ) {
        black_pawns++;
        int row_distance =  black_pieces[i].line - black_home_row;
        score -= row_distance;
    }

    // Heuristique permettant d'avantager le joueur selon le nombre 
    // de pions noirs et blancs encore en jeux
    if ( white_pawns > black_pawns ) {
        score += 0.5;
    }
    else if ( white_pawns < black_pawns ) {
        score -= 0.5;
    }

    // Ajoute une pénalité pour l'absence de pions
    if ( white_pawns == 0 ) {
        score = -100.0;
    }
    else if ( black_pawns == 0 ) {
        score = 100.0;
    }

    return score;
}

// Fonction Alpha-Beta qui retourne le meilleur coup possible pour le joueur courant
// dans la limite du temps en seconde passé en paramètre
bt_move_t bt_t::alphabeta( double _sec ) {

    // Calcul du temps maximal autorisé pour l'exécution de la fonction
    auto start_time = std::chrono::high_resolution_clock::now();
    auto max_time = std::chrono::duration<double>( _sec );
    auto current_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_time = current_time - start_time;

    int depth = 1;       // Profondeur dé départ d'alpha beta
    double alpha = INF;  // Valeur alpha
    double beta = -INF;  // Valeur beta
    bt_move_t best_move; // Meilleur coup trouvé jusqu'à présent

    // Tant que le temps de recherche n'est pas écoulé, on continue la recherche en profondeur
    while ( elapsed_time < max_time )
        // while (depth < 5)
    {

        double score = alphabeta_search( depth, alpha, beta );

        if ( score > beta ) {
            beta = score;
            // ? BIZARRE SUR L'ORDRE DES COUPS 
            // Le meilleur coup est le dernier ajouté à la liste des coups possibles
            best_move = move_stack.back();
        }

        // ! POURQUOI ? 
        /* if ( alpha >= beta ) {
            break;
        } */

        depth++;

        // Mise à jour du temps écoulé
        current_time = std::chrono::high_resolution_clock::now();
        elapsed_time = current_time - start_time;
    }

    return best_move;
}

// Fonction récursive de recherche Alpha-Beta à une certaine profondeur
// Retourne la meilleure valeur possible à partir de la position actuelle
double bt_t::alphabeta_search( int _depth, double _alpha, double _beta ) {

    if ( _depth == DEPTH_MAX ) {
        return eval();
    }

    // Génère la liste des coups possibles
    update_moves();

    // Si la liste est vide, on retourne la valeur d'évaluation de la position actuelle
    if ( nb_moves == 0 ) {
        return eval();
    }

    double best = 0.0;
    double score_move = 0.0;

    // Effectue une recherche Alpha-Beta sur chaque coup possible
    for ( int i = 0; i < nb_moves; i++ ) {

        bt_move_t move = moves[i]; // Récupère le coup à jouer
        play( move );
        // Effectue une recherche Alpha-Beta sur le coup joué et retourne un score
        score_move = alphabeta_search( _depth + 1, _alpha, _beta );

        undo( move ); // Annule le coup joué

        move_stack.push_back( move );

        // On maximise sur les tours paires uniquement
        if ( turn % 2 == 0 ) {
            best = max( score_move, best );
            if ( best >= _beta )return best;
            _alpha = max( _alpha, best );
        }
        else {
            best = min( score_move, best );
            if ( best <= _alpha )
                return best;
            _beta = min( _beta, best );
        }
    }

    return best;
}

//"6c5c"

void bt_t::undo( bt_move_t last_move ) {
    int line_i = last_move.line_i;
    int col_i = last_move.col_i;
    int line_f = last_move.line_f;
    int col_f = last_move.col_f;

    // Couleur du pion à retiré
    int color_piece_to_be_removed = board[line_f][col_f];

    // Gestion des conteneurs de pion et mise à jour de sa position précédente
    if ( color_piece_to_be_removed == WHITE ) {
        for ( int i = 0; i < nb_white_pieces; i++ ) {
            if ( white_pieces[i].line == line_f &&
                white_pieces[i].col == col_f ) {
                white_pieces[i].line = line_i;
                white_pieces[i].col = col_i;
                break;
            }
        }
    }
    else if ( color_piece_to_be_removed == BLACK ) {
        for ( int i = 0; i < nb_black_pieces; i++ ) {
            if ( black_pieces[i].line == line_f &&
                black_pieces[i].col == col_f ) {
                black_pieces[i].line = line_i;
                black_pieces[i].col = col_i;
                break;
            }
        }
    }

    // Annule le dernier coup joué en remettant la pièce dans sa position précédente au niveau du board
    board[last_move.line_f][last_move.col_f] = board[last_move.line_i][last_move.col_i];
    board[last_move.line_i][last_move.col_i] = color_piece_to_be_removed;


    // Si une pièce a été mangé on la remet sur le board et dans les conteneurs de pions
    if ( last_move.eaten ) {
        if ( turn == WHITE ) {
            board[line_f][col_f] = BLACK;
            black_pieces[nb_black_pieces].line = line_f;
            black_pieces[nb_black_pieces].col = col_f;
            nb_black_pieces++;
        }
        else if ( turn == BLACK ) {
            board[line_f][col_f] = WHITE;
            white_pieces[nb_white_pieces].line = line_f;
            white_pieces[nb_white_pieces].col = col_f;
            nb_white_pieces++;
        }
    }

    // Décrémente le numéro de tour
    turn--;
}

#endif /* MYBT_H */
