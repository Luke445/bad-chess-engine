#include <iostream>
#include <random>
#include "Board.h"
#include "Threads.h"
#include "Computer.h"

using namespace std;
using namespace std::chrono;

ComputerBoard::ComputerBoard(Threads *t, int d, bool isWhite) {
    mainBoard.resetBoard();
    threadPool = t;
    startDepth = d;

    isWhitePieces = isWhite;
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
    return mainBoard.doMove(m);
}

Move ComputerBoard::selectRandomMove(vector<Move> *moves) {
    int index = rand() % moves->size();
    return moves->at(index);
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

    /*sortEvals();
    for (int i = 0; i < moveEvals.size(); i++) { // debug - display move evals
        cout << mainBoard.moveToNotation(moveEvals[i].first) << " [" << moveEvals[i].second << "]" << endl;
    }
    cout << endl;*/

    // after primary search is finished, do a secondary search for moves with the same evaluation
    if (isWhitePieces) {
        for (int i = 0; i < moveEvals.size(); i++) {
            moveEvals[i].second = moveEvals[i].second*8 + secondaryEvalWhite(moveEvals[i].first);
        }
    }
    else {
        for (int i = 0; i < moveEvals.size(); i++) {
            moveEvals[i].second = moveEvals[i].second*8 - secondaryEvalBlack(moveEvals[i].first);
        }
    }

    // select best move
    sortEvals();

    /*for (int i = 0; i < moveEvals.size(); i++) { // debug - display move evals
        cout << mainBoard.moveToNotation(moveEvals[i].first) << " (" << moveEvals[i].second << ")" << endl;
    }*/

    return moveEvals[0].first;
}

int ComputerBoard::secondaryEvalWhite(Move m) {
    // scores a move based on how well it develops/takes space/protects king
    char piece = mainBoard.getPos(m.from);

    bool isCastle = false;
    bool movesKing = false;
    int centerDist = 0;
    bool developsPiece = false;
    bool queenOut = false;
    int pawnPush = 0;
    int totalMaterial = 0;

    // get information positional information about the move
    if (piece == whiteKing) {
        if (m.from == 60 && (m.to == 62 || m.to == 58))
            isCastle = true;
        else
            movesKing = true;
    }
    else if (piece == whitePawn) {
        if (FILE(m.to) <= 3) // pawn center distance
            centerDist = 3 - FILE(m.to);
        else // rank >= 4
            centerDist = FILE(m.to) - 4;

        if (RANK(m.from) == 6 && RANK(m.to) == 4) // pawn forward moving
            pawnPush = 2;
        else
            pawnPush = 1;
    }

    if (RANK(m.from) == 7) {
        if (piece == whiteKnight || piece == whiteBishop)
            developsPiece = true;
    }

    for (int i = 0; i < 64; i++) {
        totalMaterial += abs( getPieceValue(mainBoard.getPos(i)) );
    }

    if (piece == whiteQueen && RANK(m.to) < 6)
        queenOut = true;


    // weight the given information and calculate score
    int score;
    if (totalMaterial >= 68)  { // early game
        score = (
            isCastle * 7 + 
            movesKing * -7 +
            centerDist * -2 +
            developsPiece * 3 +
            pawnPush * 2 +
            queenOut * -2
        );
    }
    else if (totalMaterial >= 35) { // middle game
        score = (
            isCastle * 7 + 
            movesKing * -7 +
            centerDist * -1 +
            developsPiece * 4 +
            pawnPush * 2
        );
    }
    else { // endgame
        score = (
            pawnPush * 1
        );
    }

    return score;
}

int ComputerBoard::secondaryEvalBlack(Move m) {
    // scores a move based on how well it develops/takes space/protects king
    char piece = mainBoard.getPos(m.from);

    bool isCastle = false;
    bool movesKing = false;
    int centerDist = 0;
    bool developsPiece = false;
    bool queenOut = false;
    int pawnPush = 0;
    int totalMaterial = 0;

    // get information positional information about the move
    if (piece == blackKing) {
        if (m.from == 4 && (m.to == 6 || m.to == 2))
           isCastle = true;
        else
            movesKing = true;
    }
    else if (piece == blackPawn) {
        if (FILE(m.to) <= 3) // pawn center distance
            centerDist = 3 - FILE(m.to);
        else // rank >= 4
            centerDist = FILE(m.to) - 4;

        if (RANK(m.from) == 1 && RANK(m.to) == 3) // pawn forward moving
            pawnPush = 2;
        else
            pawnPush = 1;
    }

    if (RANK(m.from) == 0) {
        if (piece == blackKnight || piece == blackBishop)
            developsPiece = true;
    }

    for (int i = 0; i < 64; i++) {
        totalMaterial += abs( getPieceValue(mainBoard.getPos(i)) );
    }

    if (piece == blackQueen && RANK(m.to) > 0) {
        queenOut = true;
    }

    // weight the given information and calculate score
    int score;
    if (totalMaterial >= 68)  { // early game
        score = (
            isCastle * 7 + 
            movesKing * -7 +
            centerDist * -2 +
            developsPiece * 3 +
            pawnPush * 2 +
            queenOut * -2
        );
    }
    else if (totalMaterial >= 35) { // middle game
        score = (
            isCastle * 7 + 
            movesKing * -7 +
            centerDist * -1 +
            developsPiece * 4 +
            pawnPush * 2
        );
    }
    else { // endgame
        score = (
            pawnPush * 1
        );
    }

    return score;
}

int ComputerBoard::scoreBoard(Board *b) {
    return b->getMaterialDiff();
}

int ComputerBoard::getPieceValue(char piece) {
    switch (piece) {
        case whiteKing:
        case blackKing:
        case noPiece:
            return 0;

        case whiteQueen:
            return 9;
        case whiteRook:
            return 5;
        case whiteBishop:
            return 3;
        case whiteKnight:
            return 3;
        case whitePawn:
            return 1;

        case blackQueen:
            return -9;
        case blackRook:
            return -5;
        case blackBishop:
            return -3;
        case blackKnight:
            return -3;
        case blackPawn:
            return -1;
    }

    return 0;
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
    b.doMove(m);

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

    Board b;
    int value, stop;
    std::function<void(Move)> callback;

    if (startingBoard->getIsWhitesTurn()) {
        value = -10000;
        callback = [&] (Move m) {
            b = *startingBoard;
            if (b.doMove(m) != invalidMove) {
                value = max(value, evalPos(&b, depth - 1, alpha, beta));

                alpha = max(alpha, value);
                if (value >= beta)
                   stop = 1;
            }
        };
    }
    else {
        value = 10000;
        callback = [&] (Move m) {
            b = *startingBoard;
            if (b.doMove(m) != invalidMove) {
                value = min(value, evalPos(&b, depth - 1, alpha, beta));

                beta = min(beta, value);
                if (value <= alpha)
                    stop = 1;
            }
        };
    }

    stop = 0;
    startingBoard->startMoveGenerator(callback, &stop);
    
    if (abs(value) == 10000) { // no moves possible, game is over
        if (startingBoard->isCheck()) {
            if (startingBoard->getIsWhitesTurn())
                return -1000 * depth; // black wins
            else
                return 1000 * depth; // white wins
        }
        return 0; // stalemate
    }

    return value;
}

