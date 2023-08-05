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
bool isComputerWhite = false;

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
        return {-1, -1};

    m = board->notationToMove(move);

    return m;
}

Move getPlayerMoveGui(Move *sharedMove) {
    while (sharedMove->from == -1 || sharedMove->to == -1) {
        this_thread::sleep_for(chrono::milliseconds(10));
        if (done)
            return {-1, -1};
    }

    Move m = *sharedMove;
    *sharedMove = {-1, -1};

    return m;
}

void playWithComputer(Move *unsused) {
    Move m;
    int status;
    if (isComputerWhite)
        goto computerStarts;

    while (true) {
        com->mainBoard.printBoard();

        m = getPlayerMove(&com->mainBoard);
        if (m.to == -1)
            break;

        status = com->submitPlayerMove(m);
        if (status != gameNotOver) {
            if (status == invalidMove) {
                cout << "\n";
                continue;
            }
            printStatus(status);
            break;
        }

computerStarts:
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
    if (isComputerWhite) {
        this_thread::sleep_for(chrono::milliseconds(1000)); // wait for gui
        goto computerStarts;
    }
    
    while (true) {
        m = getPlayerMoveGui(sharedMove);
        if (m.to == -1)
            break;

        status = com->submitPlayerMove(m);
        if (status != gameNotOver) {
            if (status == invalidMove) {
                cout << "Invalid Player Move\n";
                break;
            }
            printStatus(status);
            break;
        }

computerStarts:
        status = com->doComputerMove();
        if (status != gameNotOver) {
            if (status == invalidMove) {
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
    Move sharedMove = {-1, -1};
    Threads threadPool(func, &sharedMove);

    ComputerBoard c{&threadPool, depth, isComputerWhite};
    com = &c;

    done = true;

    threadPool.shutdown();
}

auto startTime = chrono::high_resolution_clock::now();
void profileStart() {
    startTime = chrono::high_resolution_clock::now();
}

void profileEnd() {
    auto stopTime = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(stopTime - startTime);
    cout << "profiler time: " << duration.count() << "ms" << endl;
}

void createThreadsGui(int depth) {        
    function<void(Move *)> func = playWithComputerGui;
    Move sharedMove = {-1, -1};
    Threads threadPool(func, &sharedMove);

    ComputerBoard c{&threadPool, depth, isComputerWhite};
    com = &c;

    EnhancedBoard *b = &com->mainBoard;

    profileStart();
    uint64_t allPiecesBB = b->getAllPiecesBB();
    for (int i = 0; i < 1000000; i++) {
        volatile bool x = b->isBlackInCheck(allPiecesBB);
    }
    profileEnd();

    Gui g = Gui(b, &sharedMove, !isComputerWhite);

    g.runGui();

    if (exportGame)
        b->exportToPGN("out.pgn");

    done = true;

    threadPool.shutdown();
}

int main(int argc, char *argv[]) {
    bool noGui = false;
    int depth = 6;

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
        else if (strcmp(argv[i], "-b") == 0) {
            isComputerWhite = true;
        }
        else if (strcmp(argv[i], "-h") == 0) {
            cout << "arguments:" << endl;
            cout << "   -d num      sets the computer depth (1-10)" << endl;
            cout << "   -e          export the game to a pgn file" << endl;
            cout << "               in the current directory as out.pgn" << endl;
            cout << "   -b          play as the black pieces" << endl;
            cout << "   -h          display this help message" << endl;
            cout << "   --nogui     opens the game in the command line" << endl;
            return 0;
        }
    }

    if (depth < 1 || depth > 10) {
        depth = 6;
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
