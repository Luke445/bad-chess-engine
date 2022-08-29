#include <iostream>
#include <random>
#include "Board.h"
#include "Threads.h"
#include "Computer.h"

using namespace std;
using namespace std::chrono;

ComputerBoard::ComputerBoard(Threads *t, int d) {
    mainBoard.resetBoard();
    threadPool = t;
    startDepth = d;

    // TODO allow the computer to play as white
    isWhitePieces = false;
}

int ComputerBoard::doComputerMove() {
    auto start = high_resolution_clock::now();
    Move m = bruteForce();
    auto stop = high_resolution_clock::now();

    auto duration = duration_cast<milliseconds>(stop - start);

    cout << "doComputerMove time: " << duration.count() << "ms" << endl;

    return mainBoard.doMove(m);
}

int ComputerBoard::submitPlayerMove(Move m) {
    int status;

    if (mainBoard.isValidMove(m)) {
        status = mainBoard.doMove(m);
        return status;
    }

    return -1;
}

Move ComputerBoard::selectRandomMove(vector<Move> *moves) {
    int index = rand() % moves->size();
    return moves->at(index);
}

int ComputerBoard::setValueFromStatus(int status) {
    if (status == whiteWins)
        return 1000;
    else if (status == blackWins)
        return -1000;
    else
        return 0;
}

void ComputerBoard::sortEvals() {
    sort(moveEvals.begin(), moveEvals.end(),
        [](pair<Move, int> a, pair<Move, int> b) {
            return a.second > b.second;
        }
    );

    if (!isWhitePieces)
        reverse(moveEvals.begin(), moveEvals.end());
}

Move ComputerBoard::bruteForce() {
    vector<Move> *moves = mainBoard.getAllValidMoves();

    //return selectRandomMove(moves);

    /*
    first does a primary brute force search looking for things like 
    free material or tactics
    then does a secondary search through all of potential "good" moves
    to finally select the best
    */

    // if there is only one move available dont do any calculations
    if (moves->size() == 1)
        return moves->at(0);

    moveEvals.clear();

    // randomize moves so the engine is not deterministic
    shuffle(moves->begin(), moves->end(), random_device());

    // prepare jobs for threads
    for (int i = 0; i < moves->size(); i++)
        evalQueue.push(moves->at(i));
    function<void()> func = bind(&ComputerBoard::evalMove, this);
    for (int i = 0; i < moves->size(); i++)
        threadPool->addJob(func);

    // wait for brute forcing to finish
    threadPool->waitUntilDone();

    // order moves based on score
    sortEvals();

    /*for (int i = 0; i < moveEvals.size(); i++) { // debug - display move evals
        cout << mainBoard.moveToNotationNoUpdate(moveEvals[i].first) << " [" << moveEvals[i].second << "]" << endl;
    }*/

    // remove all moves that are not tied for the best
    int numMoves = 1;
    for (; numMoves < moveEvals.size(); numMoves++) {
        if (moveEvals[numMoves].second != moveEvals[0].second)
            break;
    }
    moveEvals.resize(numMoves);

    if (numMoves == 1)
        return moveEvals[0].first;

    // after primary search is finished, do a secondary search for moves with the same evaluation
    for (int i = 0; i < moveEvals.size(); i++) {
        moveEvals[i].second = secondaryEval(moveEvals[i].first);
    }

    // select best move
    sortEvals();
    return moveEvals[0].first;
}

