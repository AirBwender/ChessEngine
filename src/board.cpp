#include "board.h"
#include <iostream>

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

