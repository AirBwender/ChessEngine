#include <iostream>
#include "src/board.h"

using namespace std;

// convert 0-63 -> chess coords
string toCoord(int sq){
    string s = "";
    s += char('a' + col(sq));
    s += char('1' + row(sq));
    return s;
}

int main() {

    Board b;
    b.print();

    auto moves = generateAll(b);

    cout << "\nGenerated Moves: " 
         << moves.size() << "\n\n";

    for(auto &m : moves) {
        cout << toCoord(m.from)
             << " -> "
             << toCoord(m.to);

        if(m.flags & MOVE_CAPTURE)
            cout << "  capture!";

        if(m.flags & MOVE_PROMO)
            cout << "  promo!";

        if(m.flags & MOVE_EP)
            cout << "  ep!";

        if(m.flags & MOVE_CASTLE)
            cout << "  castle!";

        cout << "\n";
    }

    cout << "\nPerft (pseudo-legal):\n";
    for(int d = 1; d <= 3; d++) {
        long long n = perft(b, d);
        cout << "Depth " << d << ": " << n << "\n";
    }

    cout << "\nDivide (depth 3):\n";
    auto rootMoves = generateAll(b);
    long long total = 0;
    for(const auto &m : rootMoves) {
        makeMove(b, m);
        long long n = perft(b, 2);
        unmakeMove(b, m);
        total += n;
        cout << toCoord(m.from) << toCoord(m.to) << ": " << n << "\n";
    }
    cout << "Total: " << total << "\n";
}
