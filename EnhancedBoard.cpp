#include "EnhancedBoard.h"
#include <iostream>

using namespace std;

string EnhancedBoard::getPieceStr(int piece) {
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
    return {};
}

string EnhancedBoard::moveToNotation(Move m) {
    return "";
}

void EnhancedBoard::exportToPGN(string filepath) {
    return;
}

/*
Move EnhancedBoard::notationToMove(string m) {
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
string EnhancedBoard::moveToNotation(Move m) {
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
void EnhancedBoard::exportToPGN(string filepath) {
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

void EnhancedBoard::printBoard() {
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
