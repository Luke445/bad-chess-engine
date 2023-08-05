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
    //cout << "white doubled pawns: " << doubledPawns(mainBoard.whitePawnBB) << endl;
    //cout << "black doubled pawns: " << doubledPawns(mainBoard.blackPawnBB) << endl;

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
    /*if (b.isWhitesTurn)
        value = b.evalPosWhite(startDepth - 1, -10000, 10000);
    else
        value = b.evalPosBlack(startDepth - 1, -10000, 10000);*/

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
int evalPos(Board *startingBoard, int depth, int alpha, int beta) {
    if (depth == 0)
        return scoreBoard(startingBoard);

    //Board b;
    //int value, stop;
    Eval test;
    test.startingBoard = startingBoard;
    test.depth = depth;
    test.alpha = alpha;
    test.beta = beta;
    test.stop = 0;

    if (startingBoard->isWhitesTurn) {
        test.value = -10000;
        startingBoard->getAllMovesWhite(evalWhite, &test);
    }
    else {
        test.value = 10000;
        startingBoard->getAllMovesBlack(evalBlack, &test);
    }

    if (abs(test.value) == 10000) { // no moves possible, game is over
        if (test.startingBoard->isCheck()) {
            if (test.startingBoard->isWhitesTurn)
                return -1000 * test.depth; // black wins
            else
                return 1000 * test.depth; // white wins
        }
        return 0; // stalemate
    }

    return test.value;
}
/*
struct Eval {
    Board b;
    int value;
    int stop;
    Board *startingBoard;
    int depth;
    int alpha;
    int beta;
};
*/

void evalWhite(Eval *e, Move m, char piece) {
    e->b = *e->startingBoard;
    if (e->b.doMoveWhite(m, piece) != invalidMove) {
        e->value = max(e->value, evalPos(&e->b, e->depth - 1, e->alpha, e->beta));

        e->alpha = max(e->alpha, e->value);
        if (e->value >= e->beta)
           e->stop = 1;
    }
}

void evalBlack(Eval *e, Move m, char piece) {
    e->b = *e->startingBoard;
    if (e->b.doMoveBlack(m, piece) != invalidMove) {
        e->value = min(e->value, evalPos(&e->b, e->depth - 1, e->alpha, e->beta));

        e->beta = min(e->beta, e->value);
        if (e->value <= e->alpha)
            e->stop = 1;
    }
}

// popCount from https://www.chessprogramming.org/Population_Count
const uint64_t k1 = 0x5555555555555555; /*  -1/3   */
const uint64_t k2 = 0x3333333333333333; /*  -1/5   */
const uint64_t k4 = 0x0f0f0f0f0f0f0f0f; /*  -1/17  */
const uint64_t kf = 0x0101010101010101; /*  -1/255 */
int popCount(uint64_t x) {
    x =  x       - ((x >> 1)  & k1); /* put count of each 2 bits into those 2 bits */
    x = (x & k2) + ((x >> 2)  & k2); /* put count of each 4 bits into those 4 bits */
    x = (x       +  (x >> 4)) & k4 ; /* put count of each 8 bits into those 8 bits */
    x = (x * kf) >> 56; /* returns 8 most significant bits of x + (x<<8) + (x<<16) + (x<<24) + ...  */
    return (int) x;
}

int doubledPawns(uint64_t x) {
    int count = (x & 0x0101010101010101) ? 1 : 0;
    count +=    (x & 0x0202020202020202) ? 1 : 0;
    count +=    (x & 0x0404040404040404) ? 1 : 0;
    count +=    (x & 0x0808080808080808) ? 1 : 0;
    count +=    (x & 0x1010101010101010) ? 1 : 0;
    count +=    (x & 0x2020202020202020) ? 1 : 0;
    count +=    (x & 0x4040404040404040) ? 1 : 0;
    count +=    (x & 0x8080808080808080) ? 1 : 0;
    return popCount(x) - count; // #total number of pawns - files with pawns
}

int getMaterialDiff(Board *b) {
    int sum;
    sum =  popCount(b->whitePawnBB);
    sum += popCount(b->whiteKnightBB) * 3;
    sum += popCount(b->whiteBishopBB) * 3;
    sum += popCount(b->whiteRookBB) * 5;
    sum += popCount(b->whiteBishopBB & b->whiteRookBB); // bishop and rook already counted for 8pts

    sum -= popCount(b->blackPawnBB);
    sum -= popCount(b->blackKnightBB) * 3;
    sum -= popCount(b->blackBishopBB) * 3;
    sum -= popCount(b->blackRookBB) * 5;
    sum -= popCount(b->blackBishopBB & b->blackRookBB); // bishop and rook already counted for 8pts
    return sum;
}

int getTotalMaterial(Board *b) {
    int sum;
    sum =  popCount(b->whitePawnBB);
    sum += popCount(b->whiteKnightBB) * 3;
    sum += popCount(b->whiteBishopBB) * 3;
    sum += popCount(b->whiteRookBB) * 5;
    sum += popCount(b->whiteBishopBB & b->whiteRookBB); // bishop and rook already counted for 8pts

    sum += popCount(b->blackPawnBB);
    sum += popCount(b->blackKnightBB) * 3;
    sum += popCount(b->blackBishopBB) * 3;
    sum += popCount(b->blackRookBB) * 5;
    sum += popCount(b->blackBishopBB & b->blackRookBB); // bishop and rook already counted for 8pts
    return sum;
}

int scoreBoard(Board *b) {
    return getMaterialDiff(b);
    //return getMaterialDiff(b)*8 + doubledPawns(b->blackPawnBB) - doubledPawns(b->whitePawnBB);
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

int ComputerBoard::positionalEvalWhite(Board *b) {
    int kingSafety = 0;
    bool queenOut = false;
    int pawnPush = 0;
    int totalMaterial = 0;

    totalMaterial = getTotalMaterial(b);

    const uint64_t startingRankMask = 0xFF;
    if (((b->whiteRookBB & b->whiteBishopBB) & startingRankMask) != 0) {
        queenOut = true;
    }

    if (b->whiteKingPos > 55) {
        if (b->whiteKingPos == 3 || b->whiteKingPos == 4)
            kingSafety = 0;
        else
            kingSafety = 1;
    }
    else
        kingSafety = -1;

    // weight the given information and calculate score
    int score;
    if (totalMaterial >= 68)  { // early game
        score = (
            kingSafety * 2 +
            queenOut * -2
        );
    }
    else if (totalMaterial >= 35) { // middle game
        score = (
            kingSafety
        );
    }
    else { // endgame
        score = (
            0
        );
    }

    return score;
}

int ComputerBoard::positionalEvalBlack(Board *b) {
    int kingSafety = 0;
    bool queenOut = false;
    int totalMaterial = 0;

    totalMaterial = getTotalMaterial(b);

    const uint64_t startingRankMask = 0xFF00000000000000;
    if (((b->blackRookBB & b->blackBishopBB) & startingRankMask) != 0) {
        queenOut = true;
    }

    if (b->blackKingPos < 8) {
        if (b->blackKingPos == 3 || b->blackKingPos == 4)
            kingSafety = 0;
        else
            kingSafety = 1;
    }
    else
        kingSafety = -1;

    // weight the given information and calculate score
    int score;
    if (totalMaterial >= 68)  { // early game
        score = (
            kingSafety * 2 +
            queenOut * -2
        );
    }
    else if (totalMaterial >= 35) { // middle game
        score = (
            kingSafety
        );
    }
    else { // endgame
        score = (
            0
        );
    }

    return score;
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
