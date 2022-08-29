#include <iostream>
#include <stdlib.h>
#include <thread>
#include "Computer.h"
#include "EnhancedBoard.h"
#include "Threads.h"
#include "Gui.h"

using namespace std;

bool done = false;
bool exportGame = false;

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

void createThreads(int depth) {        
    function<void(Move *)> func = playWithComputer;
    Move sharedMove = {-1, -1, -1};
    Threads threadPool(func, &sharedMove);

    ComputerBoard c{&threadPool, depth};
    com = &c;

    done = true;

    threadPool.shutdown();
}

void createThreadsGui(int depth) {        
    function<void(Move *)> func = playWithComputerGui;
    Move sharedMove = {-1, -1, -1};
    Threads threadPool(func, &sharedMove);

    ComputerBoard c{&threadPool, depth};
    com = &c;

    EnhancedBoard *b = &com->mainBoard;

    /*auto start = chrono::high_resolution_clock::now();
    for (int i = 0; i < 100000000; i++) {
        volatile bool x = b->isBlackInCheck();
    }
    auto stop = chrono::high_resolution_clock::now();

    auto duration = chrono::duration_cast<chrono::milliseconds>(stop - start);

    cout << "isBlackInCheck time: " << duration.count() << "ms" << endl;*/

    Gui g = Gui(b, &sharedMove);

    g.runGui();

    if (exportGame)
        b->exportToPGN("out.pgn");

    done = true;

    threadPool.shutdown();
}

int main(int argc, char *argv[]) {
    bool noGui = false;
    int depth = 4;
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--nogui") == 0) {
            noGui = true;
        }
        else if (strcmp(argv[i], "-d") == 0) {
            i++;
            if (i < argc) {
                depth = atoi(argv[i]);
            }
        }
        else if (strcmp(argv[i], "-e") == 0) {
            exportGame = true;
        }
    }

    if (depth < 1 || depth > 10) {
        depth = 4;
        cout << "invalid depth" << endl;
    }

    if (noGui) {
        createThreads(depth);
    }
    else {
        createThreadsGui(depth);
    }

    return 0;
}
