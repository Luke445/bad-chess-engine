#include <vector>
#include <iostream>
#include "Board.h"
#include "Data.h"

using namespace std;


Board::Board() {}

void Board::resetBoard() {
    whiteKingSideCastle = false;
    whiteQueenSideCastle = false;
    blackKingSideCastle = false;
    blackQueenSideCastle = false;
    whiteRookBB = 0;
    blackRookBB = 0;
    whiteBishopBB = 0;
    blackBishopBB = 0;
    whiteKnightBB = 0;
    blackKnightBB = 0;
    whitePawnBB = 0;
    blackPawnBB = 0;
    whiteKingPos = 64;
    blackKingPos = 64;
    for (int i = 0; i < 64; i++) {
        char piece = startingBoard[i];
        switch (piece) {
            case whiteKing:
                whiteKingPos = i;
                break;
            case blackKing:
                blackKingPos = i;
                break;
            case whiteQueen: // queens are represented by a rook and bishop on the same square
                whiteRookBB |= POS_TO_BITMASK(i);
                whiteBishopBB |= POS_TO_BITMASK(i);
                break;
            case blackQueen: // queens are represented by a rook and bishop on the same square
                blackRookBB |= POS_TO_BITMASK(i);
                blackBishopBB |= POS_TO_BITMASK(i);
                break;
            case whiteRook:
                whiteRookBB |= POS_TO_BITMASK(i);
                break;
            case blackRook:
                blackRookBB |= POS_TO_BITMASK(i);
                break;
            case whiteBishop:
                whiteBishopBB |= POS_TO_BITMASK(i);
                break;
            case blackBishop:
                blackBishopBB |= POS_TO_BITMASK(i);
                break;
            case whiteKnight:
                whiteKnightBB |= POS_TO_BITMASK(i);
                break;
            case blackKnight:
                blackKnightBB |= POS_TO_BITMASK(i);
                break;
            case whitePawn:
                whitePawnBB |= POS_TO_BITMASK(i);
                break;
            case blackPawn:
                blackPawnBB |= POS_TO_BITMASK(i);
                break;
        }
    }
    isWhitesTurn = true;
    lastMove = {};

    if (whiteKingPos == 64) {
        throw runtime_error("Invalid position: No white king found!");
    }
    if (blackKingPos == 64) {
        throw runtime_error("Invalid position: No black king found!");
    }

    if (whiteKingPos == 60) {
        whiteKingSideCastle = true;
        whiteQueenSideCastle = true;
    }
    if (blackKingPos == 4) {
        blackKingSideCastle = true;
        blackQueenSideCastle = true;
    }
}

void Board::whiteCapturePos(uint64_t mask) {
    blackPawnBB &= ~mask;
    blackKnightBB &= ~mask;
    blackBishopBB &= ~mask;
    blackRookBB &= ~mask;
}

void Board::blackCapturePos(uint64_t mask) {
    whitePawnBB &= ~mask;
    whiteKnightBB &= ~mask;
    whiteBishopBB &= ~mask;
    whiteRookBB &= ~mask;
}

