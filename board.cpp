#include <iostream>

using namespace std;

enum peices {
    empty = 0,
    whiteKing = 1,
    whiteQueen = 2,
    whiteRook = 3,
    whiteBishop = 4,
    whiteKnight = 5,
    whitePawn = 6,
    blackKing = 7,
    blackQueen = 8,
    blackRook = 9,
    blackBishop = 10,
    blackKnight = 11,
    blackPawn = 12
};

struct Move {
    int fromX;
    int fromY;
    int toX;
    int toY;
};

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
    bool isWhitesTurn = true;
    int getBoardSpot(int x, int y) {
        return b[y][x];
    }
    void resetBoard() {
        memcpy(b, startingBoard, 8*8*sizeof(int));
    }

    bool isValidMove(Move m) {
        return true;
    }

    void printBoard() {
        for (int x = 0; x < 8; x++) {
            for (int y = 0; y < 8; y++) {
                cout << hex << b[x][y] << " ";
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
    board.printBoard();
}