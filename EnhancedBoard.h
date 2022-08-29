#pragma once

#include "Board.h"

class EnhancedBoard : public Board {
private:
    std::vector<Move> moveList;
    int gameStatus;
    int movesPlayed;

public:
    int doMove(Move m);

    std::string getPieceStr(char piece);

    Move notationToMove(std::string m);

    std::string moveToNotationNoUpdate(Move m);

    std::string moveToNotation(Move m);

    void exportToPGN(std::string filepath);

    void printBoard();

    int getStatus();

    Move getLastMove();
};
