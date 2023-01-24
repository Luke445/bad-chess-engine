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

    ComputerBoard(Threads *t, int d, bool isWhite);

    int doComputerMove();

    int submitPlayerMove(Move m);

    Move selectRandomMove(std::vector<Move> *moves);

    void sortEvals();

    Move bruteForce();

    int secondaryEvalWhite(Move m);

    int secondaryEvalBlack(Move m);

    int scoreBoard(Board *b);

    int getPieceValue(char piece);

    void evalMove();

    int evalPos(Board *startingBoard, int depth, int min, int max);
};
