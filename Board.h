#pragma once

#include <vector>

enum pieces {
    noPiece = 0,
    whiteKing = 1,
    whiteQueen = 2,
    whiteRook = 3,
    whiteBishop = 4,
    whiteKnight = 5,
    whitePawn = 6,
    blackKing = -1,
    blackQueen = -2,
    blackRook = -3,
    blackBishop = -4,
    blackKnight = -5,
    blackPawn = -6
};

enum gameStatusFlags {
    gameNotOver,
    whiteWins,
    blackWins,
    draw
};

struct Move {
    int from;
    int to;
    int flags; // used for promotion
};

class Board {
protected:
    char b[64];
    bool isWhitesTurn;
    Move lastMove;
    bool whiteKingSideCastle, whiteQueenSideCastle, blackKingSideCastle, blackQueenSideCastle;
    int blackKingPos;
    int whiteKingPos;
    std::vector<Move> validMoves;

public:
    Board();

    void quickCopy(Board *otherBoard);

    void resetBoard();

    int doMove(Move m);

    void quickDoMove(Move m);

    char getPos(int pos);

    bool isCheck();

    bool isWhiteInCheck();

    bool isBlackInCheck();

    //bool isWhiteInCheckNew();

    //bool isBlackInCheckNew();

    bool isWhiteSquare(int pos);

    bool isPosWhite(int pos);

    bool isValidMove(Move m);

    void getAllSimpleMoves(std::vector<Move> *moves);

    std::vector<Move> * getAllValidMoves();

    void getMovesForPiece(std::vector<Move> *moves, int pos);

    bool posOnBoard(int pos);

    bool isSquareAvailable(int pos, bool isWhite);

    void getKingMoves(std::vector<Move> *moves, int pos);

    void getQueenMoves(std::vector<Move> *moves, int pos);

    void getRookMoves(std::vector<Move> *moves, int pos);

    void getBishopMoves(std::vector<Move> *moves, int pos);

    void getKnightMoves(std::vector<Move> *moves, int pos);

    void getPawnMoves(std::vector<Move> *moves, int pos);

    int getPieceValue(char piece);

    int getMaterialDiff();

    bool getIsWhitesTurn();
};
