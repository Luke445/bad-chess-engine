#include "EnhancedBoard.h"
#include <iostream>
#include <fstream>

using namespace std;

void EnhancedBoard::resetBoard() {
    moveList.clear();
    validMoves.clear();
    movesPlayed = 0;
    gameStatus = gameNotOver;

    Board::resetBoard();

    getAllValidMoves();
}

int EnhancedBoard::doMove(Move m) {
    if (isValidMove(m)) {
        Board::doMove(m);
        moveList.push_back(m);
        movesPlayed++;
        validMoves.clear();

        getAllValidMoves();

        if (validMoves.size() == 0) {
            if (isCheck()) {
                if (isWhitesTurn)
                    gameStatus = blackWins; // black wins
                else
                    gameStatus = whiteWins; // white wins
            }
            else
                gameStatus = draw; // stalemate
        }
        else
            gameStatus = gameNotOver;
    }
    else 
        gameStatus = invalidMove;
    
    return gameStatus;
}

bool EnhancedBoard::isValidMove(Move m) {
    for (Move x : validMoves) {
        if (x.to == m.to && x.from == m.from)
            return true;
    }

    return false;
}

vector<Move> * EnhancedBoard::getAllValidMoves() {
    if (validMoves.empty()) {
        Board newBoard;

        Eval e;
        e.startingBoard = this;
        
        auto callback = [] (Eval *e, Move m, char piece) {
            e->b = *e->startingBoard;
            int status = e->b.doMove(m);
            EnhancedBoard *this_ = (EnhancedBoard *) e->startingBoard; // bit of a hack
            if (status != invalidMove) {
                this_->validMoves.push_back(m);
            }
        };

        e.stop = 0;
        getAllMoves(callback, &e);
    }

    return &validMoves;
}

