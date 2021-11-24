#include <iostream>
#include <random>
#include "Board.h"
#include "Threads.h"
#include "Computer.h"

using namespace std;
using namespace std::chrono;

// change depth here
const int DEPTH = 6;

ComputerBoard::ComputerBoard(Threads *t) {
    mainBoard.resetBoard();
    threadPool = t;
}

int ComputerBoard::doComputerMove() {
    auto start = high_resolution_clock::now();
    Move m = bruteForce(DEPTH);
    auto stop = high_resolution_clock::now();

    auto duration = duration_cast<milliseconds>(stop - start);

    // cout << "doComputerMove time: " << duration.count() << "ms" << endl;

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

    // prepare jobs
    for (int i = 0; i < moves->size(); i++)
        evalQueue.push(moves->at(i));
    function<void()> func = bind(&ComputerBoard::evalMove, this);
    for (int i = 0; i < moves->size(); i++)
        threadPool->addJob(func);

    // wait for done
    threadPool->waitUntilDone();

    // select best move
    pair<Move, int> best = moveEvals[0];
    for (pair<Move, int> curPair : moveEvals) {
        if (curPair.second < best.second)
            best = curPair;
    }

    // cleanup
    moveEvals.clear();

    return best.first;
}

int ComputerBoard::scoreBoard(Board *b) {
    return b->getMaterialDiff();
}

// shorthand callback for threads
void ComputerBoard::evalMove() {
    // get move SAFELY
    unique_lock<mutex> lock(queueLock);

    condition.wait(lock, [this]() {return !evalQueue.empty();});
    if (evalQueue.empty())
        return;
    Move m = evalQueue.front();
    evalQueue.pop();

    queueLock.unlock();

    // eval move
    int value, status;
    Board b(mainBoard);
    status = b.doMove(m);
    if (status != gameNotOver)
        value = setValueFromStatus(status);
    else
        value = evalPos(&b, DEPTH - 1, -1000, 1000);

    // submit value
    moveEvals.push_back(pair<Move, int> {m, value});
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
    int status, value;
    if (startingBoard->getIsWhitesTurn()) {
        value = -1000;
        for (Move m : *moves) {
            b.copyFromOtherBoard(startingBoard);
            status = b.doMove(m);
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
        value = 1000;
        for (Move m : *moves) {
            b.copyFromOtherBoard(startingBoard);
            status = b.doMove(m);
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