int Board::doMove(Move m) {
    int pos = m.from;
    uint64_t mask = POS_TO_BITMASK(pos);

    if (isWhitesTurn) {
        if (pos == whiteKingPos) {
            if (whiteKingSideCastle && m.to == 62) // move rook when casting
                whiteRookBB ^= POS_TO_BITMASK(61) | POS_TO_BITMASK(63);
            else if (whiteQueenSideCastle && m.to == 58) // move rook when casting
                whiteRookBB ^= POS_TO_BITMASK(59) | POS_TO_BITMASK(56);
            whiteKingPos = m.to;
            whiteKingSideCastle = false;
            whiteQueenSideCastle = false;
            whiteCapturePos(POS_TO_BITMASK(m.to));
        }
        else if ((whitePawnBB & mask) != 0) {
            // promotion
            if (m.from <= 15) {
                switch (RANK(m.to)) {
                    case whiteQueen: whiteRookBB |= POS_TO_BITMASK(FILE(m.to)); whiteBishopBB |= POS_TO_BITMASK(FILE(m.to)); break;
                    case whiteRook: whiteRookBB |= POS_TO_BITMASK(FILE(m.to)); break;
                    case whiteBishop: whiteBishopBB |= POS_TO_BITMASK(FILE(m.to)); break;
                    case whiteKnight: whiteKnightBB |= POS_TO_BITMASK(FILE(m.to)); break;
                }
            }
            else {
                // en passant
                if (FILE(m.from) != FILE(m.to) && (POS_TO_BITMASK(m.to) & getAllPiecesBB()) == 0)
                    blackPawnBB &= ~POS_TO_BITMASK(24 + FILE(m.to));
                whitePawnBB |= POS_TO_BITMASK(m.to);
            }

            whitePawnBB &= ~POS_TO_BITMASK(m.from);
            whiteCapturePos(POS_TO_BITMASK(m.to));
        }
        else if ((whiteKnightBB & mask) != 0) {
            whiteKnightBB |= POS_TO_BITMASK(m.to);
            whiteKnightBB &= ~POS_TO_BITMASK(m.from);
            whiteCapturePos(POS_TO_BITMASK(m.to));
        }
        else if ((whiteBishopBB & mask) != 0) {
            if ((whiteRookBB & mask) != 0) {
                whiteRookBB |= POS_TO_BITMASK(m.to);
                whiteRookBB &= ~POS_TO_BITMASK(m.from);
            }
            whiteBishopBB |= POS_TO_BITMASK(m.to);
            whiteBishopBB &= ~POS_TO_BITMASK(m.from);
            whiteCapturePos(POS_TO_BITMASK(m.to));
        }
        else if ((whiteRookBB & mask) != 0) {
            if (m.from == 56)
                whiteQueenSideCastle = false;
            else if (m.from == 63)
                whiteKingSideCastle = false;
            whiteRookBB |= POS_TO_BITMASK(m.to);
            whiteRookBB &= ~POS_TO_BITMASK(m.from);
            whiteCapturePos(POS_TO_BITMASK(m.to));
        }
        if (isWhiteInCheck(getAllPiecesBB())) {
            return invalidMove;
        }
    }
    else {
        if (pos == blackKingPos) {
            if (blackKingSideCastle && m.to == 6) // move rook when casting
                blackRookBB ^= POS_TO_BITMASK(5) | POS_TO_BITMASK(7);
            else if (blackQueenSideCastle && m.to == 2) // move rook when casting
                blackRookBB ^= POS_TO_BITMASK(3) | POS_TO_BITMASK(0);
            blackKingPos = m.to;
            blackKingSideCastle = false;
            blackQueenSideCastle = false;
            blackCapturePos(POS_TO_BITMASK(m.to));
        }
        else if ((blackPawnBB & mask) != 0) {
            // promotion
            if (m.from >= 48) {
                switch (RANK(m.to)) {
                    case whiteQueen: blackRookBB |= POS_TO_BITMASK(FILE(m.to) | 56); blackBishopBB |= POS_TO_BITMASK(FILE(m.to) | 56); break;
                    case whiteRook: blackRookBB |= POS_TO_BITMASK(FILE(m.to) | 56); break;
                    case whiteBishop: blackBishopBB |= POS_TO_BITMASK(FILE(m.to) | 56); break;
                    case whiteKnight: blackKnightBB |= POS_TO_BITMASK(FILE(m.to) | 56); break;
                }
            }
            else {
                // en passant
                if (FILE(m.from) != FILE(m.to) && (POS_TO_BITMASK(m.to) & getAllPiecesBB()) == 0)
                    whitePawnBB &= ~POS_TO_BITMASK(32 + FILE(m.to));
                blackPawnBB |= POS_TO_BITMASK(m.to);
            }

            blackPawnBB &= ~POS_TO_BITMASK(m.from);
            blackCapturePos(POS_TO_BITMASK(m.to));
        }
        else if ((blackKnightBB & mask) != 0) {
            blackKnightBB |= POS_TO_BITMASK(m.to);
            blackKnightBB &= ~POS_TO_BITMASK(m.from);
            blackCapturePos(POS_TO_BITMASK(m.to));
        }
        else if ((blackBishopBB & mask) != 0) {
            if ((blackRookBB & mask) != 0) {
                blackRookBB |= POS_TO_BITMASK(m.to);
                blackRookBB &= ~POS_TO_BITMASK(m.from);
            }
            blackBishopBB |= POS_TO_BITMASK(m.to);
            blackBishopBB &= ~POS_TO_BITMASK(m.from);
            blackCapturePos(POS_TO_BITMASK(m.to));
        }
        else if ((blackRookBB & mask) != 0) {
            if (m.from == 0)
                blackQueenSideCastle = false;
            else if (m.from == 7)
                blackKingSideCastle = false;
            blackRookBB |= POS_TO_BITMASK(m.to);
            blackRookBB &= ~POS_TO_BITMASK(m.from);
            blackCapturePos(POS_TO_BITMASK(m.to));
        }
        if (isBlackInCheck(getAllPiecesBB())) {
            return invalidMove;
        }
    }

    isWhitesTurn = !isWhitesTurn;
    lastMove = m;
    
    return gameNotOver;
}

