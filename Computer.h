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

    int positionalEvalWhite(Board *b);

    int secondaryEvalBlack(Move m);

    int positionalEvalBlack(Board *b);

    int getPieceValue(char piece);

    int evalPosThreaded(Board *startingBoard, int depth, int min, int max);

    int threadedEval(Board *startingBoard, int depth, int alpha, int beta);

    void evalMove();
};

int evalPos(Board *startingBoard, int depth, int min, int max);

void evalBlack(Eval *e, Move m, char piece);

void evalWhite(Eval *e, Move m, char piece);

int popCount(uint64_t BB);

int doubledPawns(uint64_t pawnBB);

int getMaterialDiff(Board *b);

int getTotalMaterial(Board *b);

int scoreBoard(Board *b);


