enum pieces {
    empty = 0,
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

enum castlingFlags {
    whiteKingSide,
    whiteQueenSide,
    blackKingSide,
    blackQueenSide
};

enum gameStatusFlags {
    gameNotOver,
    whiteWins,
    blackWins,
    draw
};

typedef struct {
    int x;
    int y;
} Pos;

typedef struct {
    Pos from;
    Pos to;
    int flags; // used for castling and promotion
} Move;

class Board {
private:
    char b[8][8];
    bool isWhitesTurn;
    int movesPlayed;
    bool whiteKingSideCastle, whiteQueenSideCastle, blackKingSideCastle, blackQueenSideCastle;
    std::vector<Move> moveList;
    int gameStatus;

public:
    Board();

    Board(Board *oldBoard);

    void copyFromOtherBoard(Board *oldBoard);

    void resetBoard();

    std::string getPieceStr(int piece);

    std::string moveToNotation(Move m);

    int doMove(Move m);

    int getPos(Pos p);

    bool isCheck();

    bool isPosWhite(Pos p);

    bool isValidMove(Move m);

    void getAllSimpleMoves(std::vector<Move> *moves);

    void getAllValidMoves(std::vector<Move> *moves);

    void getMovesForPiece(std::vector<Move> *moves, Pos p);

    bool posOnBoard(Pos p);

    bool isSquareAvailable(Pos p, bool isWhite);

    void getKingMoves(std::vector<Move> *moves, Pos p);

    void getQueenMoves(std::vector<Move> *moves, Pos p);

    void getRookMoves(std::vector<Move> *moves, Pos p);

    void getBishopMoves(std::vector<Move> *moves, Pos p);

    void getKnightMoves(std::vector<Move> *moves, Pos p);

    void getPawnMoves(std::vector<Move> *moves, Pos p);

    void printBoard();
};

int main();