bool Board::isCheck() {
    if (isWhitesTurn) {
        return isWhiteInCheck(getAllPiecesBB());
    }
    else {
        return isBlackInCheck(getAllPiecesBB());
    }
}

bool Board::isWhiteInCheck(uint64_t allPiecesBB) {
    int kingPos = whiteKingPos;
    const uint64_t *moveArray = checkBitBoards[kingPos];
    int next;

    if ( (moveArray[0] & blackRookBB) != 0 ) {
        // maybe use a mask instead of pos
        for (next = kingPos + 1; FILE(next) != 0; next++) {
            if ((POS_TO_BITMASK(next) & allPiecesBB) != 0) {
                if ((POS_TO_BITMASK(next) & blackRookBB) != 0)
                    return true;
                break;
            }
        }
        for (next = kingPos - 1; FILE(next) != 7; next--) {
            if ((POS_TO_BITMASK(next) & allPiecesBB) != 0) {
                if ((POS_TO_BITMASK(next) & blackRookBB) != 0)
                    return true;
                break;
            }
        }
        for (next = kingPos + 8; next < 64; next += 8) {
            if ((POS_TO_BITMASK(next) & allPiecesBB) != 0) {
                if ((POS_TO_BITMASK(next) & blackRookBB) != 0)
                    return true;
                break;
            }
        }
        for (next = kingPos - 8; next >= 0; next -= 8) {
            if ((POS_TO_BITMASK(next) & allPiecesBB) != 0) {
                if ((POS_TO_BITMASK(next) & blackRookBB) != 0)
                    return true;
                break;
            }
        }
    }

    if ( (moveArray[1] & blackBishopBB) != 0 ) {
        for (next = kingPos + 9; FILE(next) != 0 && next < 64; next += 9) {
            if ((POS_TO_BITMASK(next) & allPiecesBB) != 0) {
                if ((POS_TO_BITMASK(next) & blackBishopBB) != 0)
                    return true;
                break;
            }
        }
        for (next = kingPos + 7; FILE(next) != 7 && next < 64; next += 7) {
            if ((POS_TO_BITMASK(next) & allPiecesBB) != 0) {
                if ((POS_TO_BITMASK(next) & blackBishopBB) != 0)
                    return true;
                break;
            }
        }
        for (next = kingPos - 9; FILE(next) != 7 && next >= 0; next -= 9) {
            if ((POS_TO_BITMASK(next) & allPiecesBB) != 0) {
                if ((POS_TO_BITMASK(next) & blackBishopBB) != 0)
                    return true;
                break;
            }
        }
        for (next = kingPos - 7; FILE(next) != 0 && next >= 0; next -= 7) {
            if ((POS_TO_BITMASK(next) & allPiecesBB) != 0) {
                if ((POS_TO_BITMASK(next) & blackBishopBB) != 0)
                    return true;
                break;
            }
        }
    }

    if ( (moveArray[2] & blackKnightBB) != 0 ) {return true;}
    if ( (moveArray[3] & ( POS_TO_BITMASK(blackKingPos))) != 0 ) {return true;}
    if ( (moveArray[4] & blackPawnBB) != 0 ) {return true;}

    return false;
}

