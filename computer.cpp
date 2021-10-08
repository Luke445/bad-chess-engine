#include "board.h"
#include "computer.h"

using namespace std;

ComputerBoard::ComputerBoard() {
    mainBoard.resetBoard();
    srand(time(NULL));
}

int ComputerBoard::doComputerMove() {
    vector<Move> moves;
    mainBoard.getAllValidMoves(&moves);

    int index = rand() % moves.size();

    Move m = moves[index];

    return mainBoard.doMove(m);
}

int ComputerBoard::submitPlayerMove(Move m) {
    int status;

    if (mainBoard.isValidMove(m)) {
        status = mainBoard.doMove(m);
        return status;
    }

    return -1;
}

