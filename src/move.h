#pragma once

struct Move {
    int from;
    int to;
    int captured;

    Move(int f, int t, int c = 0)
        : from(f), to(t), captured(c) {}
};