bool Board::isBlackInCheck(uint64_t allPiecesBB) {
    int kingPos = blackKingPos;
    const uint64_t *moveArray = checkBitBoards[kingPos];
    int next;

    if ( (moveArray[0] & whiteRookBB) != 0 ) {
        for (next = kingPos + 1; FILE(next) != 0; next++) {
            if ((POS_TO_BITMASK(next) & allPiecesBB) != 0) {
                if ((POS_TO_BITMASK(next) & whiteRookBB) != 0)
                    return true;
                break;
            }
        }
        for (next = kingPos - 1; FILE(next) != 7; next--) {
            if ((POS_TO_BITMASK(next) & allPiecesBB) != 0) {
                if ((POS_TO_BITMASK(next) & whiteRookBB) != 0)
                    return true;
                break;
            }
        }
        for (next = kingPos + 8; next < 64; next += 8) {
            if ((POS_TO_BITMASK(next) & allPiecesBB) != 0) {
                if ((POS_TO_BITMASK(next) & whiteRookBB) != 0)
                    return true;
                break;
            }
        }
        for (next = kingPos - 8; next >= 0; next -= 8) {
            if ((POS_TO_BITMASK(next) & allPiecesBB) != 0) {
                if ((POS_TO_BITMASK(next) & whiteRookBB) != 0)
                    return true;
                break;
            }
        }
    }

    if ( (moveArray[1] & whiteBishopBB) != 0 ) {
        for (next = kingPos + 9; FILE(next) != 0 && next < 64; next += 9) {
            if ((POS_TO_BITMASK(next) & allPiecesBB) != 0) {
                if ((POS_TO_BITMASK(next) & whiteBishopBB) != 0)
                    return true;
                break;
            }
        }
        for (next = kingPos + 7; FILE(next) != 7 && next < 64; next += 7) {
            if ((POS_TO_BITMASK(next) & allPiecesBB) != 0) {
                if ((POS_TO_BITMASK(next) & whiteBishopBB) != 0)
                    return true;
                break;
            }
        }
        for (next = kingPos - 9; FILE(next) != 7 && next >= 0; next -= 9) {
            if ((POS_TO_BITMASK(next) & allPiecesBB) != 0) {
                if ((POS_TO_BITMASK(next) & whiteBishopBB) != 0)
                    return true;
                break;
            }
        }
        for (next = kingPos - 7; FILE(next) != 0 && next >= 0; next -= 7) {
            if ((POS_TO_BITMASK(next) & allPiecesBB) != 0) {
                if ((POS_TO_BITMASK(next) & whiteBishopBB) != 0)
                    return true;
                break;
            }
        }
    }

    if ( (moveArray[2] & whiteKnightBB) != 0 ) {return true;}
    if ( (moveArray[3] & ( POS_TO_BITMASK(whiteKingPos))) != 0 ) {return true;}
    if ( (moveArray[5] & whitePawnBB) != 0 ) {return true;}

    return false;
}

