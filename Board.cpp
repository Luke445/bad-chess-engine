 #include <vector>
#include <iostream>
#include "Board.h"
#include "Computer.h"
#include "Data.h"

using namespace std;


void printBB(uint64_t BB) {
    cout << "---------------\n";
    int tmp;
    for (int i = 0; i < 8; i++) {
        tmp = BB & 0xFF;
        for (int j = 0; j < 8; j++) {
            if (tmp & 1)
                cout << "1 ";
            else
                cout << "0 ";
            tmp >>= 1;
        }
        cout << "\n";
        BB >>= 8;
    }
    cout << "---------------\n";
}


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
    enPassantTarget = 64;
    if (whiteKingPos == 64) {
        //throw runtime_error("Invalid position: No white king found!");
        cout << "Invalid position: No white king found!\n";
        whiteKingPos = 0;
    }
    if (blackKingPos == 64) {
        //throw runtime_error("Invalid position: No black king found!");
        cout << "Invalid position: No black king found!\n";
        blackKingPos = 0;
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
    enPassantTarget = 64;
}

void Board::blackCapturePos(uint64_t mask) {
    whitePawnBB &= ~mask;
    whiteKnightBB &= ~mask;
    whiteBishopBB &= ~mask;
    whiteRookBB &= ~mask;
    enPassantTarget = 64;
}

int Board::doMove(Move m) { // not very fast
    uint64_t mask = POS_TO_BITMASK(m.from);
    char piece;

    if (m.from == whiteKingPos) {piece = whiteKing;}
    else if (m.from == blackKingPos) {piece = whiteKing;}
    else if ((whitePawnBB & mask) != 0) {piece = whitePawn;}
    else if ((whiteKnightBB & mask) != 0) {piece = whiteKnight;}
    else if ((whiteBishopBB & mask) != 0) {piece = whiteBishop;}
    else if ((whiteRookBB & mask) != 0) {piece = whiteRook;}

    else if ((blackPawnBB & mask) != 0) {piece = whitePawn;}
    else if ((blackKnightBB & mask) != 0) {piece = whiteKnight;}
    else if ((blackBishopBB & mask) != 0) {piece = whiteBishop;}
    else if ((blackRookBB & mask) != 0) {piece = whiteRook;}

    if (isWhitesTurn)
        return doMoveWhite(m, piece);
    else
        return doMoveBlack(m, piece);
}

int Board::doMoveWhite(Move m, char piece) {
    uint64_t from_mask = POS_TO_BITMASK(m.from);
    uint64_t to_mask = POS_TO_BITMASK(m.to);
    switch (piece) {
        case whiteKing:
            if (whiteKingSideCastle && m.to == 62) { // move rook and prevent passing through/out of check
                if (isWhiteInCheck(getAllPiecesBB()))
                    return invalidMove;
                whiteKingPos = 61;
                if (isWhiteInCheck(getAllPiecesBB()))
                    return invalidMove;
                whiteRookBB ^= POS_TO_BITMASK(61) | POS_TO_BITMASK(63);
            }
            else if (whiteQueenSideCastle && m.to == 58) { // move rook and prevent passing through/out of check
                if (isWhiteInCheck(getAllPiecesBB()))
                    return invalidMove;
                whiteKingPos = 59;
                if (isWhiteInCheck(getAllPiecesBB()))
                    return invalidMove;
                whiteRookBB ^= POS_TO_BITMASK(59) | POS_TO_BITMASK(56);
            }
            whiteKingPos = m.to;
            whiteKingSideCastle = false;
            whiteQueenSideCastle = false;
            whiteCapturePos(to_mask);
            break;
        case whiteRook:
            if ((whiteBishopBB & from_mask) != 0) {
                whiteBishopBB ^= (to_mask | from_mask);
            }
            else if (m.from == 56)
                whiteQueenSideCastle = false;
            else if (m.from == 63)
                whiteKingSideCastle = false;
            whiteRookBB ^= (to_mask | from_mask);
            whiteCapturePos(to_mask);
            break;
        case whiteBishop:
            if ((whiteRookBB & from_mask) != 0) {
                whiteRookBB ^= (to_mask | from_mask);
            }
            whiteBishopBB ^= (to_mask | from_mask);
            whiteCapturePos(to_mask);
            break;
        case whiteKnight:
            whiteKnightBB ^= (to_mask | from_mask);
            whiteCapturePos(to_mask);
            break;
        case whitePawn:
            if (m.from <= 15) {
                to_mask = POS_TO_BITMASK(FILE(m.to));
                switch (RANK(m.to)) {
                    case whiteQueen: whiteRookBB |= to_mask; whiteBishopBB |= to_mask; break;
                    case whiteRook: whiteRookBB |= to_mask; break;
                    case whiteBishop: whiteBishopBB |= to_mask; break;
                    case whiteKnight: whiteKnightBB |= to_mask; break;
                }
                whitePawnBB &= ~from_mask;
                whiteCapturePos(to_mask);
            }
            else {
                if (m.to == enPassantTarget) {
                    blackPawnBB &= ~POS_TO_BITMASK(24 + FILE(m.to));
                    whiteCapturePos(to_mask);
                }
                else if (RANK(m.from) == 6 && RANK(m.to) == 4)
                //else if ((static_cast<uint16_t>(m) & 0x3838) == 0x3020)
                //else if ((m.from & 0b111000) == (6 << 3) && (m.to & 0b111000) == (4 << 3))
                    enPassantTarget = FILE(m.to) | 40;
                else
                    whiteCapturePos(to_mask);
                whitePawnBB ^= (to_mask | from_mask);
            }
            break;
    }
    if (isWhiteInCheck(getAllPiecesBB())) {
        return invalidMove;
    }

    isWhitesTurn = !isWhitesTurn;
    
    return gameNotOver;
}

