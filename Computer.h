#include "EnhancedBoard.h"

class ComputerBoard {
public:
    EnhancedBoard mainBoard;

    ComputerBoard();

    int doComputerMove();

    int submitPlayerMove(Move m);

    Move selectRandomMove(std::vector<Move> *moves);

    Move bruteForce(int depth);

    int scoreBoard(Board *b);

    int evalPos(Board *startingBoard, int depth, int min, int max);
};
