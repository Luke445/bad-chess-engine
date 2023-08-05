#pragma once

#include <vector>
#include <inttypes.h>
#include <functional>

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
    int8_t from;
    int8_t to;
};

struct Eval;

class Board {
public:
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

    Board();

    void resetBoard();

    void whiteCapturePos(uint64_t mask);

    void blackCapturePos(uint64_t mask);

    int doMove(Move m);

    int doMoveWhite(Move m, char piece);

    int doMoveBlack(Move m, char piece);

    bool isCheck();

    bool isWhiteInCheck(uint64_t allPiecesBB);

    bool isBlackInCheck(uint64_t allPiecesBB);

    uint64_t getAllPiecesBB();

    void getAllMoves(void (*func)(Eval*, Move, char), Eval *e);

    void getAllMovesWhite(void (*func)(Eval*, Move, char), Eval *e);

    void getAllMovesBlack(void (*func)(Eval*, Move, char), Eval *e);

    int evalPosWhite(int depth, int alpha, int beta);

    int evalPosBlack(int depth, int alpha, int beta);
};

void printBB(uint64_t BB);

struct Eval {
    Board b;
    int value;
    int stop;
    Board *startingBoard;
    int depth;
    int alpha;
    int beta;
};
