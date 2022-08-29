#include "EnhancedBoard.h"
#include <iostream>
#include <fstream>

using namespace std;

int EnhancedBoard::doMove(Move m) {
    if (isValidMove(m)) {
        gameStatus = Board::doMove(m);
        moveList.push_back(m);
        movesPlayed++;
        return gameStatus;
    }

    return -1;
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
    /*try {
        // remove trailing + or #
        if (!(m.compare(m.size() - 1, 1, "+") && m.compare(m.size() - 1, 1, "#")))
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
    catch (...) {}*/
    return {};
}

string EnhancedBoard::moveToNotationNoUpdate(Move m) {
    // gives the move notation without updating the board data
    EnhancedBoard b = *this;
    return b.moveToNotation(m);
}

string EnhancedBoard::moveToNotation(Move m) {
    // returns a string containing the current move in PGN format
    // the passed in move will be played
    string from, to, out;
    // change number to letter by using ascii table offset (+97) 0 -> a, 1 -> b, etc.
    from = ((char) (m.from & 7) + 97);
    from += to_string(8 - (m.from >> 3));
    to = ((char) (m.to & 7) + 97);
    to += to_string(8 - (m.to >> 3));
    bool isCapture = getPos(m.to) != noPiece;

    char piece = getPos(m.from);

    int moveStatus = Board::doMove(m);

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
        // check for en passant  
        if ((m.from & 0b111) != (m.to & 0b111) && !isCapture)
            isCapture = true;

        if (isCapture)
            out = from.substr(0, 1) + "x" + to;
        else
            out = to;
        if ((m.to >> 3) == 0 || (m.to >> 3) == 7) { // promoting
            if (m.flags == whiteBishop || m.flags == whiteKnight || m.flags == whiteRook)
                out += "=" + getPieceStr(m.flags);
            else
                out += "=Q";
        }
    }
    else {
        bool multiplePieces = false;
        bool sameFile = false;
        bool sameRank = false;
        vector<Move> moves;
        int pos;

        b[m.from] = whiteKing;
        b[m.to] = -b[m.to];
        switch (abs(piece)) {
            case whiteKnight:
                getKnightMoves(&moves, m.to);
                break;
            case whiteQueen:
                getQueenMoves(&moves, m.to);
                break;
            case whiteRook:
                getRookMoves(&moves, m.to);
                break;
            case whiteBishop:
                getBishopMoves(&moves, m.to);
                break;
        }
        b[m.to] = -b[m.to];
        b[m.from] = noPiece;

        // & 7 == file          >> 3 == rank
        for (int i = 0; i < moves.size(); i++) {
            pos = moves.at(i).to;
            if (getPos(pos) == piece) {
                multiplePieces = true;
                if ((m.from >> 3) == (pos >> 3))
                    sameRank = true;
                else if ((m.from & 7) == (pos & 7))
                    sameFile = true;
            }
        }

        out = getPieceStr(piece);

        if (sameRank && sameFile) {
            out += from;
        }
        else if (sameFile) {
            out += from.substr(1, 1);
        }
        else if (multiplePieces) {
            out += from.substr(0, 1);
        }
        
        if (isCapture)
            out += "x";
        out += to;
    }

end:
    if (isCheck()) {
        if (moveStatus == whiteWins || moveStatus == blackWins) // checkmate
            out += "#";
        else // check
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
    resetBoard();
    for (int i = 0; i < moveList.size(); i++) {
        if (isWhiteMove) {
            f << to_string(curMove++) << ". " << moveToNotation(moveList.at(i)) << " ";
        }
        else { // black's move
            f << moveToNotation(moveList.at(i)) << " ";
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

Move EnhancedBoard::getLastMove() {
    if (movesPlayed == 0)
        return {-1, -1, -1};
    else
        return lastMove;
}

int EnhancedBoard::getStatus() {
    return gameStatus;
}
