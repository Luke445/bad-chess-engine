#include "board.h"
#include <iostream>
#include "computer.h"

using namespace std;

ComputerBoard::ComputerBoard() {
    mainBoard.resetBoard();
    srand(time(NULL));
}

int ComputerBoard::doComputerMove() {
    vector<Move> moves;
    mainBoard.getAllValidMoves(&moves);

    int max = moves.size();
    int index = rand() % (max + 1);

    Move m = moves[index];

    cout << "Computer Move: " << mainBoard.moveToNotation(m) << "\n";

    return mainBoard.doMove(m);
}

int ComputerBoard::submitPlayerMove(Move m) {
    int status;

    if (mainBoard.isValidMove(m)) {
        status = mainBoard.doMove(m);

        if (status != gameNotOver) {
            mainBoard.printBoard();
            if (status == whiteWins)
                cout << "White Wins!\n";
            else if (status == blackWins)
                cout << "Black Wins!\n";
            else
                cout << "the game is a draw\n";
        }
        return status;
    }
    else {
        cout << "Invalid Move\n";
        return -1;
    }

    return gameNotOver;
}

