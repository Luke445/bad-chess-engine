#include <iostream>
#include <vector>
#include "board.h"

using namespace std;

static const char knightOffsets[8] = {17, 10, -15, -6, 15, 6, -17, -10};

static const char startingBoard[64] = {
    blackRook, blackKnight, blackBishop, blackQueen, blackKing, blackBishop, blackKnight, blackRook,
    blackPawn, blackPawn,   blackPawn,   blackPawn,  blackPawn, blackPawn,   blackPawn,   blackPawn,
    noPiece,   noPiece,     noPiece,     noPiece,    noPiece,   noPiece,     noPiece,     noPiece  ,
    noPiece,   noPiece,     noPiece,     noPiece,    noPiece,   noPiece,     noPiece,     noPiece  ,
    noPiece,   noPiece,     noPiece,     noPiece,    noPiece,   noPiece,     noPiece,     noPiece  ,
    noPiece,   noPiece,     noPiece,     noPiece,    noPiece,   noPiece,     noPiece,     noPiece  ,
    whitePawn, whitePawn,   whitePawn,   whitePawn,  whitePawn, whitePawn,   whitePawn,   whitePawn,
    whiteRook, whiteKnight, whiteBishop, whiteQueen, whiteKing, whiteBishop, whiteKnight, whiteRook
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

/*
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
    bool isCapture = getPos(m.to) != noPiece;
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
        for (int i = 0; i + 1 < moveList.size(); i+=2) {
            f << to_string(curMove) << ". " << moveToNotation(moveList.at(i)) << " " << moveToNotation(moveList.at(i + 1)) << " \n";
            curMove += 1;
        }
    }
    else
        cout << "failed to open output file\n";
    f.close();
}
*/

int Board::doMove(Move m, bool evalCheckmate) {
    int piece = getPos(m.from);
    switch (piece) {
        // check for castling
        case whiteKing:
            if (whiteKingSideCastle && m.to == 62) {
                b[61] = whiteRook;
                b[63] = noPiece;
            }
            else if (whiteQueenSideCastle && m.to == 58) {
                b[59] = whiteRook;
                b[56] = noPiece;
            }
            whiteKingSideCastle = false;
            whiteQueenSideCastle = false;
            break;
        case blackKing:
            if (blackKingSideCastle && m.to == 6) {
                b[5] = blackRook;
                b[7] = noPiece;
            }
            else if (blackQueenSideCastle && m.to == 2) {
                b[3] = blackRook;
                b[0] = noPiece;
            }
            blackKingSideCastle = false;
            blackQueenSideCastle = false;
            break;
        case whiteRook:
            if (m.from == 56)
                whiteQueenSideCastle = false;
            else if (m.from == 63)
                whiteKingSideCastle = false;
            break;
        case blackRook:
            if (m.from == 0)
                blackQueenSideCastle = false;
            else if (m.from == 7)
                blackKingSideCastle = false;
            break;
        case whitePawn:
        case blackPawn:
            // en passant
            if ((m.from & 0b111) != (m.to & 0b111) && getPos(m.to) == noPiece)
                b[m.from + (m.to & 0b111)] = noPiece;
            // promotion
            int a;
            if (m.to <= 7 || m.to >= 56) {
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
    b[m.to] = piece;
    b[m.from] = noPiece;
    isWhitesTurn = !isWhitesTurn;
    movesPlayed++;
    lastMove = m;
    validMoves.clear();

    if (evalCheckmate) {
        int numMoves = getAllValidMoves()->size();
        // stalemate
        if (numMoves == 0)
            gameStatus = draw;

        if (isCheck()) {
            if (numMoves == 0) {
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

int Board::getPos(char pos) {
    return b[pos];
}

bool Board::isCheck() {
    char kingPos = 0;
    for (; kingPos < 64; kingPos++) {
        if (abs(getPos(kingPos)) == whiteKing) {
            if (isPosWhite(kingPos) == isWhitesTurn)
                goto kingFound;
        }
    }
    return false;
kingFound:
    vector<Move> moves;
    isWhitesTurn = !isWhitesTurn;
    getAllSimpleMoves(&moves);
    isWhitesTurn = !isWhitesTurn;
    for (Move m : moves) {
        if (m.to == kingPos)
            return true;
    }
    return false;
}

bool Board::isPosWhite(char pos) {
    return getPos(pos) > 0;
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
            if (x.to == m.to)
                return true;
        }
    }

    return false;
}

void Board::getAllSimpleMoves(vector<Move> *moves) {
    for (int pos = 0; pos < 64; pos++)
        getMovesForPiece(moves, pos);
}

vector<Move> * Board::getAllValidMoves() {
    if (validMoves.empty()) {
        getAllSimpleMoves(&validMoves);

        Board newBoard;
        for (int i = 0; i < validMoves.size(); i++) {
            newBoard.copyFromOtherBoard(this);
            newBoard.doMove(validMoves.at(i), false);
            newBoard.isWhitesTurn = !newBoard.isWhitesTurn;
            if (newBoard.isCheck()) {
                validMoves.erase(validMoves.begin() + i);
                i--;
            }
        }
    }

    return &validMoves;
}

void Board::getMovesForPiece(vector<Move> *moves, char pos) {
    // if piece color is different from current turn, return
    int piece = getPos(pos);
    if (piece == noPiece || piece > 0 != isWhitesTurn)
        return;

    // abs switches black pieces to white
    switch (abs(piece)) {
        case whiteKing:
            getKingMoves(moves, pos);
            break;
        case whiteQueen:
            getQueenMoves(moves, pos);
            break;
        case whiteRook:
            getRookMoves(moves, pos);
            break;
        case whiteBishop:
            getBishopMoves(moves, pos);
            break;
        case whiteKnight:
            getKnightMoves(moves, pos);
            break;
        case whitePawn:
            getPawnMoves(moves, pos);
            break;
    }
}

bool Board::posOnBoard(char pos) {
    return pos >= 0 && pos <= 63;
}

bool Board::isSquareAvailable(char pos, bool isWhite) {
    int piece = getPos(pos);
    return posOnBoard(pos) && (piece == noPiece || (piece > 0 != isWhite));
}

// castle through / out of checks
void Board::getKingMoves(vector<Move> *moves, char pos) {
    bool isWhite = isPosWhite(pos);
    char next;
    for (int i = -1; i < 2; i++) {
        for (int j = -1; j < 2; j++) {
            if (i != 0 || j != 0) {
                next = pos + (i * 8) + j;
                if (abs((next & 0b111) - (pos & 0b111)) <= 1) {
                    if (isSquareAvailable(next, isWhite))
                        moves->push_back((Move) {pos, next});
                }
            }
        }
    }
    // TODO: can't castle through check
    if (isWhite) {
        if (whiteKingSideCastle &&
            getPos(61) == noPiece &&
            getPos(62) == noPiece &&
            getPos(63) == whiteRook)
        {
            moves->push_back((Move) {pos, 62});
        }
        if (whiteQueenSideCastle &&
            getPos(56) == whiteRook &&
            getPos(57) == noPiece &&
            getPos(58) == noPiece &&
            getPos(59) == noPiece)
        {
            moves->push_back((Move) {pos, 58});
        }
    }
    else {
        if (blackKingSideCastle &&
            getPos(5) == noPiece &&
            getPos(6) == noPiece &&
            getPos(7) == blackRook)
        {
            moves->push_back((Move) {pos, 6});
        }
        if (blackQueenSideCastle &&
            getPos(0) == blackRook &&
            getPos(1) == noPiece &&
            getPos(2) == noPiece &&
            getPos(3) == noPiece)
        {
            moves->push_back((Move) {pos, 2});
        }
    }
}

void Board::getQueenMoves(vector<Move> *moves, char pos) {
    getRookMoves(moves, pos);
    getBishopMoves(moves, pos);
}

void Board::getRookMoves(vector<Move> *moves, char pos) {
    bool isWhite = isPosWhite(pos);
    char next = pos + 1;
    for (; next % 8 != 0; next++) {
        if (isSquareAvailable(next, isWhite)) {
            moves->push_back((Move) {pos, next});
            if (getPos(next) != noPiece)
                break;
        } else {break;}
    }
    next = pos - 1;
    for (; (next % 8) + 1 != 8; next--) {
        if (isSquareAvailable(next, isWhite)) {
            moves->push_back((Move) {pos, next});
            if (getPos(next) != noPiece)
                break;
        } else {break;}
    }
    next = pos + 8;
    for (; ; next += 8) {
        if (isSquareAvailable(next, isWhite)) {
            moves->push_back((Move) {pos, next});
            if (getPos(next) != noPiece)
                break;
        } else {break;}
    }
    next = pos - 8;
    for (; ; next -= 8) {
        if (isSquareAvailable(next, isWhite)) {
            moves->push_back((Move) {pos, next});
            if (getPos(next) != noPiece)
                break;
        } else {break;}
    }
}

void Board::getBishopMoves(vector<Move> *moves, char pos) {
    bool isWhite = isPosWhite(pos);
    char next;
    for (next = pos + 9; (next & 7) != 0; next += 9) {
        if (isSquareAvailable(next, isWhite)) {
            moves->push_back((Move) {pos, next});
            if (getPos(next) != noPiece)
                break;
        } else {break;}
    }
    for (next = pos + 7; (next & 7) != 7; next += 7) {
        if (isSquareAvailable(next, isWhite)) {
            moves->push_back((Move) {pos, next});
            if (getPos(next) != noPiece)
                break;
        } else {break;}
    }
    for (next = pos - 9; (next & 7) != 7; next -= 9) {
        if (isSquareAvailable(next, isWhite)) {
            moves->push_back((Move) {pos, next});
            if (getPos(next) != noPiece)
                break;
        } else {break;}
    }
    for (next = pos - 7; (next & 7) != 0; next -= 7) {
        if (isSquareAvailable(next, isWhite)) {
            moves->push_back((Move) {pos, next});
            if (getPos(next) != noPiece)
                break;
        } else {break;}
    }
}

void Board::getKnightMoves(vector<Move> *moves, char pos) {
    bool isWhite = isPosWhite(pos);
    char next;
    for (int i = 0; i < 8; i++) {
        next = pos + knightOffsets[i];
        if (abs((next & 0b111) - (pos & 0b111)) <= 2) {
            if (isSquareAvailable(next, isWhite))
                moves->push_back((Move) {pos, next});
        }
    }
}

void Board::getPawnMoves(vector<Move> *moves, char pos) {
    bool isWhite = isPosWhite(pos);
    char offset;
    bool onStartingRank;
    if (isWhite) {
        offset = -8;
        onStartingRank = pos >= 48;
    }
    else {
        offset = 8;
        onStartingRank = pos <= 15;
    }
    // move 2 forward
    char next = pos + offset * 2;
    if (onStartingRank && getPos(next) == noPiece && getPos(pos + offset) == noPiece)
        moves->push_back((Move) {pos, next});
    // move 1 forward
    next = pos + offset;
    if (posOnBoard(next) && getPos(next) == noPiece) {
        Move m = {pos, next};
        if (next <= 7 || next >= 56) {
            m.flags = whiteQueen; moves->push_back(m);
            m.flags = whiteRook; moves->push_back(m);
            m.flags = whiteBishop; moves->push_back(m);
            m.flags = whiteKnight; moves->push_back(m);
        } else
            moves->push_back(m);
    }


    // capture left and right (and en passant)
    if ((pos & 0b111) != 0) {
        next = pos + offset - 1;
        if ((getPos(next) != noPiece && isPosWhite(next) != isWhite) ||
            (lastMove.to == pos - 1 &&
             lastMove.from >> 3 == (pos + offset * 2) >> 3 &&
             abs(getPos(lastMove.to)) == whitePawn)
             )
        {
            Move m = {pos, next};
            if (next <= 7 || next >= 56) {
                m.flags = whiteQueen; moves->push_back(m);
                m.flags = whiteRook; moves->push_back(m);
                m.flags = whiteBishop; moves->push_back(m);
                m.flags = whiteKnight; moves->push_back(m);
            } else
                moves->push_back(m);
        }
    }
    if ((pos & 0b111) != 7) {
        next = pos + offset + 1;
        if ((getPos(next) != noPiece && isPosWhite(next) != isWhite) ||
            (lastMove.to == pos + 1 &&
             lastMove.from >> 3 == (pos + offset * 2) >> 3 &&
             abs(getPos(lastMove.to)) == whitePawn)
                )
        {
            Move m = {pos, next};
            if (next <= 7 || next >= 56) {
                m.flags = whiteQueen; moves->push_back(m);
                m.flags = whiteRook; moves->push_back(m);
                m.flags = whiteBishop; moves->push_back(m);
                m.flags = whiteKnight; moves->push_back(m);
            } else
                moves->push_back(m);
        }
    }
}

void Board::printBoard() {
    string pieceStr;
    char pos = 0;
    cout << "    a b c d e f g h\n";
    cout << "  *----------------";
    for (pos = 0; pos < 64; pos += 8) {
        cout << "\n" << (8 - ((int) (pos / 8))) << " | ";
        for (int i = 0; i < 8; i++) {
            pieceStr = getPieceStr(getPos(pos + i));
            if (!isPosWhite(pos + i))
                pieceStr = tolower((char) pieceStr[0]);
            cout << pieceStr << " ";
        }
    }
    cout << "\n";
}

int Board::getPieceValue(int piece) {
    if (piece == noPiece)
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
    for (int pos = 0; pos < 64; pos++) {
        sum += getPieceValue(getPos(pos));
    }
    return sum;
}

int Board::getWhitesMaterial() {
    int sum = 0;
    for (int pos = 0; pos < 64; pos++) {
        if (isPosWhite(pos))
            sum += getPieceValue(getPos(pos));
    }
    return sum;
}

int Board::getBlacksMaterial() {
    int sum = 0;
    for (int pos = 0; pos < 64; pos++) {
        if (!isPosWhite(pos))
            sum += getPieceValue(getPos(pos));
    }
    return sum;
}

Move Board::getLastMove() {
    if (movesPlayed == 0)
        return {-1, -1, -1};
    else
        return lastMove;
}

bool Board::getIsWhitesTurn() {
    return isWhitesTurn;
}

int Board::getStatus() {
    return gameStatus;
}
