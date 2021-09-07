#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

enum peices {
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

typedef struct {
    int x;
    int y;
} Pos;

typedef struct {
    Pos from;
    Pos to;
} Move;

static const Pos knightOffsets[8] = {{2, 1}, {1, 2}, {-2, 1}, {-1, 2}, {2, -1}, {1, -2}, {-2, -1}, {-1, -2}};

static const int startingBoard[8][8] = {
    {blackRook, blackKnight, blackBishop, blackQueen, blackKing, blackBishop, blackKnight, blackRook},
    {blackPawn, blackPawn,   blackPawn,   blackPawn,  blackPawn, blackPawn,   blackPawn,   blackPawn},
    {empty,     empty,       empty,       empty,      empty,     empty,       empty,       empty    },
    {empty,     empty,       empty,       empty,      empty,     empty,       empty,       empty    },
    {empty,     empty,       empty,       empty,      empty,     empty,       empty,       empty    },
    {empty,     empty,       empty,       empty,      empty,     empty,       empty,       empty    },
    {whitePawn, whitePawn,   whitePawn,   whitePawn,  whitePawn, whitePawn,   whitePawn,   whitePawn},
    {whiteRook, whiteKnight, whiteBishop, whiteQueen, whiteKing, whiteBishop, whiteKnight, whiteRook}
};

class Board {
public:
    int b[8][8];
    bool isWhitesTurn;
    int getBoardSpot(int x, int y) {
        return b[y][x];
    }
    void resetBoard() {
        memcpy(b, startingBoard, 8*8*sizeof(int));
        isWhitesTurn = true;
    }
    
    char getPeiceChar(int peice) {
        switch (abs(peice)) {
            case 1:
                return 'K';
            case 2:
                return 'Q';
            case 3:
                return 'R';
            case 4:
                return 'B';
            case 5:
                return 'N';
            case 6:
                return 'P';
            default:
                return ' ';
        }
    }
    
    char *moveToNotation(Move m) {
        // TODO
        return "";
    }
    
    void doMove(Move m) {
        b[m.to.x][m.to.y] = b[m.from.x][m.from.y];
        b[m.from.x][m.from.y] = 0;
    }

    bool isValidMove(Move m) {
        vector<Move> moves;
        getMovesForPiece(moves, m.from);
        for (Move x : moves) {
            if (x.to.x == m.to.x && x.to.y == m.to.y) {
                return true;
            }
        }
        return false;
    }
 
    vector<Move> getAllValidMoves() {
        // needs to limit moves if the move causes a check
        vector<Move> out;
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                Pos next = {i, j};
                getMovesForPiece(out, next);
            }
        }
        return out;
    }
    
    void getMovesForPiece(vector<Move> moves, Pos p) {
        int peice = b[p.x][p.y];
        switch (abs(b[p.x][p.y])) {
            case 1:
                getKingMoves(moves, p);
                break;
            case 2:
                getQueenMoves(moves, p);
                break;
            case 3:
                getRookMoves(moves, p);
                break;
            case 4:
                getBishopMoves(moves, p);
                break;
            case 5:
                getKnightMoves(moves, p);
                break;
            case 6:
                getPawnMoves(moves, p);
                break;
            default:
                break;
        }
    }
    
    bool isSquareAvaliable(Pos p, bool isWhite) {
        int peice = b[p.x][p.y];
        if (peice == 0 || (peice > 0 != isWhite)) {
            return true;
        }
        return false;
    }
    
    bool posOnBoard(Pos p) {
        if (p.x >= 0 && p.x <= 7 && p.y >= 0 && p.y <= 7) {
            return true;
        }
        return false;
    }
    
    void getKingMoves(vector<Move> moves, Pos p) {
        // needs castling
        bool isWhite = b[p.x][p.y] > 0;
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                if (i != 0 && j != 0) {
                    Pos next = {i, j};
                    if (isSquareAvaliable(next, isWhite)) {
                        moves.push_back((Move) {p, next});
                    }
                }
            }
        }
    }
    
    void getQueenMoves(vector<Move> moves, Pos p) {
        getRookMoves(moves, p);
        getBishopMoves(moves, p);
    }
    
    void getRookMoves(vector<Move> moves, Pos p) {
        bool isWhite = b[p.x][p.y] > 0;
        for (int i = 0; p.x + i < 8; i++) {
            Pos next = {p.x + i, p.y};
            if (isSquareAvaliable(next, isWhite)) {
                moves.push_back((Move) {p, next});
            } else {break;}
        }
        for (int i = 0; p.x - i < 8; i++) {
            Pos next = {p.x - i, p.y};
            if (isSquareAvaliable(next, isWhite)) {
                moves.push_back((Move) {p, next});
            } else {break;}
        }
        for (int i = 0; p.y + i < 8; i++) {
            Pos next = {p.x, p.y - i};
            if (isSquareAvaliable(next, isWhite)) {
                moves.push_back((Move) {p, next});
            } else {break;}
        }
        for (int i = 0; p.y - i < 8; i++) {
            Pos next = {p.x, p.y - i};
            if (isSquareAvaliable(next, isWhite)) {
                moves.push_back((Move) {p, next});
            } else {break;}
        }
    }
    
    void getBishopMoves(vector<Move> moves, Pos p) {
        bool isWhite = b[p.x][p.y] > 0;
        for (int i = 0; ; i++) {
            Pos next = {p.x + i, p.y + i};
            if (posOnBoard(next) && isSquareAvaliable(next, isWhite)) {
                moves.push_back((Move) {p, next});
            } else {break;}
        }
        for (int i = 0; ; i++) {
            Pos next = {p.x - i, p.y + i};
            if (posOnBoard(next) && isSquareAvaliable(next, isWhite)) {
                moves.push_back((Move) {p, next});
            } else {break;}
        }
        for (int i = 0; ; i++) {
            Pos next = {p.x + i, p.y - i};
            if (posOnBoard(next) && isSquareAvaliable(next, isWhite)) {
                moves.push_back((Move) {p, next});
            } else {break;}
        }
        for (int i = 0; ; i++) {
            Pos next = {p.x - i, p.y - i};
            if (posOnBoard(next) && isSquareAvaliable(next, isWhite)) {
                moves.push_back((Move) {p, next});
            } else {break;}
        }
    }
    
    void getKnightMoves(vector<Move> moves, Pos p) {
        bool isWhite = b[p.x][p.y] > 0;
        for (int i = 0; i < 8; i++) {
            Pos offset = knightOffsets[i];
            Pos next = {p.x + offset.x, p.y + offset.y};
            if (isSquareAvaliable(next, isWhite)) {
                moves.push_back((Move) {p, next});
            } 
        }
    }
    
    void getPawnMoves(vector<Move> moves, Pos p) {
        // add captures, 2 forward on first move, en passant, promotion
        bool isWhite = b[p.x][p.y] > 0;
        Pos next = {p.x, p.y + 1};
        if (isWhite && isSquareAvaliable(next, isWhite)) {
            moves.push_back((Move) {p, next});
            return;
        }
        next = (Pos) {p.x, p.y - 1};
        if (!isWhite && isSquareAvaliable(next, isWhite)) {
            moves.push_back((Move) {p, next});
            return;
        }
    }
    
    void printBoard() {
        for (int x = 0; x < 8; x++) {
            for (int y = 0; y < 8; y++) {
                cout << getPeiceChar(b[x][y]) << " ";
            }
            cout << "\n";
        }
    }
};

int main()
{
    Board board;
    board.resetBoard();
    int x = board.getBoardSpot(0, 0);
    board.getAllValidMoves();
    board.printBoard();
}