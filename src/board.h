enum Piece {
    EMPTY = 0,
    WP, WN, WB, WR, WQ, WK,
    BP, BN, BB, BR, BQ, BK
};

struct Move {
    int from, to;
    int captured;
};

class Board {
public:
    int squares[64];
    bool whiteToMove;

    Board();
    void print();
};