void Board::startMoveGenerator(std::function<void(Move)> func, int *stop) {
    uint64_t whitePiecesBB = whiteRookBB | whitePawnBB | whiteKnightBB | whiteBishopBB | POS_TO_BITMASK(whiteKingPos);
    uint64_t blackPiecesBB = blackRookBB | blackPawnBB | blackKnightBB | blackBishopBB | POS_TO_BITMASK(blackKingPos);
    uint64_t allPiecesBB = whitePiecesBB | blackPiecesBB;

    if (isWhitesTurn) {
        for (int pos = 0; pos < 64 && *stop == 0; pos++) {
            if ((POS_TO_BITMASK(pos) & whitePiecesBB) != 0)
                getMovesForWhitePiece(func, pos, whitePiecesBB, blackPiecesBB, allPiecesBB);
        }
        getWhiteKingMoves(func, whiteKingPos, allPiecesBB, whitePiecesBB);
    }
    else {
        for (int pos = 0; pos < 64 && *stop == 0; pos++) {
            if ((POS_TO_BITMASK(pos) & blackPiecesBB) != 0)
                getMovesForBlackPiece(func, pos, whitePiecesBB, blackPiecesBB, allPiecesBB);
        }
        getBlackKingMoves(func, blackKingPos, allPiecesBB, blackPiecesBB);
    }
}

void Board::getMovesForWhitePiece(function<void(Move)> func, int pos, uint64_t whiteBB, uint64_t blackBB, uint64_t allBB) {
    uint64_t mask = POS_TO_BITMASK(pos);
    if ((whitePawnBB & mask) != 0) {
        getWhitePawnMoves(func, pos, allBB, blackBB);
        return;
    }
    if ((whiteKnightBB & mask) != 0) {
        getKnightMoves(func, pos, whiteBB);
        return;
    }
    if ((whiteBishopBB & mask) != 0) {
        getBishopMoves(func, pos, allBB, whiteBB);
        // allow fallthrough here for queens
    }
    if ((whiteRookBB & mask) != 0) {
        getRookMoves(func, pos, allBB, whiteBB);
    }
}

void Board::getMovesForBlackPiece(function<void(Move)> func, int pos, uint64_t whiteBB, uint64_t blackBB, uint64_t allBB) {
    uint64_t mask = POS_TO_BITMASK(pos);
    if ((blackPawnBB & mask) != 0) {
        getBlackPawnMoves(func, pos, allBB, whiteBB);
        return;
    }
    if ((blackKnightBB & mask) != 0) {
        getKnightMoves(func, pos, blackBB);
        return;
    }
    if ((blackBishopBB & mask) != 0) {
        getBishopMoves(func, pos, allBB, blackBB);
        // allow falthrough here for queens
    }
    if ((blackRookBB & mask) != 0) {
        getRookMoves(func, pos, allBB, blackBB);
    }
}

// TODO: should not be able to castle through checks
void Board::getWhiteKingMoves(function<void(Move)> func, int pos, uint64_t allPiecesBB, uint64_t sameColorBB) {
    const int *moveArray = kingMoves[pos];
    int next;

    for (int i = 1; i < moveArray[0]; i++) {
        next = moveArray[i];
        if ( (POS_TO_BITMASK(next) & sameColorBB) == 0 )
            func((Move) {pos, next});
    }

    if (whiteKingSideCastle &&
        ((POS_TO_BITMASK(61) | POS_TO_BITMASK(62)) & allPiecesBB) == 0 &&
        (POS_TO_BITMASK(63) & whiteRookBB) != 0 )
    {
        if (!isWhiteInCheck(allPiecesBB))
            func((Move) {pos, 62});
    }
    if (whiteQueenSideCastle &&
        (POS_TO_BITMASK(56) & whiteRookBB) != 0 &&
        ((POS_TO_BITMASK(57) | POS_TO_BITMASK(58) | POS_TO_BITMASK(59)) & allPiecesBB) == 0)
    {
        if (!isWhiteInCheck(allPiecesBB))
            func((Move) {pos, 58});
    }
}