// piece is always passed in as white, but this function assumes its actually black
int Board::doMoveBlack(Move m, char piece) {
    uint64_t from_mask = POS_TO_BITMASK(m.from);
    uint64_t to_mask = POS_TO_BITMASK(m.to);
    switch (piece) {
        case whiteKing:
            if (blackKingSideCastle && m.to == 6) { // move rook and prevent passing through/out of check
                if (isBlackInCheck(getAllPiecesBB()))
                    return invalidMove;
                blackKingPos = 5;
                if (isBlackInCheck(getAllPiecesBB()))
                    return invalidMove;
                blackRookBB ^= POS_TO_BITMASK(5) | POS_TO_BITMASK(7);
            }
            else if (blackQueenSideCastle && m.to == 2) { // move rook and prevent passing through/out of check
                if (isBlackInCheck(getAllPiecesBB()))
                    return invalidMove;
                blackKingPos = 3;
                if (isBlackInCheck(getAllPiecesBB()))
                    return invalidMove;
                blackRookBB ^= POS_TO_BITMASK(3) | POS_TO_BITMASK(0);
            }
            blackKingPos = m.to;
            blackKingSideCastle = false;
            blackQueenSideCastle = false;
            blackCapturePos(to_mask);
            break;
        case whiteRook:
            if ((blackBishopBB & from_mask) != 0) {
                blackBishopBB ^= (to_mask | from_mask);
            }
            else if (m.from == 0)
                blackQueenSideCastle = false;
            else if (m.from == 7)
                blackKingSideCastle = false;
            blackRookBB ^= (to_mask | from_mask);
            blackCapturePos(to_mask);
            break;
        case whiteBishop:
            if ((blackRookBB & from_mask) != 0) {
                blackRookBB ^= (to_mask | from_mask);
            }
            blackBishopBB ^= (to_mask | from_mask);
            blackCapturePos(to_mask);
            break;
        case whiteKnight:
            blackKnightBB ^= (to_mask | from_mask);
            blackCapturePos(to_mask);
            break;
        case whitePawn:
            if (m.from >= 48) {
                to_mask = POS_TO_BITMASK(m.to | 56); // (m.to | 56) sets the rank correctly (bc the rank stores the promotion piece)
                switch (RANK(m.to)) {
                    case whiteQueen: blackRookBB |= to_mask; blackBishopBB |= to_mask; break;
                    case whiteRook: blackRookBB |= to_mask; break;
                    case whiteBishop: blackBishopBB |= to_mask; break;
                    case whiteKnight: blackKnightBB |= to_mask; break;
                }
                blackPawnBB &= ~from_mask;
                blackCapturePos(to_mask);
            }
            else {
                // en passant
                if (m.to == enPassantTarget) {
                    whitePawnBB &= ~POS_TO_BITMASK(32 + FILE(m.to));
                    blackCapturePos(to_mask);
                }
                else if (RANK(m.from) == 1 && RANK(m.to) == 3)
                    enPassantTarget = FILE(m.to) | 16;
                else
                    blackCapturePos(to_mask);
                blackPawnBB ^= (to_mask | from_mask);
            }
            break;
    }

    if (isBlackInCheck(getAllPiecesBB())) {
        return invalidMove;
    }

    isWhitesTurn = !isWhitesTurn;
    
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
    uint64_t next;

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
        return false;
    }

    if ( (moveArray[2] & blackKnightBB) != 0 ) {return true;}
    //if ( (moveArray[3] & ( POS_TO_BITMASK(blackKingPos))) != 0 ) {return true;}
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
    //if ( (moveArray[3] & ( POS_TO_BITMASK(whiteKingPos))) != 0 ) {return true;}
    if ( (moveArray[5] & whitePawnBB) != 0 ) {return true;}

    return false;
}