string EnhancedBoard::getPieceStr(char piece) {
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

Move EnhancedBoard::notationToMove(string m) {
    return {0, 0};
    /*try {
        // remove trailing + or #
        if (m.compare(m.size() - 1, 1, "+") == 0 || m.compare(m.size() - 1, 1, "#") == 0)
            m = m.substr(0, m.size() - 1);

        // castling
        if (m == "O-O") {
            if (isWhitesTurn)
                return {39, 55};
            else
                return {32, 48};
        } else if (m == "O-O-O") {
            if (isWhitesTurn)
                return {39, 23};
            else
                return {32, 16};
        }

        // first check for promotion
        if (m.compare(m.size() - 2, 1, "=") == 0) // just remove for now TODO
            m = m.substr(0, m.size() - 2);

        int to_rank = 8 - stoi(m.substr(m.size() - 1, m.size()));
        if (to_rank < 0 || to_rank > 7)
            return {0, 0};
        int to_file = ((char) m[m.size() - 2]) - 97;
        if (to_file < 0 || to_file > 7)
            return {0, 0};

        cout << to_rank << " " << to_file << endl;

        m = m.substr(0, m.size() - 2);
        cout << "1. " << m << endl;
        int from_rank, from_file;
        switch (m.size()) {
            case 3: // non-pawn move with both rank and file
                from_rank = 8 - stoi(m.substr(1, 2));
                if (from_rank < 0 || from_rank > 7)
                    return {-1, -1};
                from_file = ((char) m[1]) - 97;
                if (from_file < 0 || from_file > 7)
                    return {0, 0};
                return {from_rank*8 + from_file, to_rank*8 + to_file};
            case 2: // pawn capture or piece move with rank/file
                if (m[1] == 'x') { // pawn capture
                    from_file = ((char) m[0]) - 97;
                    if (from_file < 0 || from_file > 7)
                        return {0, 0};
                    if (isWhitesTurn)
                        return {(to_rank + 1)*8 + from_file, to_rank*8 + to_file};
                    else
                        return {(to_rank - 1)*8 + from_file, to_rank*8 + to_file};
                }
                try {
                    from_rank = 8 - stoi(m.substr(1, 2));
                    if (from_rank < 0 || from_rank > 7)
                        return {0, 0};
                    // non-pawn move with rank data TODO
                    return {0, 0};
                }
                catch (...) {
                    from_file = ((char) m[1]) - 97;
                    if (from_file < 0 || from_file > 7)
                        return {0, 0};
                    // non-pawn move with file data TODO
                    return {0, 0};
                }
            case 1: // non-pawn move TODO
                return {0, 0};
            case 0: // (nothing) - pawn push 
                // TODO: promotion
                if (isWhitesTurn) {
                    if (getPos((to_rank + 1)*8 + to_file) == whitePawn)
                        return {(to_rank + 1)*8 + to_file, to_rank*8 + to_file};
                    else
                        return {(to_rank + 2)*8 + to_file, to_rank*8 + to_file};
                }
                else {
                    if (getPos((to_rank - 1)*8 + to_file) == blackPawn)
                        return {(to_rank - 1)*8 + to_file, to_rank*8 + to_file};
                    else
                        return {(to_rank - 2)*8 + to_file, to_rank*8 + to_file};
                }
            default:
                return {0, 0};
        }
    }
    catch (...) {
        return {0, 0};
    }*/
}

string EnhancedBoard::moveToNotation(Move m) {
    EnhancedBoard b = *this;
    return b.moveToNotationInternal(m);
}

string EnhancedBoard::moveToNotationInternal(Move m) {
    // returns a string containing the current move in PGN format
    // the passed in move will be played on the current board
    string from, to, out;
    // change number to letter by using ascii table offset (+97) 0 -> a, 1 -> b, etc.
    from = ((char) FILE(m.from) + 97);
    from += to_string(8 - RANK(m.from));
    to = ((char) FILE(m.to) + 97);
    to += to_string(8 - RANK(m.to));
    char piece = getPos(m.from);
    bool multiplePieces = false;
    bool sameFile = false;
    bool sameRank = false;
    Move move;    

    // castling
    if (piece == whiteKing) {
        if (m.from == 60 && m.to == 62) {
            out = "O-O";
            goto end;
        }
        else if (m.from == 60 && m.to == 58) {
            out = "O-O-O";
            goto end;
        }
    }
    else if (piece == blackKing) {
        if (m.from == 4 && m.to == 6) {
            out = "O-O";
            goto end;
        }
        else if (m.from == 4 && m.to == 2) {
            out = "O-O-O";
            goto end;
        }
    }

    if (abs(piece) == whitePawn) {
        if (FILE(m.from) != FILE(m.to)) // pawn capturing
            out = from.substr(0, 1) + "x";
        else // no capture - push
            out = "";

        if ((RANK(m.from) == 1 && piece == whitePawn) || 
            (RANK(m.from) == 6 && piece == blackPawn)
        ) { // promoting
            if (piece == whitePawn) // rank holds promotion piece, get rank manually
                out += to.substr(0, 1) + "8";
            else
                out += to.substr(0, 1) + "1";
            if (RANK(m.to) == whiteBishop || RANK(m.to) == whiteKnight || RANK(m.to) == whiteRook)
                out += "=" + getPieceStr(RANK(m.to));
            else
                out += "=Q";
        }
        else // pawn push
            out += to;
    }
    else {
        out = getPieceStr(piece);

        for (int i = 0; i < validMoves.size(); i++) {
            move = validMoves.at(i);
            if (getPos(move.from) == piece && m.to == move.to && m.from != move.from) {
                multiplePieces = true;
                if (RANK(m.from) == RANK(move.from))
                    sameRank = true;
                else if (FILE(m.from) == FILE(move.from))
                    sameFile = true;
            }
        }

        if (sameRank && sameFile) {
            out += from;
        }
        else if (sameFile) {
            out += from.substr(1, 1);
        }
        else if (multiplePieces) {
            out += from.substr(0, 1);
        }
        
        if (getPos(m.to) != noPiece) // capturing
            out += "x";
        out += to;
    }

end:
    Board::doMove(m);
    validMoves.clear();
    getAllValidMoves();

    if (isCheck()) {
        if (validMoves.size() == 0)
            out += "#";
        else
            out += "+";
    }

    return out;
}

void EnhancedBoard::exportToPGN(string filepath) {
    ofstream f;
    f.open(filepath);
    if (!f.is_open()) {
        cout << "failed to open output file\n";
        return;
    }

    // header information
    f << "[Event \"?\"]\n";
    f << "[Site \"?\"]\n";
    f << "[Date \"????.??.??\"]\n";
    f << "[Round \"?\"]\n";
    f << "[White \"?\"]\n";
    f << "[Black \"?\"]\n";
    f << "[Result \"*\"]\n\n";

    // moves
    bool isWhiteMove = true;
    int curMove = 1;

    // reset the board data and then play through all the moves again
    // to restore the original board and create the pgn notation
    Board::resetBoard();
    for (int i = 0; i < moveList.size(); i++) {
        if (isWhiteMove) {
            f << to_string(curMove++) << ". " << moveToNotationInternal(moveList.at(i)) << " ";
        }
        else { // black's move
            f << moveToNotationInternal(moveList.at(i)) << " ";
        }
        isWhiteMove = !isWhiteMove;
    }

    f.close();
}

void EnhancedBoard::printBoard() {
    string pieceStr;
    cout << "    a b c d e f g h\n";
    cout << "  *----------------";
    for (int pos = 0; pos < 64; pos += 8) {
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

char EnhancedBoard::getPos(int pos) {
    uint64_t mask;

    if (pos == whiteKingPos) {return whiteKing;}
    if (pos == blackKingPos) {return blackKing;}

    mask = POS_TO_BITMASK(pos);
    if ((whitePawnBB & mask) != 0) {return whitePawn;}
    if ((whiteKnightBB & mask) != 0) {return whiteKnight;}
    if ((whiteBishopBB & mask) != 0) {
        if ((whiteRookBB & mask) != 0)
            return whiteQueen;
        return whiteBishop;
    }
    if ((whiteRookBB & mask) != 0) {return whiteRook;}

    if ((blackPawnBB & mask) != 0) {return blackPawn;}
    if ((blackKnightBB & mask) != 0) {return blackKnight;}
    if ((blackBishopBB & mask) != 0) {
        if ((blackRookBB & mask) != 0)
            return blackQueen;
        return blackBishop;
    }
    if ((blackRookBB & mask) != 0) {return blackRook;}


    return noPiece;
}

bool EnhancedBoard::isPosWhite(int pos) {
    return getPos(pos) > 0;
}

Move EnhancedBoard::getLastMove() {
    if (moveList.size() == 0)
        return {-1, -1};
    else
        return moveList.at(moveList.size() - 1);
}

int EnhancedBoard::getStatus() {
    return gameStatus;
}
