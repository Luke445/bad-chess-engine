#pragma once
#include <queue>
#include "EnhancedBoard.h"
#include "Threads.h"

class ComputerBoard {
private:
    std::queue<Move> evalQueue;
    std::mutex queueLock;
    std::condition_variable condition;
    std::vector<std::pair<Move, int>> moveEvals;
    Threads *threadPool;

public:
    EnhancedBoard mainBoard;

    ComputerBoard(Threads *t);

    int doComputerMove();

    int submitPlayerMove(Move m);

    Move selectRandomMove(std::vector<Move> *moves);

    int setValueFromStatus(int status);

    Move bruteForce(int depth);

    int scoreBoard(Board *b);

    void evalMove();

    int evalPos(Board *startingBoard, int depth, int min, int max);
};