uint64_t Board::getAllPiecesBB() {
    return whiteRookBB | whitePawnBB | whiteKnightBB | whiteBishopBB | blackRookBB | blackPawnBB | blackKnightBB | blackBishopBB | POS_TO_BITMASK(blackKingPos) | POS_TO_BITMASK(whiteKingPos);
}

void Board::getAllMoves(void (*func)(Eval*, Move, char), Eval *e) {
    if (isWhitesTurn)
        getAllMovesWhite(func, e);
    else
        getAllMovesBlack(func, e);
}

void Board::getAllMovesWhite(void (*func)(Eval*, Move, char), Eval *e) {
    uint64_t sameColorBB = whiteRookBB | whitePawnBB | whiteKnightBB | whiteBishopBB | POS_TO_BITMASK(whiteKingPos);
    uint64_t oppositeColorBB = blackRookBB | blackPawnBB | blackKnightBB | blackBishopBB | POS_TO_BITMASK(blackKingPos);
    uint64_t allPiecesBB = sameColorBB | oppositeColorBB;

    const int8_t *kingMoveArray = kingMoves[whiteKingPos];
    int8_t next;
    int nextSectionIndex;

    //const uint64_t *moveArray = checkBitBoards[kingPos];
    const uint64_t blackKingProtected = checkBitBoards[blackKingPos][3];
    //(moveArray[3] & ( POS_TO_BITMASK(blackKingPos))) != 0 ) {return true;}

    // --- King ---
    for (int i = 1; i < kingMoveArray[0]; i++) {
        next = kingMoveArray[i];
        if ( (POS_TO_BITMASK(next) & (sameColorBB | blackKingProtected)) == 0 )
            func(e, (Move) {static_cast<int8_t>(whiteKingPos), next}, whiteKing);
    }

    if (whiteKingSideCastle &&
        ((POS_TO_BITMASK(61) | POS_TO_BITMASK(62)) & allPiecesBB) == 0 &&
        (POS_TO_BITMASK(63) & whiteRookBB) != 0 )
    {
        func(e, (Move) {60, 62}, whiteKing);
    }
    if (whiteQueenSideCastle &&
        (POS_TO_BITMASK(56) & whiteRookBB) != 0 &&
        ((POS_TO_BITMASK(57) | POS_TO_BITMASK(58) | POS_TO_BITMASK(59)) & allPiecesBB) == 0)
    {
        func(e, (Move) {60, 58}, whiteKing);
    }

    uint64_t mask;
    for (int8_t pos = 0; pos < 64 && e->stop == 0; pos++) {
        mask = POS_TO_BITMASK(pos);
        if ((mask & sameColorBB) != 0) {
            if ((whitePawnBB & mask) != 0) { // --- Pawn ---
                // move 1 forward
                next = pos - 8;
                if ((POS_TO_BITMASK(next) & allPiecesBB) == 0) {
                    Move m = {pos, next};
                    if (next <= 7) { // encode promotion piece in destination rank
                        m.to = FILE(next) | (whiteQueen << 3); func(e, m, whitePawn);
                        m.to = FILE(next) | (whiteRook << 3); func(e, m, whitePawn);
                        m.to = FILE(next) | (whiteBishop << 3); func(e, m, whitePawn);
                        m.to = FILE(next) | (whiteKnight << 3); func(e, m, whitePawn);
                    } else {
                       func(e, m, whitePawn);

                        // move 2 forward
                        next -= 8;
                        if (pos >= 48 && (POS_TO_BITMASK(next) & allPiecesBB) == 0)
                            func(e, (Move) {pos, next}, whitePawn);
                    }
                }

                // capture left and right (and en passant)
                if (FILE(pos) != 0) {
                    next = pos - 9;
                    if ( (POS_TO_BITMASK(next) & oppositeColorBB) != 0 || (next == enPassantTarget) ) {
                        Move m = {pos, next};
                        if (next <= 7) { // encode promotion piece in destination rank
                            m.to = FILE(next) | (whiteQueen << 3); func(e, m, whitePawn);
                            m.to = FILE(next) | (whiteRook << 3); func(e, m, whitePawn);
                            m.to = FILE(next) | (whiteBishop << 3); func(e, m, whitePawn);
                            m.to = FILE(next) | (whiteKnight << 3); func(e, m, whitePawn);
                        } else
                            func(e, m, whitePawn);
                    }
                }
                if (FILE(pos) != 7) {
                    next = pos - 7;
                    if ( (POS_TO_BITMASK(next) & oppositeColorBB) != 0 || (next == enPassantTarget) ) {
                        Move m = {pos, next};
                        if (next <= 7) { // encode promotion piece in destination rank
                            m.to = FILE(next) | (whiteQueen << 3); func(e, m, whitePawn);
                            m.to = FILE(next) | (whiteRook << 3); func(e, m, whitePawn);
                            m.to = FILE(next) | (whiteBishop << 3); func(e, m, whitePawn);
                            m.to = FILE(next) | (whiteKnight << 3); func(e, m, whitePawn);
                        } else
                            func(e, m, whitePawn);
                    }
                }
                continue;
            }
            if ((whiteKnightBB & mask) != 0) { // --- Knight ---
                const int8_t *knightMoveArray = knightMoves[pos];

                for (int i = 1; i < knightMoveArray[0]; i++) {
                    next = knightMoveArray[i];
                    if ( (POS_TO_BITMASK(next) & sameColorBB) == 0 )
                        func(e, (Move) {pos, next}, whiteKnight);
                }
                continue;
            }
            if ((whiteBishopBB & mask) != 0) { // --- Bishop ---
                for (next = pos + 9; FILE(next) != 0 && next < 64; next += 9) {
                    if ((POS_TO_BITMASK(next) & allPiecesBB) == 0) {
                        func(e, (Move) {pos, next}, whiteBishop);
                    } else {
                        if ((POS_TO_BITMASK(next) & oppositeColorBB) != 0)
                            func(e, (Move) {pos, next}, whiteBishop);
                        break;
                    }
                }
                for (next = pos + 7; FILE(next) != 7 && next < 64; next += 7) {
                    if ((POS_TO_BITMASK(next) & allPiecesBB) == 0) {
                        func(e, (Move) {pos, next}, whiteBishop);
                    } else {
                        if ((POS_TO_BITMASK(next) & oppositeColorBB) != 0)
                            func(e, (Move) {pos, next}, whiteBishop);
                        break;
                    }
                }
                for (next = pos - 9; FILE(next) != 7 && next >= 0; next -= 9) {
                    if ((POS_TO_BITMASK(next) & allPiecesBB) == 0) {
                        func(e, (Move) {pos, next}, whiteBishop);
                    } else {
                        if ((POS_TO_BITMASK(next) & oppositeColorBB) != 0)
                            func(e, (Move) {pos, next}, whiteBishop);
                        break;
                    }
                }
                for (next = pos - 7; FILE(next) != 0 && next >= 0; next -= 7) {
                    if ((POS_TO_BITMASK(next) & allPiecesBB) == 0) {
                        func(e, (Move) {pos, next}, whiteBishop);
                    } else {
                        if ((POS_TO_BITMASK(next) & oppositeColorBB) != 0)
                            func(e, (Move) {pos, next}, whiteBishop);
                        break;
                    }
                }
                // allow fallthrough here for queens
            }
            if ((whiteRookBB & mask) != 0) { // --- Rook ---
                for (next = pos + 1; FILE(next) != 0; next++) {
                    if ((POS_TO_BITMASK(next) & allPiecesBB) == 0) {
                        func(e, (Move) {pos, next}, whiteRook);
                    } else {
                        if ((POS_TO_BITMASK(next) & oppositeColorBB) != 0)
                            func(e, (Move) {pos, next}, whiteRook);
                        break;
                    }
                }
                for (next = pos - 1; FILE(next) != 7; next--) {
                    if ((POS_TO_BITMASK(next) & allPiecesBB) == 0) {
                        func(e, (Move) {pos, next}, whiteRook);
                    } else {
                        if ((POS_TO_BITMASK(next) & oppositeColorBB) != 0)
                            func(e, (Move) {pos, next}, whiteRook);
                        break;
                    }
                }
                for (next = pos + 8; next < 64; next += 8) {
                    if ((POS_TO_BITMASK(next) & allPiecesBB) == 0) {
                        func(e, (Move) {pos, next}, whiteRook);
                    } else {
                        if ((POS_TO_BITMASK(next) & oppositeColorBB) != 0)
                            func(e, (Move) {pos, next}, whiteRook);
                        break;
                    }
                }
                for (next = pos - 8; next >= 0; next -= 8) {
                    if ((POS_TO_BITMASK(next) & allPiecesBB) == 0) {
                        func(e, (Move) {pos, next}, whiteRook);
                    } else {
                        if ((POS_TO_BITMASK(next) & oppositeColorBB) != 0)
                            func(e, (Move) {pos, next}, whiteRook);
                        break;
                    }
                }
            }
        }
    }
}