// TODO: should not be able to castle through checks
void Board::getBlackKingMoves(function<void(Move)> func, int pos, uint64_t allPiecesBB, uint64_t sameColorBB) {
    const int *moveArray = kingMoves[pos];
    int next;

    for (int i = 1; i < moveArray[0]; i++) {
        next = moveArray[i];
        if ( (POS_TO_BITMASK(next) & sameColorBB) == 0 )
            func((Move) {pos, next});
    }

    if (blackKingSideCastle &&
        ((POS_TO_BITMASK(5) | POS_TO_BITMASK(6)) & allPiecesBB) == 0 &&
        (POS_TO_BITMASK(7) & blackRookBB) != 0 )
    {
        if (!isBlackInCheck(allPiecesBB))
            func((Move) {pos, 6});
    }
    if (blackQueenSideCastle &&
        (POS_TO_BITMASK(0) & blackRookBB) != 0 &&
        ((POS_TO_BITMASK(1) | POS_TO_BITMASK(2) | POS_TO_BITMASK(3)) & allPiecesBB) == 0)
    {
        if (!isBlackInCheck(allPiecesBB))
           func((Move) {pos, 2});
    }
}

void Board::getRookMoves(function<void(Move)> func, int pos, uint64_t allPiecesBB, uint64_t sameColorBB) {
    int next;
    for (next = pos + 1; FILE(next) != 0; next++) {
        if ((POS_TO_BITMASK(next) & sameColorBB) == 0) {
            func((Move) {pos, next});
            if ((POS_TO_BITMASK(next) & allPiecesBB) != 0)
                break;
        } else {break;}
    }
    for (next = pos - 1; FILE(next) != 7; next--) {
        if ((POS_TO_BITMASK(next) & sameColorBB) == 0) {
            func((Move) {pos, next});
            if ((POS_TO_BITMASK(next) & allPiecesBB) != 0)
                break;
        } else {break;}
    }
    for (next = pos + 8; next < 64; next += 8) {
        if ((POS_TO_BITMASK(next) & sameColorBB) == 0) {
            func((Move) {pos, next});
            if ((POS_TO_BITMASK(next) & allPiecesBB) != 0)
                break;
        } else { break; }
    }
    for (next = pos - 8; next >= 0; next -= 8) {
        if ((POS_TO_BITMASK(next) & sameColorBB) == 0) {
            func((Move) {pos, next});
            if ((POS_TO_BITMASK(next) & allPiecesBB) != 0)
                break;
        } else {break;}
    }
}

void Board::getBishopMoves(function<void(Move)> func, int pos, uint64_t allPiecesBB, uint64_t sameColorBB) {
    int next;
    for (next = pos + 9; FILE(next) != 0 && next < 64; next += 9) {
        if ((POS_TO_BITMASK(next) & sameColorBB) == 0) {
            func((Move) {pos, next});
            if ((POS_TO_BITMASK(next) & allPiecesBB) != 0)
                break;
        } else {break;}
    }
    for (next = pos + 7; FILE(next) != 7 && next < 64; next += 7) {
        if ((POS_TO_BITMASK(next) & sameColorBB) == 0) {
            func((Move) {pos, next});
            if ((POS_TO_BITMASK(next) & allPiecesBB) != 0)
                break;
        } else {break;}
    }
    for (next = pos - 9; FILE(next) != 7 && next >= 0; next -= 9) {
        if ((POS_TO_BITMASK(next) & sameColorBB) == 0) {
            func((Move) {pos, next});
            if ((POS_TO_BITMASK(next) & allPiecesBB) != 0)
                break;
        } else {break;}
    }
    for (next = pos - 7; FILE(next) != 0 && next >= 0; next -= 7) {
        if ((POS_TO_BITMASK(next) & sameColorBB) == 0) {
            func((Move) {pos, next});
            if ((POS_TO_BITMASK(next) & allPiecesBB) != 0)
                break;
        } else {break;}
    }
}

