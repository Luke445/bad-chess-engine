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
    char from;
    char to;
    char flags; // used for castling and promotion
};

class Board {
private:
    char b[64];
    bool isWhitesTurn;
    int movesPlayed;
    bool whiteKingSideCastle, whiteQueenSideCastle, blackKingSideCastle, blackQueenSideCastle;
    Move lastMove;
    int gameStatus;
    std::vector<Move> validMoves;

public:
    Board();

    Board(Board *oldBoard);

    void copyFromOtherBoard(Board *oldBoard);

    void resetBoard();

    std::string getPieceStr(int piece);

    Move notationToMove(std::string m);

    std::string moveToNotation(Move m);

    void exportToPGN(std::string filepath);

    int doMove(Move m, bool evalCheckmate=true);

    int getPos(char pos);

    bool isCheck();

    bool isPosWhite(char pos);

    bool isValidMove(Move m);

    void getAllSimpleMoves(std::vector<Move> *moves);

    std::vector<Move> * getAllValidMoves();

    void getMovesForPiece(std::vector<Move> *moves, char pos);

    bool posOnBoard(char pos);

    bool isSquareAvailable(char pos, bool isWhite);

    void getKingMoves(std::vector<Move> *moves, char pos);

    void getQueenMoves(std::vector<Move> *moves, char pos);

    void getRookMoves(std::vector<Move> *moves, char pos);

    void getBishopMoves(std::vector<Move> *moves, char pos);

    void getKnightMoves(std::vector<Move> *moves, char pos);

    void getPawnMoves(std::vector<Move> *moves, char pos);

    void printBoard();

    int getPieceValue(int piece);

    int getMaterialDiff();

    int getWhitesMaterial();

    int getBlacksMaterial();

    Move getLastMove();

    bool getIsWhitesTurn();

    int getStatus();
};
