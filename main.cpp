#include <iostream>
#include <thread>
#include "Computer.h"
#include "EnhancedBoard.h"
#include "Threads.h"
#include "Gui.h"

using namespace std;

bool done = false;

ComputerBoard *com;

void printStatus(int status) {
    if (status == whiteWins)
        cout << "White Wins!\n";
    else if (status == blackWins)
        cout << "Black Wins!\n";
    else
        cout << "The game is a draw\n";
}

Move getPlayerMove(EnhancedBoard *board) {
    string move;
    Move m;

    cout << "Enter Move (enter q to quit):\n";
    cin >> move;
    if (move == "q")
        return {-1, -1, -1};

    m = board->notationToMove(move);

    return m;
}

Move getPlayerMoveGui(Move *sharedMove) {
    while (sharedMove->from == -1 || sharedMove->to == -1 || sharedMove->flags == -1) {
        this_thread::sleep_for(chrono::milliseconds(10));
        if (done)
            return {-1, -1, -1};
    }

    Move m = *sharedMove;
    *sharedMove = {-1, -1, -1};

    return m;
}

void playWithComputer(Move *unsused) {
    Move m;
    int status;
    while (true) {
        com->mainBoard.printBoard();

        m = getPlayerMove(&com->mainBoard);
        if (m.to == -1)
            break;

        cout << (int)m.from << ", " << (int)m.to << "\n";

        status = com->submitPlayerMove(m);
        if (status != gameNotOver) {
            if (status == -1) {
                cout << "\n";
                continue;
            }
            printStatus(status);
            break;
        }

        com->mainBoard.printBoard();

        status = com->doComputerMove();
        if (status != gameNotOver) {
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

void playWithComputerGui(Move *sharedMove) {
    Move m;
    int status;
    while (true) {
        m = getPlayerMoveGui(sharedMove);
        if (m.to == -1)
            break;

        status = com->submitPlayerMove(m);
        if (status != gameNotOver) {
            if (status == -1) {
                continue;
            }
            printStatus(status);
            break;
        }

        status = com->doComputerMove();
        if (status != gameNotOver) {
            if (status == -1) {
                cout << "Invalid Com Move\n";
                break;
            }
            printStatus(status);
            break;
        }
    }
}

void createThreads() {        
    function<void(Move *)> func = playWithComputer;
    Move sharedMove = {-1, -1, -1};
    Threads threadPool(func, &sharedMove);

    ComputerBoard c{&threadPool};
    com = &c;

    done = true;

    threadPool.shutdown();
}

void createThreadsGui() {        
    function<void(Move *)> func = playWithComputerGui;
    Move sharedMove = {-1, -1, -1};
    Threads threadPool(func, &sharedMove);

    ComputerBoard c{&threadPool};
    com = &c;

    EnhancedBoard *b = &com->mainBoard;
    Gui g = Gui(b, &sharedMove);

    g.runGui();

    done = true;

    threadPool.shutdown();
}

int main(int argc, char *argv[]) {
    if (argc >= 2 && strcmp(argv[1], "--nogui") == 0)
        createThreads();
    else
        createThreadsGui();

    return 0;
}