void Board::getKnightMoves(function<void(Move)> func, int pos, uint64_t sameColorBB) {
    const int *moveArray = knightMoves[pos];
    int next;

    for (int i = 1; i < moveArray[0]; i++) {
        next = moveArray[i];
        if ( (POS_TO_BITMASK(next) & sameColorBB) == 0 )
            func((Move) {pos, next});
    }
}

void Board::getWhitePawnMoves(function<void(Move)> func, int pos, uint64_t allPiecesBB, uint64_t oppositeColorBB) {
    bool checkEnPassant = ((POS_TO_BITMASK(lastMove.to) & blackPawnBB) != 0 &&  // if a pawn just moved
                            RANK(lastMove.from) == 1 &&         // from the starting rank
                            RANK(lastMove.to) == 3);            // two ranks forward, then check en passant

    // move 1 forward
    int next = pos - 8;
    if ((POS_TO_BITMASK(next) & allPiecesBB) == 0) {
        Move m = {pos, next};
        if (next <= 7) { // encode promotion piece in destination rank
            m.to = FILE(next) | (whiteQueen << 3); func(m);
            m.to = FILE(next) | (whiteRook << 3); func(m);
            m.to = FILE(next) | (whiteBishop << 3); func(m);
            m.to = FILE(next) | (whiteKnight << 3); func(m);
        } else {
           func(m);

            // move 2 forward
            next -= 8;
            if (pos >= 48 && (POS_TO_BITMASK(next) & allPiecesBB) == 0)
                func((Move) {pos, next});
        }
    }

    // capture left and right (and en passant)
    if (FILE(pos) != 0) {
        next = pos - 9;
        if (((POS_TO_BITMASK(next) & oppositeColorBB) != 0) ||                 // if there is a black piece, allow capture
            (checkEnPassant && lastMove.to == pos - 1)) // if pawn just moved next to this pawn -> en passant
        {
            Move m = {pos, next};
            if (next <= 7) { // encode promotion piece in destination rank
                m.to = FILE(next) | (whiteQueen << 3); func(m);
                m.to = FILE(next) | (whiteRook << 3); func(m);
                m.to = FILE(next) | (whiteBishop << 3); func(m);
                m.to = FILE(next) | (whiteKnight << 3); func(m);
            } else
                func(m);
        }
    }
    if (FILE(pos) != 7) {
        next = pos - 7;
        if (((POS_TO_BITMASK(next) & oppositeColorBB) != 0) ||                 // if there is a black piece, allow capture
            (checkEnPassant && lastMove.to == pos + 1)) // if pawn just moved next to this pawn -> en passant
        {
            Move m = {pos, next};
            if (next <= 7) { // encode promotion piece in destination rank
                m.to = FILE(next) | (whiteQueen << 3); func(m);
                m.to = FILE(next) | (whiteRook << 3); func(m);
                m.to = FILE(next) | (whiteBishop << 3); func(m);
                m.to = FILE(next) | (whiteKnight << 3); func(m);
            } else
                func(m);
        }
    }
}

