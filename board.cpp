#include <iostream>
#include <vector>
#include "board.h"

using namespace std;

static const Pos knightOffsets[8] = {{2, 1}, {1, 2}, {-2, 1}, {-1, 2}, {2, -1}, {1, -2}, {-2, -1}, {-1, -2}};

static const char startingBoard[8][8] = {
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
    memcpy(b, startingBoard, 8*8*sizeof(char));
    isWhitesTurn = true;
    movesPlayed = 0;

    whiteKingSideCastle = true;
    whiteQueenSideCastle = true;
    blackKingSideCastle = true;
    blackQueenSideCastle = true;
}

string Board::getPieceStr(int piece) {
    // abs switches black pieces to white
    switch (abs(piece)) {
        case whiteKing:
            return "K";
        case whiteQueen:
            return "Q";
        case whiteRook:
            return "R";
        case whiteBishop:
            return "B";
        case whiteKnight:
            return "N"; // K already used so N is standard replacement
        case whitePawn:
            return "P";
        default:
            return " ";
    }
}

string Board::moveToNotation(Move m) {
    // returns a string containing the current move in PGN format
    string from, to, out;
    // change number to letter by using ascii table offset (+97) 0 -> a, 1 -> b, etc.
    from = ((char) m.from.x + 97);
    from += to_string(m.from.y);
    to = ((char) m.to.x + 97);
    to += to_string(m.to.y);
    bool isCapture = getPos(m.to) != empty;
    int piece = getPos(m.from);

    // castling
    if (abs(piece) == whiteKing) {
        switch (m.flags) {
            case whiteKingSide:
            case blackKingSide:
                return "O-O";
            case whiteQueenSide:
            case blackQueenSide:
                return "O-O-O";
            default:
                break;
        }
    }

    if (abs(piece) == whitePawn) {
        if (isCapture)
            out = from.substr(0, 1) + "x" + to;
        else
            out = to;
        if (m.to.y == 0 || m.to.y == 7) { // promoting
            if (m.flags == whiteBishop || m.flags == whiteKnight || m.flags == whiteRook)
                out += "=" + getPieceStr(m.flags);
            else
                out += "=Q";
        }
    }
    else {
        out = getPieceStr(getPos(m.from));
        // TODO: add extra descriptor if multiple pieces can move to same spot
        if (isCapture)
            out += "x";
        out += to;
    }

    // TODO: check if doing this move causes a check or checkmate
    if (false) // checkmate
        out += "#";
    else if (false) // check
        out += "+";

    return out;
}

void Board::doMove(Move m) {
    // y and x reversed
    int piece = getPos(m.from);
    switch (piece) {
        // check for castling
        case whiteKing:
            if (whiteKingSideCastle && m.flags == whiteKingSide) {
                b[7][5] = whiteRook;
                b[7][7] = empty;
            }
            else if (whiteQueenSideCastle && m.flags == whiteQueenSide) {
                b[7][3] = whiteRook;
                b[7][0] = empty;
            }
            whiteKingSideCastle = false;
            whiteQueenSideCastle = false;
            break;
        case blackKing:
            if (blackKingSideCastle && m.flags == blackKingSide) {
                b[0][5] = blackRook;
                b[0][7] = empty;
            }
            else if (blackQueenSideCastle && m.flags == blackQueenSide) {
                b[0][3] = blackRook;
                b[0][0] = empty;
            }
            blackKingSideCastle = false;
            blackQueenSideCastle = false;
            break;
        case whiteRook:
            if (m.from.x == 0)
                blackQueenSideCastle = false;
            else if (m.from.x == 7)
                blackKingSideCastle = false;
            break;
        case blackRook:
            if (m.from.x == 0)
                blackQueenSideCastle = false;
            else if (m.from.x == 7)
                blackKingSideCastle = false;
            break;
        case whitePawn:
        case blackPawn:
            // en passant
            if (m.from.x != m.to.x && getPos(m.to) == empty)
                b[m.from.y][m.to.x] = empty;
            int a;
            if (m.to.y == 0 || m.to.y == 7) {
                switch (m.flags) {
                    case whiteRook:
                        a = whiteRook;
                        break;
                    case whiteBishop:
                        a = whiteBishop;
                        break;
                    case whiteKnight:
                        a = whiteKnight;
                        break;
                    default:
                        a = whiteQueen;
                        break;
                }
                // if the pawn was black, negate the piece, turning it black
                if (piece == blackPawn)
                    piece = -a;
                else
                    piece = a;
            }
            break;
    }
    b[m.to.y][m.to.x] = piece;
    b[m.from.y][m.from.x] = empty;
    isWhitesTurn = !isWhitesTurn;
    movesPlayed++;
    moveList.push_back(m);
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
        if (x.to.x == m.to.x && x.to.y == m.to.y)
            return true;
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
    if (isWhitesTurn != isPosWhite(p))
        return;
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
    bool isWhite = isPosWhite(p);
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (i != 0 && j != 0) {
                Pos next = {i, j};
                if (isSquareAvailable(next, isWhite))
                    moves->push_back((Move) {p, next});
            }
        }
    }
    // TODO: can't castle through check
    if (isWhite) {
        if (whiteKingSideCastle &&
            getPos((Pos) {p.x + 1, p.y}) == empty &&
            getPos((Pos) {p.x + 2, p.y}) == empty)
        {
            moves->push_back((Move) {p, {p.x + 2, p.y}, whiteKingSide});
        }
        if (whiteQueenSideCastle &&
            getPos((Pos) {p.x - 1, p.y}) == empty &&
            getPos((Pos) {p.x - 2, p.y}) == empty &&
            getPos((Pos) {p.x - 3, p.y}) == empty)
        {
            moves->push_back((Move) {p, {p.x - 2, p.y}, whiteQueenSide});
        }
    }
    else {
        if (blackKingSideCastle &&
            getPos((Pos) {p.x + 1, p.y}) == empty &&
            getPos((Pos) {p.x + 2, p.y}) == empty)
        {
            moves->push_back((Move) {p, {p.x + 2, p.y}, blackKingSide});
        }
        if (blackQueenSideCastle &&
            getPos((Pos) {p.x - 1, p.y}) == empty &&
            getPos((Pos) {p.x - 2, p.y}) == empty &&
            getPos((Pos) {p.x - 3, p.y}) == empty)
        {
            moves->push_back((Move) {p, {p.x - 2, p.y}, blackQueenSide});
        }
    }
}

