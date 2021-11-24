#include <vector>
#include "Board.h"

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
    memcpy(b, oldBoard->b, 64 * sizeof(char));
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
                b[(m.from & 0b111000) + (m.to & 0b111)] = noPiece;
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
    bool out = false;
    for (int pos = 0; (pos < 64) && !out; pos++) {
        int piece = getPos(pos);
        if (piece == noPiece || piece > 0 == isWhitesTurn)
            continue;

        switch (abs(piece)) {
            case whiteKing:
                out = doesKingAttackKing(kingPos, pos);
                break;
            case whiteQueen:
                out = doesQueenAttackKing(kingPos, pos);
                break;
            case whiteRook:
                out = doesRookAttackKing(kingPos, pos);
                break;
            case whiteBishop:
                out = doesBishopAttackKing(kingPos, pos);
                break;
            case whiteKnight:
                out = doesKnightAttackKing(kingPos, pos);
                break;
            case whitePawn:
                out = doesPawnAttackKing(kingPos, pos);
                break;
        }
    }
    return out;
}

bool Board::doesKingAttackKing(char pos, char otherKingPos) {
    bool isWhite = isPosWhite(pos);
    char next;
    for (int i = -1; i < 2; i++) {
        for (int j = -1; j < 2; j++) {
            if (i != 0 || j != 0) {
                next = pos + (i * 8) + j;
                if (otherKingPos == next) {
                    if (abs((next & 0b111) - (pos & 0b111)) <= 1) {
                        if (isSquareAvailable(next, isWhite))
                            return true;
                    }
                }
            }
        }
    }
    // castling can't capture a piece
    return false;
}

bool Board::doesQueenAttackKing(char pos, char kingPos) {
    bool out = doesRookAttackKing(pos, kingPos);
    if (!out)
        out = doesBishopAttackKing(pos, kingPos);
    return out;
}

bool Board::doesRookAttackKing(char pos, char kingPos) {
    bool isWhite = isPosWhite(pos);
    char next;

    for (next = pos + 1; (next & 0b111) != 0; next++) {
        if (isSquareAvailable(next, isWhite)) {
            if (kingPos == next) {return true;}
            if (getPos(next) != noPiece)
                break;
        } else {break;}
    }
    for (next = pos - 1; (next & 0b111) != 7; next--) {
        if (isSquareAvailable(next, isWhite)) {
            if (kingPos == next) {return true;}
            if (getPos(next) != noPiece)
                break;
        } else {break;}
    }
    for (next = pos + 8; ; next += 8) {
        if (isSquareAvailable(next, isWhite)) {
            if (kingPos == next) { return true; }
            if (getPos(next) != noPiece)
                break;
        } else { break; }
    }
    for (next = pos - 8; ; next -= 8) {
        if (isSquareAvailable(next, isWhite)) {
            if (kingPos == next) {return true;}
            if (getPos(next) != noPiece)
                break;
        } else {break;}
    }

    return false;
}

bool Board::isWhiteSquare(char pos) {
    if (pos & 0b1000)
        return pos & 0b1;
    else
        return !(pos & 0b1);
}

bool Board::doesBishopAttackKing(char pos, char kingPos) {
    bool isWhite = isPosWhite(pos);
    char next;

    for (next = pos + 9; (next & 0b111) != 0; next += 9) {
        if (isSquareAvailable(next, isWhite)) {
            if (kingPos == next) {return true;}
            if (getPos(next) != noPiece)
                break;
        } else {break;}
    }
    for (next = pos + 7; (next & 0b111) != 7; next += 7) {
        if (isSquareAvailable(next, isWhite)) {
            if (kingPos == next) {return true;}
            if (getPos(next) != noPiece)
                break;
        } else {break;}
    }
    for (next = pos - 9; (next & 0b111) != 7; next -= 9) {
        if (isSquareAvailable(next, isWhite)) {
            if (kingPos == next) {return true;}
            if (getPos(next) != noPiece)
                break;
        } else {break;}
    }
    for (next = pos - 7; (next & 0b111) != 0; next -= 7) {
        if (isSquareAvailable(next, isWhite)) {
            if (kingPos == next) {return true;}
            if (getPos(next) != noPiece)
                break;
        } else {break;}
    }

    return false;
}

bool Board::doesKnightAttackKing(char pos, char kingPos) {
    bool isWhite = isPosWhite(pos);
    char next;
    for (int i = 0; i < 8; i++) {
        next = pos + knightOffsets[i];
        if (kingPos == next) {
            if (abs((next & 0b111) - (pos & 0b111)) <= 2) {
                if (isSquareAvailable(next, isWhite))
                   return true;
            }
        }
    }
    return false;
}

bool Board::doesPawnAttackKing(char pos, char kingPos) {
    bool isWhite = isPosWhite(pos);
    char offset, next;
    if (isWhite)
        offset = -8;
    else
        offset = 8;

    // capture left and right (not possible for en passant to also capture other piece)
    if ((pos & 0b111) != 0) {
        next = pos + offset - 1;
        if (getPos(next) != noPiece && isPosWhite(next) != isWhite)
            if (kingPos == next) {return true;}
    }
    if ((pos & 0b111) != 7) {
        next = pos + offset + 1;
        if (getPos(next) != noPiece && isPosWhite(next) != isWhite)
            if (kingPos == next) {return true;}
    }
    return false;
}

bool Board::isPosWhite(char pos) {
    return getPos(pos) > 0;
}

bool Board::isValidMove(Move m) {
    vector<Move> *moves = getAllValidMoves();
    for (Move x : *moves) {
        if (x.to == m.to && x.from == m.from)
            return true;
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
    char next;
    for (next = pos + 1; (next & 0b111) != 0; next++) {
        if (isSquareAvailable(next, isWhite)) {
            moves->push_back((Move) {pos, next});
            if (getPos(next) != noPiece)
                break;
        } else {break;}
    }
    for (next = pos - 1; (next & 0b111) != 7; next--) {
        if (isSquareAvailable(next, isWhite)) {
            moves->push_back((Move) {pos, next});
            if (getPos(next) != noPiece)
                break;
        } else {break;}
    }
    for (next = pos + 8; ; next += 8) {
        if (isSquareAvailable(next, isWhite)) {
            moves->push_back((Move) {pos, next});
            if (getPos(next) != noPiece)
                break;
        } else { break; }
    }
    for (next = pos - 8; ; next -= 8) {
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
