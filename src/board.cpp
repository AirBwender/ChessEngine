#include "board.h"
#include <iostream>
#include <vector>
#include <cmath>

using namespace std;



Board::Board() {
    int start[64] = {
        BR, BN, BB, BQ, BK, BB, BN, BR,
        BP, BP, BP, BP, BP, BP, BP, BP,
        EMPTY, EMPTY, EMPTY, EMPTY,
        EMPTY, EMPTY, EMPTY, EMPTY,
        EMPTY, EMPTY, EMPTY, EMPTY,
        EMPTY, EMPTY, EMPTY, EMPTY,
        WP, WP, WP, WP, WP, WP, WP, WP,
        WR, WN, WB, WQ, WK, WB, WN, WR
    };

    for(int i=0;i<64;i++)
        squares[i] = start[i];

    whiteToMove = true;
}

void Board::print() {

    const char* names = ".PNBRQKpnbrqk";

    for(int r = 7; r >= 0; r--) {
        for(int c = 0; c < 8; c++) {

            int piece = squares[r*8 + c];

            char symbol = names[piece];

            std::cout << symbol << " ";
        }
        std::cout << "\n";
    }

    std::cout << (whiteToMove ? "White" : "Black") 
              << " to move\n";
}

// ----- Helpers -----
bool inBounds(int sq) {
    return sq >= 0 && sq < 64;
}

int row(int sq) { return sq / 8; }
int col(int sq) { return sq % 8; }


vector<Move> generateKnight(Board &b, int sq) {

    vector<int> offsets = {
        -17, -15, -10, -6,
         6, 10, 15, 17
    };

    vector<Move> moves;

    for(int off : offsets) {
        int to = sq + off;

        if(!inBounds(to)) continue;

        // PREVENT BOARD WRAP
        int dr = abs(row(to) - row(sq));
        int dc = abs(col(to) - col(sq));
        if(dr + dc != 3) continue;

        int piece = b.squares[to];

        // empty OR enemy
        if(piece == EMPTY ||
          (b.whiteToMove && piece >= BP) ||
          (!b.whiteToMove && piece <= WK)) {

            moves.emplace_back(sq, to, piece);
        }
    }

    return moves;
}


vector<Move> generatePawn(Board &b, int sq) {

    vector<Move> moves;

    int dir = b.whiteToMove ? -8 : 8;
    int to = sq + dir;

    // ----- forward one -----
    if(inBounds(to) && b.squares[to] == EMPTY) {
        moves.emplace_back(sq, to);

        // ----- double step -----
        bool startRank =
            (b.whiteToMove && row(sq) == 6) ||
            (!b.whiteToMove && row(sq) == 1);

        int to2 = sq + 2*dir;

        if(startRank && inBounds(to2) && b.squares[to2] == EMPTY)
            moves.emplace_back(sq, to2);
    }

    // ----- captures -----
    int caps[2] = {dir-1, dir+1};

    for(int c : caps) {
        int t = sq + c;

        if(!inBounds(t)) continue;

        int p = b.squares[t];

        if(p != EMPTY &&
          ((b.whiteToMove && p >= BP) ||
           (!b.whiteToMove && p <= WK))) {

            moves.emplace_back(sq, t, p);
        }
    }

    return moves;
}

vector<Move> generateAll(Board &b) {

    vector<Move> all;

    for(int i=0;i<64;i++){

        int p = b.squares[i];

        if(p == EMPTY) continue;

        bool isWhitePiece = p <= WK;

        if(isWhitePiece != b.whiteToMove)
            continue;

        if(p == WN || p == BN) {
            auto m = generateKnight(b, i);
            all.insert(all.end(), m.begin(), m.end());
        }

        if(p == WP || p == BP) {
            auto m = generatePawn(b, i);
            all.insert(all.end(), m.begin(), m.end());
        }
    }

    return all;
}