void Board::getBlackPawnMoves(function<void(Move)> func, int pos, uint64_t allPiecesBB, uint64_t oppositeColorBB) {
    bool checkEnPassant = ((POS_TO_BITMASK(lastMove.to) & whitePawnBB) != 0 &&  // if a pawn just moved
                            RANK(lastMove.from) == 6 &&         // from the starting rank
                            RANK(lastMove.to) == 4);            // two ranks forward, then check en passant

    // move 1 forward
    int next = pos + 8;
    if ((POS_TO_BITMASK(next) & allPiecesBB) == 0) {
        Move m = {pos, next};
        if (next >= 56) { // encode promotion piece in destination rank
            m.to = FILE(next) | (whiteQueen << 3); func(m);
            m.to = FILE(next) | (whiteRook << 3); func(m);
            m.to = FILE(next) | (whiteBishop << 3); func(m);
            m.to = FILE(next) | (whiteKnight << 3); func(m);
        } else {
            func(m);

            // move 2 forward
            next += 8;
            if (pos <= 15 && (POS_TO_BITMASK(next) & allPiecesBB) == 0)
                func((Move) {pos, next});
        }
    }

    // capture left and right (and en passant)
    if (FILE(pos) != 0) {
        next = pos + 7;
        if (((POS_TO_BITMASK(next) & oppositeColorBB) != 0) || // if there is a white piece, allow capture
            (checkEnPassant && lastMove.to == pos - 1))     // if pawn just moved next to this pawn -> en passant
        {
            Move m = {pos, next};
            if (next >= 56) { // encode promotion piece in destination rank
                m.to = FILE(next) | (whiteQueen << 3); func(m);
                m.to = FILE(next) | (whiteRook << 3); func(m);
                m.to = FILE(next) | (whiteBishop << 3); func(m);
                m.to = FILE(next) | (whiteKnight << 3); func(m);
            } else
                func(m);
        }
    }
    if (FILE(pos) != 7) {
        next = pos + 9;
        if (((POS_TO_BITMASK(next) & oppositeColorBB) != 0) || // if there is a white piece, allow capture
            (checkEnPassant && lastMove.to == pos + 1))     // if pawn just moved next to this pawn -> en passant
        {
            Move m = {pos, next};
            if (next >= 56) { // encode promotion piece in destination rank
                m.to = FILE(next) | (whiteQueen << 3); func(m);
                m.to = FILE(next) | (whiteRook << 3); func(m);
                m.to = FILE(next) | (whiteBishop << 3); func(m);
                m.to = FILE(next) | (whiteKnight << 3); func(m);
            } else
                func(m);
        }
    }
}

// popCount from https://www.chessprogramming.org/Population_Count
const uint64_t k1 = 0x5555555555555555; /*  -1/3   */
const uint64_t k2 = 0x3333333333333333; /*  -1/5   */
const uint64_t k4 = 0x0f0f0f0f0f0f0f0f; /*  -1/17  */
const uint64_t kf = 0x0101010101010101; /*  -1/255 */
int popCount(uint64_t x) {
    x =  x       - ((x >> 1)  & k1); /* put count of each 2 bits into those 2 bits */
    x = (x & k2) + ((x >> 2)  & k2); /* put count of each 4 bits into those 4 bits */
    x = (x       +  (x >> 4)) & k4 ; /* put count of each 8 bits into those 8 bits */
    x = (x * kf) >> 56; /* returns 8 most significant bits of x + (x<<8) + (x<<16) + (x<<24) + ...  */
    return (int) x;
}

int Board::getMaterialDiff() {
    int sum;
    sum =  popCount(whitePawnBB);
    sum += popCount(whiteKnightBB) * 3;
    sum += popCount(whiteBishopBB) * 3;
    sum += popCount(whiteRookBB) * 5;
    sum += popCount(whiteBishopBB & whiteRookBB); // bishop and rook already counted for 8pts

    sum -= popCount(blackPawnBB);
    sum -= popCount(blackKnightBB) * 3;
    sum -= popCount(blackBishopBB) * 3;
    sum -= popCount(blackRookBB) * 5;
    sum -= popCount(blackBishopBB & blackRookBB); // bishop and rook already counted for 8pts
    return sum;
}

bool Board::getIsWhitesTurn() {
    return isWhitesTurn;
}

uint64_t Board::getAllPiecesBB() {
    return whiteRookBB | whitePawnBB | whiteKnightBB | whiteBishopBB | blackRookBB | blackPawnBB | blackKnightBB | blackBishopBB | POS_TO_BITMASK(blackKingPos) | POS_TO_BITMASK(whiteKingPos);
}
