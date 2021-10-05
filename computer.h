#include "board.h"

class ComputerBoard {
public:
    Board mainBoard;

    ComputerBoard();

    int doComputerMove();

    int submitPlayerMove(Move m);
};