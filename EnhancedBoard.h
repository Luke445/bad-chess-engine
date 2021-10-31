#pragma once

#include "Board.h"

class EnhancedBoard : public Board {
public:
    std::string getPieceStr(int piece);

    Move notationToMove(std::string m);

    std::string moveToNotation(Move m);

    void exportToPGN(std::string filepath);

    void printBoard();
};
