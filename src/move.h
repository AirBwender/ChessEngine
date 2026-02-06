#pragma once

enum MoveFlags {
    MOVE_NONE   = 0,
    MOVE_CAPTURE= 1 << 0,
    MOVE_EP     = 1 << 1,
    MOVE_CASTLE = 1 << 2,
    MOVE_PROMO  = 1 << 3
};

struct Move {
    int from;
    int to;
    int moved;
    int captured;
    int promo;
    int flags;

    Move(int f, int t, int m, int c = 0, int pr = 0, int fl = MOVE_NONE)
        : from(f), to(t), moved(m), captured(c), promo(pr), flags(fl) {}
};
