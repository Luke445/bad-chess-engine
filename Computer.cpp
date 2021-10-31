#include <iostream>
#include <random>
#include "Board.h"
#include "Computer.h"

using namespace std;
using namespace std::chrono;

ComputerBoard::ComputerBoard() {
    mainBoard.resetBoard();
}

int ComputerBoard::doComputerMove() {
    auto start = high_resolution_clock::now();
    // change depth here
    Move m = bruteForce(4);
    auto stop = high_resolution_clock::now();

    auto duration = duration_cast<milliseconds>(stop - start);

    cout << "doComputerMove time: " << duration.count() << "ms" << endl;

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

Move ComputerBoard::selectRandomMove(vector<Move> *moves) {
    int index = rand() % moves->size();
    return moves->at(index);
}

int ComputerBoard::setValueFromStatus(int status) {
    if (status == whiteWins)
        return 1000;
    else if (status == blackWins)
        return -1000;
    else
        return 0;
}

Move ComputerBoard::bruteForce(int depth) {
    vector<Move> *moves = mainBoard.getAllValidMoves();
    // randomize moves with the same score
    shuffle(moves->begin(), moves->end(), random_device());
    Board b;
    Move bestMove = moves->at(0);
    int bestScore = 1000;
    int value, status;
    for (Move m : *moves) {
        b.copyFromOtherBoard(&mainBoard);
        status = b.doMove(m);
        if (status != gameNotOver)
            value = setValueFromStatus(status);
        else
            value = evalPos(&b, depth - 1, -1000, 1000);

        if (value < bestScore) {
            bestScore = value;
            bestMove = m;
        }
    }
    return bestMove;
}

int ComputerBoard::scoreBoard(Board *b) {
    return b->getMaterialDiff();
}

/*
 * implements an alpha-beta algorithm that scores the
 * given position based on material difference
 * depth - moves to search in plies
 */
int ComputerBoard::evalPos(Board *startingBoard, int depth, int alpha, int beta) {
    if (depth == 0)
        return scoreBoard(startingBoard);

    vector<Move> *moves = startingBoard->getAllValidMoves();
    if (moves->size() == 0)
        return 0;

    Board b;
    if (startingBoard->getIsWhitesTurn()) {
        int value = -1000;
        Move bestMove = moves->at(0);
        for (Move m : *moves) {
            b.copyFromOtherBoard(startingBoard);
            int status = b.doMove(m);
            if (status != gameNotOver)
                value = setValueFromStatus(status);
            else
                value = max(value, evalPos(&b, depth - 1, alpha, beta));

            alpha = max(alpha, value);
            if (value >= beta)
                break;
        }
        return value;
    }
    else {
        int value = 1000;
        Move bestMove = moves->at(0);
        for (Move m : *moves) {
            b.copyFromOtherBoard(startingBoard);
            int status = b.doMove(m);
            if (status != gameNotOver)
                value = setValueFromStatus(status);
            else
                value = min(value, evalPos(&b, depth - 1, alpha, beta));

            beta = min(beta, value);
            if (value <= alpha)
                break;
        }
        return value;
    }
}
