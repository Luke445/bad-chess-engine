#pragma once
#include <queue>
#include "EnhancedBoard.h"
#include "Threads.h"

class ComputerBoard {
private:
    std::queue<Move> evalQueue;
    std::mutex queueMutex;
    
    std::vector<std::pair<Move, int>> moveEvals;
    std::mutex evalMutex;
    
    Threads *threadPool;
    int startDepth;
    bool isWhitePieces;

public:
    EnhancedBoard mainBoard;

    ComputerBoard(Threads *t, int d);

    int doComputerMove();

    int submitPlayerMove(Move m);

    Move selectRandomMove(std::vector<Move> *moves);

    int setValueFromStatus(int status);

    void sortEvals();

    Move bruteForce();

    int secondaryEval(Move m);

    int scoreBoard(Board *b);

    void evalMove();

    int evalPos(Board *startingBoard, int depth, int min, int max);
};
