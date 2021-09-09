#include <iostream>
#include <vector>
#include "board.h"

using namespace std;

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

void Board::resetBoard() {
    memcpy(b, startingBoard, 8*8*sizeof(int));
    isWhitesTurn = true;
    movesPlayed = 0;
}

char Board::getPieceChar(int piece) {
    // abs switches black pieces to white
    switch (abs(piece)) {
        case whiteKing:
            return 'K';
        case whiteQueen:
            return 'Q';
        case whiteRook:
            return 'R';
        case whiteBishop:
            return 'B';
        case whiteKnight:
            return 'N'; // K already used so N is standard replacement
        case whitePawn:
            return 'P';
        default:
            return ' ';
    }
}

string Board::moveToNotation(Move m) {
    // TODO: returns the chess notation of what this move would be on the current board
    string from, to, out;
    // change number to letter by using ascii table offset (+97)
    from = ((char) m.from.x + 97);
    from += to_string(m.from.y);
    to = ((char) m.to.x + 97);
    to += to_string(m.to.y);
    bool isCapture = getPos(m.to) != empty;

    out = from + to;
    return out;
}

void Board::doMove(Move m) {
    // y and x reversed
    b[m.to.y][m.to.x] = b[m.from.y][m.from.x];
    b[m.from.y][m.from.x] = empty;
    isWhitesTurn = !isWhitesTurn;
    movesPlayed++;
}

int Board::getPos(Pos p) {
    // y and x reversed
    return b[p.y][p.x];
}

bool Board::isPosWhite(Pos p) {
    return getPos(p) > 0;
}

bool Board::isValidMove(Move m) {
    vector<Move> moves;
    getMovesForPiece(&moves, m.from);
    for (Move x : moves) {
        if (x.to.x == m.to.x && x.to.y == m.to.y) {
            return true;
        }
    }
    return false;
}

void Board::getAllValidMoves(vector<Move> *moves) {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            Pos next = {i, j};
            getMovesForPiece(moves, next);
        }
    }
}

void Board::getMovesForPiece(vector<Move> *moves, Pos p) {
    // TODO: needs to account for checks
    int piece = getPos(p);
    // if piece color is different from current turn, return
    if (isWhitesTurn != isPosWhite(p)) {
        return;
    }
    // abs switches black pieces to white
    switch (abs(piece)) {
        case whiteKing:
            getKingMoves(moves, p);
            break;
        case whiteQueen:
            getQueenMoves(moves, p);
            break;
        case whiteRook:
            getRookMoves(moves, p);
            break;
        case whiteBishop:
            getBishopMoves(moves, p);
            break;
        case whiteKnight:
            getKnightMoves(moves, p);
            break;
        case whitePawn:
            getPawnMoves(moves, p);
            break;
        default:
            break;
    }
}

bool Board::posOnBoard(Pos p) {
    return p.x >= 0 && p.x <= 7 && p.y >= 0 && p.y <= 7;
}

bool Board::isSquareAvailable(Pos p, bool isWhite) {
    int piece = getPos(p);
    return posOnBoard(p) && (piece == 0 || (piece > 0 != isWhite));
}

void Board::getKingMoves(vector<Move> *moves, Pos p) {
    // TODO: needs castling
    bool isWhite = isPosWhite(p);
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (i != 0 && j != 0) {
                Pos next = {i, j};
                if (isSquareAvailable(next, isWhite)) {
                    moves->push_back((Move) {p, next});
                }
            }
        }
    }
}

void Board::getQueenMoves(vector<Move> *moves, Pos p) {
    getRookMoves(moves, p);
    getBishopMoves(moves, p);
}

void Board::getRookMoves(vector<Move> *moves, Pos p) {
    bool isWhite = isPosWhite(p);
    for (int i = 0; p.x + i < 8; i++) {
        Pos next = {p.x + i, p.y};
        if (isSquareAvailable(next, isWhite)) {
            moves->push_back((Move) {p, next});
        } else {break;}
    }
    for (int i = 0; p.x - i < 8; i++) {
        Pos next = {p.x - i, p.y};
        if (isSquareAvailable(next, isWhite)) {
            moves->push_back((Move) {p, next});
        } else {break;}
    }
    for (int i = 0; p.y + i < 8; i++) {
        Pos next = {p.x, p.y - i};
        if (isSquareAvailable(next, isWhite)) {
            moves->push_back((Move) {p, next});
        } else {break;}
    }
    for (int i = 0; p.y - i < 8; i++) {
        Pos next = {p.x, p.y - i};
        if (isSquareAvailable(next, isWhite)) {
            moves->push_back((Move) {p, next});
        } else {break;}
    }
}

