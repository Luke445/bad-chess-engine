#include <iostream>
#include "computer.h"

using namespace std;

void printStatus(int status) {
    if (status == whiteWins)
        cout << "White Wins!\n";
    else if (status == blackWins)
        cout << "Black Wins!\n";
    else
        cout << "The game is a draw\n";
}

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

Move getPlayerMoveGui() {
    string move;

    cout << "Enter Move (enter q to quit):\n";
    cin >> move;
    if (move == "q")
        return {{-1, -1}, {-1, -1}};

    Move m = (Move) {
        {stoi(move.substr(0, 1)), stoi(move.substr(1, 1))},
        {stoi(move.substr(2, 1)), stoi(move.substr(3, 1))},
        stoi(move.substr(4, 1))
    };

    return m;
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
        if (status != gameNotOver) {
            com.mainBoard.printBoardGui();
            if (status == -1) {
                cout << "\n";
                continue;
            }
            printStatus(status);
            break;
        }

        com.mainBoard.printBoard();

        status = com.doComputerMove();
        if (status != gameNotOver) {
            com.mainBoard.printBoardGui();
            if (status == -1) {
                cout << "Invalid Com Move\n";
                break;
            }
            printStatus(status);
            break;
        }

        cout << "\n";
    }
}

void playWithComputerGui() {
    Move m;
    int status;
    ComputerBoard com = ComputerBoard();
    while (true) {
        com.mainBoard.printBoardGui();

        m = getPlayerMoveGui();
        if (m.to.x == -1)
            break;

        status = com.submitPlayerMove(m);
        if (status != gameNotOver) {
            com.mainBoard.printBoardGui();
            if (status == -1) {
                cout << "Invalid Move\n";
                continue;
            }
            printStatus(status);
            break;
        }

        com.mainBoard.printBoardGui();

        status = com.doComputerMove();
        if (status != gameNotOver) {
            com.mainBoard.printBoardGui();
            if (status == -1) {
                cout << "Invalid Com Move\n";
                break;
            }
            printStatus(status);
            break;
        }
    }
}

int main(int argc, char *argv[]) {
    bool guiMode = false;
    if (argc >= 2 && strcmp(argv[1], "--gui") == 0) {
        guiMode = true;
    }

    if (guiMode)
        playWithComputerGui();
    else
        playWithComputer();

    /*
    cout << "save game (y/n): ";
    cin >> input;
    if (input == "y")
        board.exportToPGN("~/Documents/game.pgn");*/

    return 0;
}
