#include <vector>
#include "move.h"

enum Piece {
    EMPTY = 0,
    WP, WN, WB, WR, WQ, WK,
    BP, BN, BB, BR, BQ, BK
};

class Board {
public:
    int squares[64];
    bool whiteToMove;
    int castlingRights;
    int enPassant;

    struct State {
        int captured;
        int castlingRights;
        int enPassant;
    };
    std::vector<State> history;

    Board();
    void print();
};

// ----- Helpers -----
bool inBounds(int sq);
int row(int sq);
int col(int sq);

// ----- Move Generation -----
std::vector<Move> generateKnight(Board &b, int sq);
std::vector<Move> generatePawn(Board &b, int sq);
std::vector<Move> generateBishop(Board &b, int sq);
std::vector<Move> generateRook(Board &b, int sq);
std::vector<Move> generateQueen(Board &b, int sq);
std::vector<Move> generateKing(Board &b, int sq);
std::vector<Move> generateAll(Board &b);

void makeMove(Board &b, const Move &m);
void unmakeMove(Board &b, const Move &m);

long long perft(Board &b, int depth);
long long divide(Board &b, int depth);