void Board::getQueenMoves(vector<Move> *moves, Pos p) {
    getRookMoves(moves, p);
    getBishopMoves(moves, p);
}

void Board::getRookMoves(vector<Move> *moves, Pos p) {
    bool isWhite = isPosWhite(p);
    for (int i = 1; p.x + i < 8; i++) {
        Pos next = {p.x + i, p.y};
        if (isSquareAvailable(next, isWhite)) {
            moves->push_back((Move) {p, next});
        } else {break;}
    }
    for (int i = 1; p.x - i < 8; i++) {
        Pos next = {p.x - i, p.y};
        if (isSquareAvailable(next, isWhite)) {
            moves->push_back((Move) {p, next});
        } else {break;}
    }
    for (int i = 1; p.y + i < 8; i++) {
        Pos next = {p.x, p.y - i};
        if (isSquareAvailable(next, isWhite)) {
            moves->push_back((Move) {p, next});
        } else {break;}
    }
    for (int i = 1; p.y - i < 8; i++) {
        Pos next = {p.x, p.y - i};
        if (isSquareAvailable(next, isWhite)) {
            moves->push_back((Move) {p, next});
        } else {break;}
    }
}

void Board::getBishopMoves(vector<Move> *moves, Pos p) {
    bool isWhite = isPosWhite(p);
    for (int i = 1; ; i++) {
        Pos next = {p.x + i, p.y + i};
        if (posOnBoard(next) && isSquareAvailable(next, isWhite)) {
            moves->push_back((Move) {p, next});
        } else {break;}
    }
    for (int i = 1; ; i++) {
        Pos next = {p.x - i, p.y + i};
        if (posOnBoard(next) && isSquareAvailable(next, isWhite)) {
            moves->push_back((Move) {p, next});
        } else {break;}
    }
    for (int i = 1; ; i++) {
        Pos next = {p.x + i, p.y - i};
        if (posOnBoard(next) && isSquareAvailable(next, isWhite)) {
            moves->push_back((Move) {p, next});
        } else {break;}
    }
    for (int i = 1; ; i++) {
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
        if (isSquareAvailable(next, isWhite))
            moves->push_back((Move) {p, next});
    }
}

void Board::getPawnMoves(vector<Move> *moves, Pos p) {
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
    if (p.y == startingRank && getPos(next) == empty && getPos((Pos) {p.x, p.y + offset}) == empty)
        moves->push_back((Move) {p, next});
    // move 1 forward
    next = (Pos) {p.x, p.y + offset};
    if (getPos(next) == empty)
        moves->push_back((Move) {p, next});
    // capture left and right (and en passant)
    Move lastMove = (moveList.size() == 0) ? (Move) {{0, 0}, {0, 0}} : moveList.back();
    next = (Pos) {p.x - 1, p.y + offset};
    if ((getPos(next) != empty && isPosWhite(next) != isWhite) ||
        (lastMove.to.x == p.x - 1 && lastMove.to.y == p.y &&
        lastMove.from.y == p.y + offset * 2 &&
        abs(getPos(lastMove.to)) == whitePawn)
        )
    {
        moves->push_back((Move) {p, next});
    }
    next = (Pos) {p.x + 1, p.y + offset};
    if (getPos(next) != empty && isPosWhite(next) != isWhite ||
        (lastMove.to.x == p.x + 1 && lastMove.to.y == p.y &&
         lastMove.from.y == p.y + offset * 2 &&
         abs(getPos(lastMove.to)) == whitePawn)
         )
    {
        moves->push_back((Move) {p, next});
    }
}

void Board::printBoard() {
    Pos p;
    string pieceStr;
    cout << "    0 1 2 3 4 5 6 7\n";
    cout << "  *----------------";
    for (p.y = 0; p.y < 8; p.y++) {
        cout << "\n" << p.y << " | ";
        for (p.x = 0; p.x < 8; p.x++) {
            pieceStr = getPieceStr(getPos(p));
            if (!isPosWhite(p))
                pieceStr = tolower((char) pieceStr[0]);
            cout << pieceStr << " ";
        }
    }
    cout << "\n";
}

int main() {
    Board board;
    board.resetBoard();

    int x1, x2, y1, y2;
    while (true) {
        board.printBoard();

        cout << "From (enter -1 to exit):\n";
        cin >> x1;
        if (x1 == -1)
            break;
        cin >> y1;
        cout << "To:\n";
        cin >> x2;
        cin >> y2;
        Move m = {{x1, y1},{x2, y2}};
        cout << "\n" << board.moveToNotation(m) << "\n";
        if (board.isValidMove(m))
            board.doMove(m);
        else
            cout << "Invalid Move\n";

        cout << "\n";
    }
}