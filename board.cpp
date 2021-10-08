#include <iostream>
#include <vector>
#include <fstream>
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

Board::Board() {}

Board::Board(Board *oldBoard) {
    copyFromOtherBoard(oldBoard);
}

void Board::copyFromOtherBoard(Board *oldBoard) {
    memcpy(b, oldBoard->b, 8 * 8 * sizeof(char));
    isWhitesTurn = oldBoard->isWhitesTurn;
    movesPlayed = oldBoard->movesPlayed;
    gameStatus = oldBoard->gameStatus;
    lastMove = oldBoard->lastMove;

    whiteKingSideCastle = oldBoard->whiteKingSideCastle;
    whiteQueenSideCastle = oldBoard->whiteQueenSideCastle;
    blackKingSideCastle = oldBoard->blackKingSideCastle;
    blackQueenSideCastle = oldBoard->blackQueenSideCastle;
}

void Board::resetBoard() {
    memcpy(b, startingBoard, 8*8*sizeof(char));
    isWhitesTurn = true;
    movesPlayed = 0;
    gameStatus = gameNotOver;
    lastMove = {};

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

Move Board::notationToMove(string m) {
    try {
        // remove trailing + or #
        if (!(m.compare(m.size() - 1, 1, "+") && m.compare(m.size() - 1, 1, "#")))
            m = m.substr(0, m.size() - 1);

        // castling
        if (m == "O-O") {
            if (isWhitesTurn)
                return {{4, 7}, {6, 7}};
            else
                return {{4, 0}, {6, 0}};
        } else if (m == "O-O-O") {
            if (isWhitesTurn)
                return {{4, 7}, {2, 7}};
            else
                return {{4, 0}, {2, 0}};
        }

        // check for promotion
        if (m.compare(m.size() - 2, 1, "=") == 0) {
            Pos to = {((char) m.at(m.size() - 4)) - 97, (8 - stoi(m.substr(m.size() - 3, 1)))};
            int promoteTo = whiteQueen;
            string str = m.substr(m.size() - 1, 1);
            if (str == "R")
                promoteTo = whiteRook;
            else if (str == "B")
                promoteTo = whiteBishop;
            else if (str == "N")
                promoteTo = whiteKnight;
            if (to.y == 7)
                return {{to.x, 6}, to, promoteTo};
            else if (to.y == 0)
                return {{to.x, 1}, to, promoteTo};
        }

        // convert notation position to coordinate (h7 -> (7, 7))
        Pos to = {((char) m.at(m.size() - 2)) - 97, (8 - stoi(m.substr(m.size() - 1, 1)))};
        m = m.substr(0, m.size() - 2);

        // remove x denoting capture if present
        if (m.size() != 0 && m.compare(m.size() - 1, 1, "x") == 0)
            m = m.substr(0, m.size() - 1);

        char descriptor;
        Pos from;
        vector <Move> moves;
        int a;
        // switch on remaining size
        switch (m.size()) {
            case 3:
                // contains a piece name and full coordinate extra descriptor
                from = {((char) m.at(m.size() - 2)) - 97, (8 - stoi(m.substr(m.size() - 1, 1)))};
                return {from, to};
            case 2:
                // contains a piece name and either a rank or file extra descriptor
                descriptor = m.at(m.size() - 1);
                if (descriptor >= 97 && descriptor <= (97 + 8)) {
                    int x = descriptor - 97;
                    for (int y = 0; y < 8; y++) {
                        if (getPieceStr(b[y][x]) == m.substr(0, 1)) {
                            if (isValidMove({{x, y}, to}))
                                return {{x, y}, to};
                        }
                    }
                } else {
                    int y = (8 - stoi(m.substr(m.size() - 1, 1)));
                    for (int x = 0; x < 8; x++) {
                        if (getPieceStr(b[y][x]) == m.substr(0, 1)) {
                            if (isValidMove({{x, y}, to}))
                                return {{x, y}, to};
                        }
                    }
                }
                break;
            case 1:
                // has just a piece name, or in if a pawn is capturing, a file letter
                a = ((char) m.at(m.size() - 1)) - 97;
                if (a >= 0 && a <= 7) {
                    if (isWhitesTurn)
                        return {{a, to.y + 1}, to};
                    else
                        return {{a, to.y - 1}, to};
                }
                for (int y = 0; y < 8; y++) {
                    for (int x = 0; x < 8; x++) {
                        if (getPieceStr(b[y][x]) == m.substr(0, 1)) {
                            if (isValidMove({{x, y}, to}))
                                return {{x, y}, to};
                        }
                    }
                }
            case 0:
                // pawn push
                if (isWhitesTurn) {
                    if (isValidMove((Move) {{to.x, to.y + 1}, to}))
                        return {{to.x, to.y + 1}, to};
                    else
                        return {{to.x, to.y + 2}, to};
                } else {
                    if (isValidMove((Move) {{to.x, to.y - 1}, to}))
                        return {{to.x, to.y - 1}, to};
                    else
                        return {{to.x, to.y - 2}, to};
                }
                break;
        }
    }
    catch (...) {}
    return {};
}

// missing extra descriptor
string Board::moveToNotation(Move m) {
    // returns a string containing the current move in PGN format
    string from, to, out;
    // change number to letter by using ascii table offset (+97) 0 -> a, 1 -> b, etc.
    from = ((char) m.from.x + 97);
    from += to_string(8 - m.from.y);
    to = ((char) m.to.x + 97);
    to += to_string(8 - m.to.y);
    bool isCapture = getPos(m.to) != empty;
    int piece = getPos(m.from);

    // castling
    if (piece == whiteKing) {
        if (whiteKingSideCastle && m.to.x == 6)
            out = "O-O";
        else if (whiteQueenSideCastle && m.to.x == 2)
            out = "O-O-O";
        goto end;
    }
    else if (piece == blackKing) {
        if (blackKingSideCastle && m.to.x == 6)
            out = "O-O";
        else if (blackQueenSideCastle && m.to.x == 2)
            out = "O-O-O";
        goto end;
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

end:
    Board newBoard = Board(this);
    int status = newBoard.doMove(m);
    if (newBoard.isCheck()) {
        if (status == whiteWins || status == blackWins) // checkmate
            out += "#";
        else // check
            out += "+";
    }

    return out;
}

// needs the new implementation of the moves list
void Board::exportToPGN(string filepath) {
    // TODO: export current game to a valid .pgn file
    ofstream f;
    f.open(filepath);
    if (f.is_open()) {
        f << "[Event \"Testing\"]\n";
        f << "[Site \"Somewhere in the US\"]\n";
        f << "[Date \"??\"]\n";
        f << "[Round \"0\"]\n";
        f << "[White \"Tester, 1\"]\n";
        f << "[Black \"Tester, 2\"]\n";
        f << "[Result \"*\"]\n\n";

        int curMove = 0;
        /*
        for (int i = 0; i + 1 < moveList.size(); i+=2) {
            f << to_string(curMove) << ". " << moveToNotation(moveList.at(i)) << " " << moveToNotation(moveList.at(i + 1)) << " \n";
            curMove += 1;
        }*/
    }
    else
        cout << "failed to open output file\n";
    f.close();
}

int Board::doMove(Move m, bool evalCheckmate) {
    int piece = getPos(m.from);
    switch (piece) {
        // check for castling
        case whiteKing:
            if (whiteKingSideCastle && m.to.x == 6) {
                b[7][5] = whiteRook;
                b[7][7] = empty;
            }
            else if (whiteQueenSideCastle && m.to.x == 2) {
                b[7][3] = whiteRook;
                b[7][0] = empty;
            }
            whiteKingSideCastle = false;
            whiteQueenSideCastle = false;
            break;
        case blackKing:
            if (blackKingSideCastle && m.to.x == 6) {
                b[0][5] = blackRook;
                b[0][7] = empty;
            }
            else if (blackQueenSideCastle && m.to.x == 2) {
                b[0][3] = blackRook;
                b[0][0] = empty;
            }
            blackKingSideCastle = false;
            blackQueenSideCastle = false;
            break;
        case whiteRook:
            if (m.from.x == 0)
                whiteQueenSideCastle = false;
            else if (m.from.x == 7)
                whiteKingSideCastle = false;
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
            // promotion
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
    // y and x reversed
    b[m.to.y][m.to.x] = piece;
    b[m.from.y][m.from.x] = empty;
    isWhitesTurn = !isWhitesTurn;
    movesPlayed++;
    lastMove = m;

    if (evalCheckmate) {
        if (isCheck()) {
            vector<Move> moves;
            getAllValidMoves(&moves);
            if (moves.size() == 0) {
                if (isWhitesTurn)
                    gameStatus = blackWins;
                else
                    gameStatus = whiteWins;
            }
        }

        return gameStatus;
    }
    else
        return gameNotOver;
}

int Board::getPos(Pos p) {
    // y and x reversed
    return b[p.y][p.x];
}

bool Board::isCheck() {
    Pos kingPos;
    for (int x = 0; x < 8; x++) {
        for (int y = 0; y < 8; y++) {
            kingPos = (Pos) {x, y};
            if (abs(getPos(kingPos)) == whiteKing) {
                if (isPosWhite(kingPos) == isWhitesTurn)
                    goto kingFound;
            }
        }
    }
    return false;
kingFound:
    vector<Move> moves;
    isWhitesTurn = !isWhitesTurn;
    getAllSimpleMoves(&moves);
    for (Move m : moves) {
        if (m.to.x == kingPos.x && m.to.y == kingPos.y) {
            isWhitesTurn = !isWhitesTurn;
            return true;
        }
    }
    isWhitesTurn = !isWhitesTurn;
    return false;
}

bool Board::isPosWhite(Pos p) {
    return getPos(p) > 0;
}

bool Board::isValidMove(Move m) {
    vector<Move> moves;
    getMovesForPiece(&moves, m.from);

    Board newBoard;
    for (Move x : moves) {
        newBoard.copyFromOtherBoard(this);
        newBoard.doMove(x, false);
        newBoard.isWhitesTurn = !newBoard.isWhitesTurn;
        if (!newBoard.isCheck()) {
            if (x.to.x == m.to.x && x.to.y == m.to.y)
                return true;
        }
    }

    return false;
}

void Board::getAllSimpleMoves(vector<Move> *moves) {
    for (int x = 0; x < 8; x++) {
        for (int y = 0; y < 8; y++) {
            Pos next = {x, y};
            getMovesForPiece(moves, next);
        }
    }
}

void Board::getAllValidMoves(vector<Move> *moves) {
    getAllSimpleMoves(moves);

    Board newBoard;
    for (int i = 0; i < moves->size(); i++) {
        newBoard.copyFromOtherBoard(this);
        newBoard.doMove(moves->at(i), false);
        newBoard.isWhitesTurn = !newBoard.isWhitesTurn;
        if (newBoard.isCheck()) {
            moves->erase(moves->begin() + i);
            i--;
        }
    }
}

void Board::getMovesForPiece(vector<Move> *moves, Pos p) {
    // if piece color is different from current turn, return
    if (isWhitesTurn != isPosWhite(p)) {
        return;
    }

    int piece = getPos(p);
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

// castle through / out of checks
void Board::getKingMoves(vector<Move> *moves, Pos p) {
    bool isWhite = isPosWhite(p);
    for (int i = -1; i < 2; i++) {
        for (int j = -1; j < 2; j++) {
            if (i != 0 || j != 0) {
                Pos next = {p.x + i, p.y + j};
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
            moves->push_back((Move) {p, {p.x + 2, p.y}});
        }
        else if (whiteQueenSideCastle &&
            getPos((Pos) {p.x - 1, p.y}) == empty &&
            getPos((Pos) {p.x - 2, p.y}) == empty &&
            getPos((Pos) {p.x - 3, p.y}) == empty)
        {
            moves->push_back((Move) {p, {p.x - 2, p.y}});
        }
    }
    else {
        if (blackKingSideCastle &&
            getPos((Pos) {p.x + 1, p.y}) == empty &&
            getPos((Pos) {p.x + 2, p.y}) == empty)
        {
            moves->push_back((Move) {p, {p.x + 2, p.y}});
        }
        else if (blackQueenSideCastle &&
            getPos((Pos) {p.x - 1, p.y}) == empty &&
            getPos((Pos) {p.x - 2, p.y}) == empty &&
            getPos((Pos) {p.x - 3, p.y}) == empty)
        {
            moves->push_back((Move) {p, {p.x - 2, p.y}});
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
            if (getPos(next) != empty)
                break;
        } else {break;}
    }
    for (int i = 1; p.x - i < 8; i++) {
        Pos next = {p.x - i, p.y};
        if (isSquareAvailable(next, isWhite)) {
            moves->push_back((Move) {p, next});
            if (getPos(next) != empty)
                break;
        } else {break;}
    }
    for (int i = 1; p.y + i < 8; i++) {
        Pos next = {p.x, p.y + i};
        if (isSquareAvailable(next, isWhite)) {
            moves->push_back((Move) {p, next});
            if (getPos(next) != empty)
                break;
        } else {break;}
    }
    for (int i = 1; p.y - i < 8; i++) {
        Pos next = {p.x, p.y - i};
        if (isSquareAvailable(next, isWhite)) {
            moves->push_back((Move) {p, next});
            if (getPos(next) != empty)
                break;
        } else {break;}
    }
}

void Board::getBishopMoves(vector<Move> *moves, Pos p) {
    bool isWhite = isPosWhite(p);
    for (int i = 1; ; i++) {
        Pos next = {p.x + i, p.y + i};
        if (isSquareAvailable(next, isWhite)) {
            moves->push_back((Move) {p, next});
            if (getPos(next) != empty)
                break;
        } else {break;}
    }
    for (int i = 1; ; i++) {
        Pos next = {p.x - i, p.y + i};
        if (isSquareAvailable(next, isWhite)) {
            moves->push_back((Move) {p, next});
            if (getPos(next) != empty)
                break;
        } else {break;}
    }
    for (int i = 1; ; i++) {
        Pos next = {p.x + i, p.y - i};
        if (isSquareAvailable(next, isWhite)) {
            moves->push_back((Move) {p, next});
            if (getPos(next) != empty)
                break;
        } else {break;}
    }
    for (int i = 1; ; i++) {
        Pos next = {p.x - i, p.y - i};
        if (isSquareAvailable(next, isWhite)) {
            moves->push_back((Move) {p, next});
            if (getPos(next) != empty)
                break;
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

// push extra moves for promotion
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
    // TODO: should push 4 moves for promotion
    next = (Pos) {p.x, p.y + offset};
    if (posOnBoard(next) && getPos(next) == empty)
        moves->push_back((Move) {p, next});
    // capture left and right (and en passant)
    next = (Pos) {p.x - 1, p.y + offset};
    if ((posOnBoard(next) && getPos(next) != empty && isPosWhite(next) != isWhite) ||
        (lastMove.to.x == p.x - 1 && lastMove.to.y == p.y &&
        lastMove.from.y == p.y + offset * 2 &&
        abs(getPos(lastMove.to)) == whitePawn)
        )
    {
        moves->push_back((Move) {p, next});
    }
    next = (Pos) {p.x + 1, p.y + offset};
    if ((posOnBoard(next) && getPos(next) != empty && isPosWhite(next) != isWhite) ||
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
    cout << "    a b c d e f g h\n";
    cout << "  *----------------";
    for (p.y = 0; p.y < 8; p.y++) {
        cout << "\n" << (8 - p.y) << " | ";
        for (p.x = 0; p.x < 8; p.x++) {
            pieceStr = getPieceStr(getPos(p));
            if (!isPosWhite(p))
                pieceStr = tolower((char) pieceStr[0]);
            cout << pieceStr << " ";
        }
    }
    cout << "\n";
}

void Board::printBoardGui() {
    Pos p;
    string pieceStr;
    cout << "----- NEW BOARD -----\n";
    for (p.y = 0; p.y < 8; p.y++) {
        for (p.x = 0; p.x < 8; p.x++) {
            pieceStr = getPieceStr(getPos(p));
            if (!isPosWhite(p))
                pieceStr = tolower((char) pieceStr[0]);
            cout << pieceStr << "-";
        }
        cout << "\n";
    }
}

int Board::getPieceValue(int piece) {
    if (piece == empty)
        return 0;

    int value;
    switch (abs(piece)) {
        case whiteQueen:
            value = 9;
            break;
        case whiteRook:
            value = 5;
            break;
        case whiteBishop:
            value = 3;
            break;
        case whiteKnight:
            value = 3;
            break;
        case whitePawn:
            value = 1;
            break;
    }

    if (piece < 0)
        return -value;
    else
        return value;
}

int Board::getMaterialDiff() {
    int sum = 0;
    for (int x = 0; x < 8; x++) {
        for (int y = 0; y < 8; y++) {
            sum += getPieceValue(getPos((Pos) {x, y}));
        }
    }
    return sum;
}

int Board::getWhitesMaterial() {
    int sum = 0;
    int piece;
    Pos next;
    for (int x = 0; x < 8; x++) {
        for (int y = 0; y < 8; y++) {
            next = (Pos) {x, y};
            if (isPosWhite(next))
                sum += getPieceValue(getPos(next));
        }
    }
    return sum;
}

int Board::getBlacksMaterial() {
    int sum = 0;
    int piece;
    Pos next;
    for (int x = 0; x < 8; x++) {
        for (int y = 0; y < 8; y++) {
            next = (Pos) {x, y};
            if (!isPosWhite(next))
                sum += getPieceValue(getPos(next));
        }
    }
    return sum;
}