void Board::getAllMovesBlack(void (*func)(Eval*, Move, char), Eval *e) {
    uint64_t sameColorBB = blackRookBB | blackPawnBB | blackKnightBB | blackBishopBB | POS_TO_BITMASK(blackKingPos);
    uint64_t oppositeColorBB = whiteRookBB | whitePawnBB | whiteKnightBB | whiteBishopBB | POS_TO_BITMASK(whiteKingPos);
    uint64_t allPiecesBB = sameColorBB | oppositeColorBB;

    const int8_t *kingMoveArray = kingMoves[blackKingPos];
    int8_t next;
    int nextSectionIndex;

    const uint64_t whiteKingProtected = checkBitBoards[whiteKingPos][3];

    // --- King ---
    for (int i = 1; i < kingMoveArray[0]; i++) {
        next = kingMoveArray[i];
        if ( (POS_TO_BITMASK(next) & (sameColorBB | whiteKingProtected)) == 0 )
            func(e, (Move) {static_cast<int8_t>(blackKingPos), next}, whiteKing);
    }

    if (blackKingSideCastle &&
        ((POS_TO_BITMASK(5) | POS_TO_BITMASK(6)) & allPiecesBB) == 0 &&
        (POS_TO_BITMASK(7) & blackRookBB) != 0 )
    {
        func(e, (Move) {4, 6}, whiteKing);
    }
    if (blackQueenSideCastle &&
        (POS_TO_BITMASK(0) & blackRookBB) != 0 &&
        ((POS_TO_BITMASK(1) | POS_TO_BITMASK(2) | POS_TO_BITMASK(3)) & allPiecesBB) == 0)
    {
        func(e, (Move) {4, 2}, whiteKing);
    }

    uint64_t mask;
    for (int8_t pos = 0; pos < 64 && e->stop == 0; pos++) {
        mask = POS_TO_BITMASK(pos);
        if ((mask & sameColorBB) != 0) {
            if ((blackPawnBB & mask) != 0) { // --- Pawn ---
                // move 1 forward
                next = pos + 8;
                if ((POS_TO_BITMASK(next) & allPiecesBB) == 0) {
                    Move m = {pos, next};
                    if (next >= 56) { // encode promotion piece in destination rank
                        m.to = FILE(next) | (whiteQueen << 3); func(e, m, whitePawn);
                        m.to = FILE(next) | (whiteRook << 3); func(e, m, whitePawn);
                        m.to = FILE(next) | (whiteBishop << 3); func(e, m, whitePawn);
                        m.to = FILE(next) | (whiteKnight << 3); func(e, m, whitePawn);
                    } else {
                        func(e, m, whitePawn);

                        // move 2 forward
                        next += 8;
                        if (pos <= 15 && (POS_TO_BITMASK(next) & allPiecesBB) == 0)
                            func(e, (Move) {pos, next}, whitePawn);
                    }
                }

                // capture left and right (and en passant)
                if (FILE(pos) != 0) {
                    next = pos + 7;
                    if ( (POS_TO_BITMASK(next) & oppositeColorBB) != 0 || (next == enPassantTarget) ) {
                        Move m = {pos, next};
                        if (next >= 56) { // encode promotion piece in destination rank
                            m.to = FILE(next) | (whiteQueen << 3); func(e, m, whitePawn);
                            m.to = FILE(next) | (whiteRook << 3); func(e, m, whitePawn);
                            m.to = FILE(next) | (whiteBishop << 3); func(e, m, whitePawn);
                            m.to = FILE(next) | (whiteKnight << 3); func(e, m, whitePawn);
                        } else
                            func(e, m, whitePawn);
                    }
                }
                if (FILE(pos) != 7) {
                    next = pos + 9;
                    if ( (POS_TO_BITMASK(next) & oppositeColorBB) != 0 || (next == enPassantTarget) ) {
                        Move m = {pos, next};
                        if (next >= 56) { // encode promotion piece in destination rank
                            m.to = FILE(next) | (whiteQueen << 3); func(e, m, whitePawn);
                            m.to = FILE(next) | (whiteRook << 3); func(e, m, whitePawn);
                            m.to = FILE(next) | (whiteBishop << 3); func(e, m, whitePawn);
                            m.to = FILE(next) | (whiteKnight << 3); func(e, m, whitePawn);
                        } else
                            func(e, m, whitePawn);
                    }
                }
                continue;
            }
            if ((blackKnightBB & mask) != 0) { // --- Knight ---
                const int8_t *knightMoveArray = knightMoves[pos];

                for (int i = 1; i < knightMoveArray[0]; i++) {
                    next = knightMoveArray[i];
                    if ( (POS_TO_BITMASK(next) & sameColorBB) == 0 )
                        func(e, (Move) {pos, next}, whiteKnight);
                }
                continue;
            }
            if ((blackBishopBB & mask) != 0) { // --- Bishop ---
                for (next = pos + 9; FILE(next) != 0 && next < 64; next += 9) {
                    if ((POS_TO_BITMASK(next) & allPiecesBB) == 0) {
                        func(e, (Move) {pos, next}, whiteBishop);
                    } else {
                        if ((POS_TO_BITMASK(next) & oppositeColorBB) != 0)
                            func(e, (Move) {pos, next}, whiteBishop);
                        break;
                    }
                }
                for (next = pos + 7; FILE(next) != 7 && next < 64; next += 7) {
                    if ((POS_TO_BITMASK(next) & allPiecesBB) == 0) {
                        func(e, (Move) {pos, next}, whiteBishop);
                    } else {
                        if ((POS_TO_BITMASK(next) & oppositeColorBB) != 0)
                            func(e, (Move) {pos, next}, whiteBishop);
                        break;
                    }
                }
                for (next = pos - 9; FILE(next) != 7 && next >= 0; next -= 9) {
                    if ((POS_TO_BITMASK(next) & allPiecesBB) == 0) {
                        func(e, (Move) {pos, next}, whiteBishop);
                    } else {
                        if ((POS_TO_BITMASK(next) & oppositeColorBB) != 0)
                            func(e, (Move) {pos, next}, whiteBishop);
                        break;
                    }
                }
                for (next = pos - 7; FILE(next) != 0 && next >= 0; next -= 7) {
                    if ((POS_TO_BITMASK(next) & allPiecesBB) == 0) {
                        func(e, (Move) {pos, next}, whiteBishop);
                    } else {
                        if ((POS_TO_BITMASK(next) & oppositeColorBB) != 0)
                            func(e, (Move) {pos, next}, whiteBishop);
                        break;
                    }
                }
                // allow fallthrough here for queens
            }
            if ((blackRookBB & mask) != 0) { // --- Rook ---
                for (next = pos + 1; FILE(next) != 0; next++) {
                    if ((POS_TO_BITMASK(next) & allPiecesBB) == 0) {
                        func(e, (Move) {pos, next}, whiteRook);
                    } else {
                        if ((POS_TO_BITMASK(next) & oppositeColorBB) != 0)
                            func(e, (Move) {pos, next}, whiteRook);
                        break;
                    }
                }
                for (next = pos - 1; FILE(next) != 7; next--) {
                    if ((POS_TO_BITMASK(next) & allPiecesBB) == 0) {
                        func(e, (Move) {pos, next}, whiteRook);
                    } else {
                        if ((POS_TO_BITMASK(next) & oppositeColorBB) != 0)
                            func(e, (Move) {pos, next}, whiteRook);
                        break;
                    }
                }
                for (next = pos + 8; next < 64; next += 8) {
                    if ((POS_TO_BITMASK(next) & allPiecesBB) == 0) {
                        func(e, (Move) {pos, next}, whiteRook);
                    } else {
                        if ((POS_TO_BITMASK(next) & oppositeColorBB) != 0)
                            func(e, (Move) {pos, next}, whiteRook);
                        break;
                    }
                }
                for (next = pos - 8; next >= 0; next -= 8) {
                    if ((POS_TO_BITMASK(next) & allPiecesBB) == 0) {
                        func(e, (Move) {pos, next}, whiteRook);
                    } else {
                        if ((POS_TO_BITMASK(next) & oppositeColorBB) != 0)
                            func(e, (Move) {pos, next}, whiteRook);
                        break;
                    }
                }
            }
        }
    }
}

