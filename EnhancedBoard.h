#pragma once

#include "Board.h"

class EnhancedBoard : public Board {
private:
    std::vector<Move> moveList;
    int gameStatus;
    int movesPlayed;
    std::vector<Move> validMoves;

    std::string moveToNotationInternal(Move m);

public:
    int doMove(Move m);

    bool isValidMove(Move m);

    std::vector<Move> * getAllValidMoves();

    std::string getPieceStr(char piece);

    Move notationToMove(std::string m);

    std::string moveToNotation(Move m);

    void exportToPGN(std::string filepath);

    void printBoard();

    char getPos(int pos);

    bool isPosWhite(int pos);

    Move getLastMove();

    int getStatus();
};
