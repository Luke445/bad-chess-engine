#pragma once

#include <vector>
#include <inttypes.h>

// get the rank or file from a position (0-63) by bit operations
#define RANK(pos) ((pos) >> 3) // rank is horizontal
#define FILE(pos) ((pos) & 7)  // file is vertical
#define POS_TO_BITMASK(pos) ((uint64_t)1 << (pos))

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
    draw,
    invalidMove
};

struct Move {
    int from;
    int to;
};

class Board {
protected:
    uint64_t whitePawnBB;
    uint64_t whiteKnightBB;
    uint64_t whiteBishopBB;
    uint64_t whiteRookBB;

    uint64_t blackPawnBB;
    uint64_t blackKnightBB;
    uint64_t blackBishopBB;
    uint64_t blackRookBB;

    int blackKingPos;
    int whiteKingPos;

    int enPassantTarget;
    bool isWhitesTurn;
    bool whiteKingSideCastle, whiteQueenSideCastle, blackKingSideCastle, blackQueenSideCastle;

public:
    Board();

    void resetBoard();

    void whiteCapturePos(uint64_t mask);

    void blackCapturePos(uint64_t mask);

    int doMove(Move m);

    int quickDoMoveWhite(Move m, char piece);

    int quickDoMoveBlack(Move m, char piece);

    bool isCheck();

    bool isWhiteInCheck(uint64_t allPiecesBB);

    bool isBlackInCheck(uint64_t allPiecesBB);

    void startMoveGenerator(std::function<void(Move, char)> func, int *stop);

    void getMovesForWhitePiece(std::function<void(Move, char)> func, int pos, uint64_t whiteBB, uint64_t blackBB, uint64_t allBB);
    
    void getMovesForBlackPiece(std::function<void(Move, char)> func, int pos, uint64_t whiteBB, uint64_t blackBB, uint64_t allBB);

    void getWhiteKingMoves(std::function<void(Move, char)> func, int pos, uint64_t allPiecesBB, uint64_t sameColorBB);

    void getBlackKingMoves(std::function<void(Move, char)> func, int pos, uint64_t allPiecesBB, uint64_t sameColorBB);

    void getRookMoves(std::function<void(Move, char)> func, int pos, uint64_t allPiecesBB, uint64_t sameColorBB);

    void getBishopMoves(std::function<void(Move, char)> func, int pos, uint64_t allPiecesBB, uint64_t sameColorBB);

    void getKnightMoves(std::function<void(Move, char)> func, int pos, uint64_t sameColorBB);

    void getWhitePawnMoves(std::function<void(Move, char)> func, int pos, uint64_t allPiecesBB, uint64_t oppositeColorBB);

    void getBlackPawnMoves(std::function<void(Move, char)> func, int pos, uint64_t allPiecesBB, uint64_t oppositeColorBB);

    int getMaterialDiff();

    bool getIsWhitesTurn();

    uint64_t getAllPiecesBB();
};
