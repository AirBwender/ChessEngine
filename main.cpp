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

        if(m.captured != EMPTY)
            cout << "  capture!";

        cout << "\n";
    }
}