void Board::getBishopMoves(vector<Move> *moves, Pos p) {
    bool isWhite = isPosWhite(p);
    for (int i = 0; ; i++) {
        Pos next = {p.x + i, p.y + i};
        if (posOnBoard(next) && isSquareAvailable(next, isWhite)) {
            moves->push_back((Move) {p, next});
        } else {break;}
    }
    for (int i = 0; ; i++) {
        Pos next = {p.x - i, p.y + i};
        if (posOnBoard(next) && isSquareAvailable(next, isWhite)) {
            moves->push_back((Move) {p, next});
        } else {break;}
    }
    for (int i = 0; ; i++) {
        Pos next = {p.x + i, p.y - i};
        if (posOnBoard(next) && isSquareAvailable(next, isWhite)) {
            moves->push_back((Move) {p, next});
        } else {break;}
    }
    for (int i = 0; ; i++) {
        Pos next = {p.x - i, p.y - i};
        if (posOnBoard(next) && isSquareAvailable(next, isWhite)) {
            moves->push_back((Move) {p, next});
        } else {break;}
    }
}

void Board::getKnightMoves(vector<Move> *moves, Pos p) {
    bool isWhite = isPosWhite(p);
    for (int i = 0; i < 8; i++) {
        Pos offset = knightOffsets[i];
        Pos next = {p.x + offset.x, p.y + offset.y};
        if (isSquareAvailable(next, isWhite)) {
            moves->push_back((Move) {p, next});
        }
    }
}

void Board::getPawnMoves(vector<Move> *moves, Pos p) {
    // TODO: en passant, promotion
    bool isWhite = isPosWhite(p);
    int offset, startingRank;
    if (isWhite) {
        offset = -1;
        startingRank = 6;
    }
    else {
        offset = 1;
        startingRank = 1;
    }
    // move 2 forward
    Pos next = {p.x, p.y + offset * 2};
    if (p.y == startingRank && getPos(next) == empty && getPos((Pos) {p.x, p.y + offset}) == empty) {
        moves->push_back((Move) {p, next});
    }
    // move 1 forward
    next = (Pos) {p.x, p.y + offset};
    if (getPos(next) == empty) {
        moves->push_back((Move) {p, next});
    }
    // capture left and right
    next = (Pos) {p.x - 1, p.y + offset};
    if (getPos(next) != empty && isPosWhite(next) != isWhite) {
        moves->push_back((Move) {p, next});
    }
    next = (Pos) {p.x + 1, p.y + offset};
    if (getPos(next) != empty && isPosWhite(next) != isWhite) {
        moves->push_back((Move) {p, next});
    }
}

void Board::printBoard() {
    for (int x = 0; x < 8; x++) {
        for (int y = 0; y < 8; y++) {
            cout << getPieceChar(b[x][y]) << " ";
        }
        cout << "\n";
    }
}

int main()
{
    Board board;
    board.resetBoard();
    
    board.printBoard();

    int x1, x2, y1, y2;
    string c;
    while (true) {
        cout << "From:\n";
        cin >> x1;
        cin >> y1;
        cout << "To:\n";
        cin >> x2;
        cin >> y2;
        cout << "{" << x1 << " " << y1 << "}, {" << x2 << " " << y2 << "}\n";
        Move m = {{x1, y1},
                  {x2, y2}};
        cout << board.moveToNotation(m) << "\n";
        if (board.isValidMove(m)) {
            board.doMove(m);
        }
        else {
            cout << "Invalid Move\n";
        }

        cout << "\n";
        board.printBoard();

        cout << "continue? (y/n): ";
        cin >> c;
        if (c == "n") {
            break;
        }
    }
}