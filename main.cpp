#include <iostream>
#include "computer.h"

using namespace std;

Move getPlayerMove(Board *board) {
    string move;
    Move m;

    cout << "Enter Move (enter q to quit):\n";
    cin >> move;
    if (move == "q")
        return {{-1, -1}, {-1, -1}};

    m = board->notationToMove(move);

    return m;
}

void playWithHuman() {
    Board board;
    board.resetBoard();

    Move m;
    int status;
    while (true) {
        board.printBoard();

        m = getPlayerMove(&board);
        if (m.to.x == -1)
            break;

        if (board.isValidMove(m)) {
            status = board.doMove(m);
            if (status != gameNotOver) {
                board.printBoard();
                if (status == whiteWins)
                    cout << "White Wins!\n";
                else if (status == blackWins)
                    cout << "Black Wins!\n";
                else
                    cout << "the game is a draw\n";
                break;
            }
        }
        else
            cout << "Invalid Move\n";

        cout << "\n";
    }
}

void playWithComputer() {
    Move m;
    int status;
    ComputerBoard com = ComputerBoard();
    while (true) {
        com.mainBoard.printBoard();

        m = getPlayerMove(&com.mainBoard);
        if (m.to.x == -1)
            break;

        cout << "{" << m.from.x << ", " << m.from.y << "} {" << m.to.x << ", " << m.to.y << "}\n";

        status = com.submitPlayerMove(m);
        if (status == -1) {
            cout << "\n";
            continue;
        }
        else if (status != gameNotOver)
            break;

        com.mainBoard.printBoard();

        status = com.doComputerMove();
        if (status != gameNotOver)
            break;

        cout << "\n";
    }
}

int main() {
    string input;
    cout << "play against computer (y/n): ";
    cin >> input;
    if (input == "y")
        playWithComputer();
    else
        playWithHuman();

    /*
    cout << "save game (y/n): ";
    cin >> input;
    if (input == "y")
        board.exportToPGN("~/Documents/game.pgn");*/

    return 0;
}
