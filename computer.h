#include "board.h"

class ComputerBoard {
public:
    Board mainBoard;

    ComputerBoard();

    int doComputerMove();

    int submitPlayerMove(Move m);

    Move selectRandomMove(std::vector<Move> *moves);

    Move bruteForce(int depth);

    int evalPos(Board *startingBoard, int depth, int min, int max);
};
