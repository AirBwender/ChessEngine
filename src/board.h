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
std::vector<Move> generateAll(Board &b);