int ComputerBoard::secondaryEval(Move m) {
    // scores a move based on how well it develops/takes space/protects king
    char piece = mainBoard.getPos(m.from);

    bool isCastle = false;
    bool movesKing = false;
    int centerDist = 0;
    bool developsPiece = false;
    bool pawnPush = false;
    int totalMaterial = 0;

    // get information positional information about the move
    if (piece == whiteKing) {
        if (m.from == 60 && (m.to == 62 || m.to == 58))
            isCastle = true;
        else
            movesKing = true;
    }
    else if (piece == blackKing) {
        if (m.from == 4 && (m.to == 6 || m.to == 2))
           isCastle = true;
        else
            movesKing = true;
    }

    if (abs(piece) == whitePawn) { // only care about staying in the center for pawns
        if ((m.to & 7) <= 3)
            centerDist = 3 - (m.to & 7);
        else // rank >= 4
            centerDist = (m.to & 7) - 4;
    }

    if ((m.from >> 3) == 7 && isWhitePieces) {
        if (piece == whiteKnight || piece == whiteBishop)
            developsPiece = true;
    }
    else if ((m.from >> 3) == 0 && !isWhitePieces) {
        if (piece == blackKnight || piece == blackBishop)
            developsPiece = true;
    }

    if (abs(piece) == whitePawn)
        pawnPush = true;

    for (int i = 0; i < 64; i++) {
        totalMaterial += abs( mainBoard.getPieceValue(mainBoard.getPos(i)) );
    }


    // weight the given information and calculate score
    int score;
    if (totalMaterial >= 68)  { // early game
        score = (
            isCastle * 10 + 
            movesKing * -10 +
            centerDist * -2 +
            developsPiece * 3 +
            pawnPush * 4 +
            (abs(piece) == whiteQueen) * -2
        );
    }
    else if (totalMaterial >= 35) { // middle game
        score = (
            isCastle * 10 + 
            movesKing * -10 +
            centerDist * -1 +
            developsPiece * 4 +
            pawnPush * 3
        );
    }
    else { // endgame
        score = (
            pawnPush * 1
        );
    }

    if (!isWhitePieces)
        score = -score;

    //cout << mainBoard.moveToNotationNoUpdate(m) << " (" << score << ")" << endl;
    return score;
}

int ComputerBoard::scoreBoard(Board *b) {
    return b->getMaterialDiff();
}

// shorthand callback for threads
void ComputerBoard::evalMove() {
    // get move SAFELY
    queueMutex.lock();

    if (evalQueue.empty())
        return;
    Move m = evalQueue.front();
    evalQueue.pop();

    queueMutex.unlock();

    // eval move
    int value, status;
    Board b(mainBoard);
    status = b.doMove(m);
    if (status != gameNotOver)
        value = setValueFromStatus(status) * startDepth;
    else
        value = evalPos(&b, startDepth - 1, -10000, 10000);

    // submit value SAFELY
    evalMutex.lock();

    moveEvals.push_back(pair<Move, int> {m, value});

    evalMutex.unlock();
}

/*
 * implements an alpha-beta algorithm that scores the
 * given position based on material difference
 * depth - moves to search in plies
 */
int ComputerBoard::evalPos(Board *startingBoard, int depth, int alpha, int beta) {
    if (depth == 0)
        return scoreBoard(startingBoard);

    vector<Move> *moves = startingBoard->getAllValidMoves();
    if (moves->size() == 0)
        return 0;

    Board b;
    int status, value;
    if (startingBoard->getIsWhitesTurn()) {
        value = -10000;
        for (Move m : *moves) {
            b = *startingBoard;
            status = b.doMove(m);
            if (status != gameNotOver)
                value = setValueFromStatus(status) * depth;  // depth will be higher the closer the checkmate is
            else
                value = max(value, evalPos(&b, depth - 1, alpha, beta));

            alpha = max(alpha, value);
            if (value >= beta)
                break;
        }
        return value;
    }
    else {
        value = 10000;
        for (Move m : *moves) {
            b = *startingBoard;
            status = b.doMove(m);
            if (status != gameNotOver)
                value = setValueFromStatus(status) * depth;  // depth will be higher the closer the checkmate is
            else
                value = min(value, evalPos(&b, depth - 1, alpha, beta));

            beta = min(beta, value);
            if (value <= alpha)
                break;
        }
        return value;
    }
}
