#include <iostream>
#include <random>
#include "board.h"
#include "computer.h"

using namespace std;

ComputerBoard::ComputerBoard() {
    mainBoard.resetBoard();
}

int ComputerBoard::doComputerMove() {
    Move m = bruteForce(4);

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

Move ComputerBoard::bruteForce(int depth) {
    vector<Move> moves(*mainBoard.getAllValidMoves());
    shuffle(moves.begin(), moves.end(), random_device());
    Board b;
    Move bestMove = moves[0];
    int bestScore = 1000;
    int value;
    for (Move m : moves) {
        b.copyFromOtherBoard(&mainBoard);
        b.doMove(m);
        value = evalPos(&b, depth - 1, -1000, 1000);
        if (value < bestScore) {
            bestScore = value;
            bestMove = m;
        }
    }
    return bestMove;
}

/*
 * implements an alpha-beta algorithm that scores the
 * given position based on material difference
 * depth - moves to search in plies
 */
int ComputerBoard::evalPos(Board *startingBoard, int depth, int alpha, int beta) {
    if (depth == 0)
        return startingBoard->getMaterialDiff();

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
            if (status != gameNotOver) {
                if (status == whiteWins)
                    value = 1000;
                else if (status == blackWins)
                    value = -1000;
                else
                    value = 0;
            }
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
            if (status != gameNotOver) {
                if (status == whiteWins)
                    value = 1000;
                else if (status == blackWins)
                    value = -1000;
                else
                    value = 0;
            }
            else
                value = min(value, evalPos(&b, depth - 1, alpha, beta));
            beta = min(beta, value);
            if (value <= alpha)
                break;
        }
        return value;
    }
}
