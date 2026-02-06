#include "board.h"
#include <iostream>
#include <vector>
#include <cmath>

using namespace std;



Board::Board() {
    int start[64] = {
        WR, WN, WB, WQ, WK, WB, WN, WR,
        WP, WP, WP, WP, WP, WP, WP, WP,

        EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
        EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
        EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
        EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,

        BP, BP, BP, BP, BP, BP, BP, BP,
        BR, BN, BB, BQ, BK, BB, BN, BR
    };



    for(int i=0;i<64;i++)
        squares[i] = start[i];

    whiteToMove = true;
    castlingRights = 0b1111; // WK, WQ, BK, BQ
    enPassant = -1;
}

void Board::print() {

    const char* names = ".PNBRQKpnbrqk";

    for(int r = 0; r < 8; r++) {
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
    int movedPiece = b.squares[sq];

    for(int off : offsets) {
        int to = sq + off;

        if(!inBounds(to)) continue;

        // PREVENT BOARD WRAP
        int dr = abs(row(to) - row(sq));
        int dc = abs(col(to) - col(sq));
        if(dr + dc != 3) continue;

        int target = b.squares[to];

        // empty OR enemy
        if(target == EMPTY ||
          (b.whiteToMove && target >= BP) ||
          (!b.whiteToMove && target <= WK)) {

            if(target == EMPTY) {
                moves.emplace_back(sq, to, movedPiece);
            } else {
                moves.emplace_back(sq, to, movedPiece, target, 0, MOVE_CAPTURE);
            }
        }
    }

    return moves;
}


vector<Move> generatePawn(Board &b, int sq) {

    vector<Move> moves;

    int dir = b.whiteToMove ? 8 : -8;
    int to = sq + dir;
    int piece = b.squares[sq];

    // ----- forward one -----
    if(inBounds(to) && b.squares[to] == EMPTY) {
        bool promoRank = (b.whiteToMove && row(to) == 7) ||
                         (!b.whiteToMove && row(to) == 0);
        if(promoRank) {
            int promos[4] = { b.whiteToMove ? WQ : BQ,
                              b.whiteToMove ? WR : BR,
                              b.whiteToMove ? WB : BB,
                              b.whiteToMove ? WN : BN };
            for(int pr : promos) {
                moves.emplace_back(sq, to, piece, EMPTY, pr, MOVE_PROMO);
            }
        } else {
            moves.emplace_back(sq, to, piece);
        }

        // ----- double step -----
        bool startRank =
            (b.whiteToMove && row(sq) == 1) ||
            (!b.whiteToMove && row(sq) == 6);


        int to2 = sq + 2*dir;

        if(startRank && inBounds(to2) && b.squares[to2] == EMPTY) {
            moves.emplace_back(sq, to2, piece);
        }
    }

    // ----- captures -----
    int caps[2] = {dir-1, dir+1};

    for(int c : caps) {
        int t = sq + c;

        if(!inBounds(t)) continue;
        if(abs(col(t) - col(sq)) != 1) continue;

        int p = b.squares[t];

        if(p != EMPTY &&
          ((b.whiteToMove && p >= BP) ||
           (!b.whiteToMove && p <= WK))) {

            bool promoRank = (b.whiteToMove && row(t) == 7) ||
                             (!b.whiteToMove && row(t) == 0);
            if(promoRank) {
                int promos[4] = { b.whiteToMove ? WQ : BQ,
                                  b.whiteToMove ? WR : BR,
                                  b.whiteToMove ? WB : BB,
                                  b.whiteToMove ? WN : BN };
                for(int pr : promos) {
                    moves.emplace_back(sq, t, piece, p, pr, MOVE_PROMO | MOVE_CAPTURE);
                }
            } else {
                moves.emplace_back(sq, t, piece, p, 0, MOVE_CAPTURE);
            }
        }
    }

    // ----- en passant -----
    if(b.enPassant != -1) {
        for(int c : caps) {
            int t = sq + c;
            if(!inBounds(t)) continue;
            if(abs(col(t) - col(sq)) != 1) continue;
            if(t != b.enPassant) continue;

            int capSq = b.whiteToMove ? (t - 8) : (t + 8);
            int capPiece = b.whiteToMove ? BP : WP;
            if(inBounds(capSq) && b.squares[capSq] == capPiece) {
                moves.emplace_back(sq, t, piece, capPiece, 0, MOVE_EP | MOVE_CAPTURE);
            }
        }
    }

    return moves;
}

vector<Move> generateBishop(Board &b, int sq) {
    vector<Move> moves;
    int piece = b.squares[sq];
    int dirs[4] = { -9, -7, 7, 9 };

    for(int d : dirs) {
        int t = sq + d;
        while(inBounds(t) && abs(row(t) - row(t - d)) == 1 && abs(col(t) - col(t - d)) == 1) {
            int p = b.squares[t];
            if(p == EMPTY) {
                moves.emplace_back(sq, t, piece);
            } else {
                if((b.whiteToMove && p >= BP) ||
                   (!b.whiteToMove && p <= WK)) {
                    moves.emplace_back(sq, t, piece, p, 0, MOVE_CAPTURE);
                }
                break;
            }
            t += d;
        }
    }
    return moves;
}

vector<Move> generateRook(Board &b, int sq) {
    vector<Move> moves;
    int piece = b.squares[sq];
    int dirs[4] = { -8, 8, -1, 1 };

    for(int d : dirs) {
        int t = sq + d;
        while(inBounds(t) && (d == -8 || d == 8 || row(t) == row(t - d))) {
            int p = b.squares[t];
            if(p == EMPTY) {
                moves.emplace_back(sq, t, piece);
            } else {
                if((b.whiteToMove && p >= BP) ||
                   (!b.whiteToMove && p <= WK)) {
                    moves.emplace_back(sq, t, piece, p, 0, MOVE_CAPTURE);
                }
                break;
            }
            t += d;
        }
    }
    return moves;
}

vector<Move> generateQueen(Board &b, int sq) {
    vector<Move> moves;
    auto bmv = generateBishop(b, sq);
    auto rmv = generateRook(b, sq);
    moves.insert(moves.end(), bmv.begin(), bmv.end());
    moves.insert(moves.end(), rmv.begin(), rmv.end());
    return moves;
}

vector<Move> generateKing(Board &b, int sq) {
    vector<Move> moves;
    int piece = b.squares[sq];
    int offsets[8] = { -9, -8, -7, -1, 1, 7, 8, 9 };

    for(int off : offsets) {
        int t = sq + off;
        if(!inBounds(t)) continue;
        if(abs(row(t) - row(sq)) > 1 || abs(col(t) - col(sq)) > 1) continue;

        int p = b.squares[t];
        if(p == EMPTY) {
            moves.emplace_back(sq, t, piece);
        } else if((b.whiteToMove && p >= BP) ||
                  (!b.whiteToMove && p <= WK)) {
            moves.emplace_back(sq, t, piece, p, 0, MOVE_CAPTURE);
        }
    }

    // ----- castling (pseudo-legal) -----
    if(b.whiteToMove) {
        int kingSq = 4;
        if(sq == kingSq) {
            if((b.castlingRights & 0b0001) &&
               b.squares[5] == EMPTY && b.squares[6] == EMPTY) {
                moves.emplace_back(4, 6, piece, EMPTY, 0, MOVE_CASTLE);
            }
            if((b.castlingRights & 0b0010) &&
               b.squares[3] == EMPTY && b.squares[2] == EMPTY && b.squares[1] == EMPTY) {
                moves.emplace_back(4, 2, piece, EMPTY, 0, MOVE_CASTLE);
            }
        }
    } else {
        int kingSq = 60;
        if(sq == kingSq) {
            if((b.castlingRights & 0b0100) &&
               b.squares[61] == EMPTY && b.squares[62] == EMPTY) {
                moves.emplace_back(60, 62, piece, EMPTY, 0, MOVE_CASTLE);
            }
            if((b.castlingRights & 0b1000) &&
               b.squares[59] == EMPTY && b.squares[58] == EMPTY && b.squares[57] == EMPTY) {
                moves.emplace_back(60, 58, piece, EMPTY, 0, MOVE_CASTLE);
            }
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

        if(p == WB || p == BB) {
            auto m = generateBishop(b, i);
            all.insert(all.end(), m.begin(), m.end());
        }

        if(p == WR || p == BR) {
            auto m = generateRook(b, i);
            all.insert(all.end(), m.begin(), m.end());
        }

        if(p == WQ || p == BQ) {
            auto m = generateQueen(b, i);
            all.insert(all.end(), m.begin(), m.end());
        }

        if(p == WK || p == BK) {
            auto m = generateKing(b, i);
            all.insert(all.end(), m.begin(), m.end());
        }
    }

    return all;
}

static void updateCastlingRights(Board &b, const Move &m) {
    // White king/rook moves
    if(m.moved == WK) b.castlingRights &= 0b1100;
    if(m.moved == WR) {
        if(m.from == 0) b.castlingRights &= 0b1101;
        if(m.from == 7) b.castlingRights &= 0b1110;
    }

    // Black king/rook moves
    if(m.moved == BK) b.castlingRights &= 0b0011;
    if(m.moved == BR) {
        if(m.from == 56) b.castlingRights &= 0b0111;
        if(m.from == 63) b.castlingRights &= 0b1011;
    }

    // Capture of rook on original squares
    if(m.captured == WR) {
        if(m.to == 0) b.castlingRights &= 0b1101;
        if(m.to == 7) b.castlingRights &= 0b1110;
    }
    if(m.captured == BR) {
        if(m.to == 56) b.castlingRights &= 0b0111;
        if(m.to == 63) b.castlingRights &= 0b1011;
    }
}

void makeMove(Board &b, const Move &m) {
    Board::State st;
    st.captured = m.captured;
    st.castlingRights = b.castlingRights;
    st.enPassant = b.enPassant;
    b.history.push_back(st);

    b.enPassant = -1;

    // handle en passant capture
    if(m.flags & MOVE_EP) {
        int capSq = b.whiteToMove ? (m.to - 8) : (m.to + 8);
        b.squares[capSq] = EMPTY;
    }

    // move piece
    b.squares[m.to] = (m.flags & MOVE_PROMO) ? m.promo : m.moved;
    b.squares[m.from] = EMPTY;

    // handle castling rook move
    if(m.flags & MOVE_CASTLE) {
        if(m.to == 6) { // white king side
            b.squares[5] = WR;
            b.squares[7] = EMPTY;
        } else if(m.to == 2) { // white queen side
            b.squares[3] = WR;
            b.squares[0] = EMPTY;
        } else if(m.to == 62) { // black king side
            b.squares[61] = BR;
            b.squares[63] = EMPTY;
        } else if(m.to == 58) { // black queen side
            b.squares[59] = BR;
            b.squares[56] = EMPTY;
        }
    }

    // set en passant square on double pawn push
    if(m.moved == WP && (m.to - m.from) == 16) {
        b.enPassant = m.from + 8;
    } else if(m.moved == BP && (m.from - m.to) == 16) {
        b.enPassant = m.from - 8;
    }

    updateCastlingRights(b, m);

    b.whiteToMove = !b.whiteToMove;
}

void unmakeMove(Board &b, const Move &m) {
    if(b.history.empty()) return;
    Board::State st = b.history.back();
    b.history.pop_back();

    b.whiteToMove = !b.whiteToMove;
    b.castlingRights = st.castlingRights;
    b.enPassant = st.enPassant;

    // undo castling rook move
    if(m.flags & MOVE_CASTLE) {
        if(m.to == 6) { // white king side
            b.squares[7] = WR;
            b.squares[5] = EMPTY;
        } else if(m.to == 2) { // white queen side
            b.squares[0] = WR;
            b.squares[3] = EMPTY;
        } else if(m.to == 62) { // black king side
            b.squares[63] = BR;
            b.squares[61] = EMPTY;
        } else if(m.to == 58) { // black queen side
            b.squares[56] = BR;
            b.squares[59] = EMPTY;
        }
    }

    // move piece back
    b.squares[m.from] = m.moved;
    if(m.flags & MOVE_PROMO) {
        b.squares[m.to] = EMPTY;
    } else {
        b.squares[m.to] = m.captured;
    }

    // undo en passant capture
    if(m.flags & MOVE_EP) {
        int capSq = b.whiteToMove ? (m.to - 8) : (m.to + 8);
        b.squares[capSq] = b.whiteToMove ? BP : WP;
        b.squares[m.to] = EMPTY;
    }

    // restore captured piece for normal capture
    if((m.flags & MOVE_CAPTURE) && !(m.flags & MOVE_EP) && !(m.flags & MOVE_PROMO)) {
        b.squares[m.to] = m.captured;
    }
    if((m.flags & MOVE_CAPTURE) && (m.flags & MOVE_PROMO)) {
        b.squares[m.to] = m.captured;
    }
}

long long perft(Board &b, int depth) {
    if(depth == 0) return 1;

    long long nodes = 0;
    auto moves = generateAll(b);

    for(const auto &m : moves) {
        makeMove(b, m);
        nodes += perft(b, depth - 1);
        unmakeMove(b, m);
    }

    return nodes;
}

long long divide(Board &b, int depth) {
    long long total = 0;
    auto moves = generateAll(b);

    for(const auto &m : moves) {
        makeMove(b, m);
        long long n = perft(b, depth - 1);
        unmakeMove(b, m);
        total += n;
    }

    return total;
}


