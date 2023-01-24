#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "Gui.h"

using namespace std;


const int FPS_CAP = 30;

// screen and box sizes
const int WIDTH = 600;
const int HEIGHT = 600;
const int BOX_WIDTH = WIDTH / 8;
const int BOX_HEIGHT = HEIGHT / 8;

// circle radius and offsets
const int CIRCLE_RADIUS = BOX_WIDTH / 6;
const int BOX_WIDTH_CENTER = BOX_WIDTH / 2;
const int BOX_HEIGHT_CENTER = BOX_HEIGHT / 2;

// image folder and filenames
const char *IMG_FOLDER = "images/";
const char *FILENAMES[] = {
        "wk.png", "wq.png", "wr.png", "wb.png", "wn.png", "wp.png",
        "bk.png", "bq.png", "br.png", "bb.png", "bn.png", "bp.png"
};


Gui::Gui(EnhancedBoard *board, Move *m, bool isWhite) {
    b = board;
    sharedMove = m;
    lastMove = b->getLastMove();
    isWhitePieces = isWhite;
}

void Gui::drawCircle(int centerX, int centerY, int radius){
    int diameter = radius * 2;
    int radiusSquared = radius * radius;
    int dx, dy;

    for (int w = 0; w < diameter; w++) {
        for (int h = 0; h < diameter; h++) {
            dx = radius - w;
            dy = radius - h;
            if ((dx*dx + dy*dy) <= radiusSquared) {
                SDL_RenderDrawPoint(renderer, centerX + dx, centerY + dy);
            }
        }
    }
}

void Gui::drawChessBoard() {
    SDL_RenderClear(renderer);

    SDL_Rect rect;

    rect.w = BOX_WIDTH;
    rect.h = BOX_HEIGHT;

    bool isWhite = true;
    char piece;
    for (int x = 0; x < 8; x++) {
        for (int y = 0; y < 8; y++) {
            rect.x = x * rect.w;
            rect.y = y * rect.h;
            int pos = y * 8 + x;

            if ((isPieceSelected && selectedPos == pos) ||
                (lastMove.from == pos) || (lastMove.to == pos))
            {
                if (isWhite)
                    SDL_SetRenderDrawColor(renderer, 240, 232, 129, 255);
                else
                    SDL_SetRenderDrawColor(renderer, 138, 130, 33, 255);
            }
            else if (isWhite)
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            else
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

            if (checkmatePos == pos)
                SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);

            SDL_RenderFillRect(renderer, &rect);

            // blit images here
            piece = b->getPos(pos);
            if (piece != noPiece) {
                if (piece < 0) {
                    SDL_RenderCopy(renderer, images[abs(piece) + 5], NULL, &rect);
                }
                else {
                    SDL_RenderCopy(renderer, images[piece - 1], NULL, &rect);
                }
            }

            isWhite = !isWhite;
        }
        isWhite = !isWhite;
    }

    if (promoting) {
        // draw box
        SDL_Rect rect;
        rect.w = BOX_WIDTH - 10;
        rect.h = BOX_WIDTH * 4 - 10;
        rect.x = FILE(selectedPos2) * BOX_WIDTH + 5;
        rect.y = RANK(selectedPos2) * BOX_HEIGHT + 5;
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderFillRect(renderer, &rect);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderDrawRect(renderer, &rect);

        // add pieces
        rect.h = BOX_HEIGHT - 10;
        SDL_RenderCopy(renderer, images[1], NULL, &rect);
        rect.y += BOX_HEIGHT;
        SDL_RenderCopy(renderer, images[2], NULL, &rect);
        rect.y += BOX_HEIGHT;
        SDL_RenderCopy(renderer, images[3], NULL, &rect);
        rect.y += BOX_HEIGHT;
        SDL_RenderCopy(renderer, images[4], NULL, &rect);
    }
    else if (isPieceSelected) {
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, 130, 130, 130, 150);
        vector<Move> *moves = b->getAllValidMoves();
        for (Move m : *moves) {
            if (m.from == selectedPos) {
                // manually draw pawns that could promote bc promotion piece is store in destination rank
                if (b->getPos(selectedPos) == whitePawn && RANK(selectedPos) == 1) {
                    if (RANK(m.to) == whiteQueen)
                        drawCircle(FILE(m.to) * BOX_WIDTH + BOX_WIDTH_CENTER, BOX_HEIGHT_CENTER, CIRCLE_RADIUS);
                }
                else
                    drawCircle(FILE(m.to) * BOX_WIDTH + BOX_WIDTH_CENTER, RANK(m.to) * BOX_HEIGHT + BOX_HEIGHT_CENTER, CIRCLE_RADIUS);
            }
        }
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
    }

    SDL_RenderPresent(renderer);
}

void Gui::drawChessBoardFlipped() {
    SDL_RenderClear(renderer);

    SDL_Rect rect;

    rect.w = BOX_WIDTH;
    rect.h = BOX_HEIGHT;

    bool isWhite = true;
    char piece;
    for (int x = 0; x < 8; x++) {
        for (int y = 0; y < 8; y++) {
            rect.x = x * rect.w;
            rect.y = y * rect.h;
            int pos = y * 8 + x;

            if ((isPieceSelected && selectedPos == 63 - pos) ||
                (lastMove.from == 63 - pos) || (lastMove.to == 63 - pos))
            {
                if (isWhite)
                    SDL_SetRenderDrawColor(renderer, 240, 232, 129, 255);
                else
                    SDL_SetRenderDrawColor(renderer, 138, 130, 33, 255);
            }
            else if (isWhite)
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            else
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

            if (checkmatePos == pos)
                SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);

            SDL_RenderFillRect(renderer, &rect);

            // blit images here
            piece = b->getPos(63 - pos);
            if (piece != noPiece) {
                if (piece < 0) {
                    SDL_RenderCopy(renderer, images[abs(piece) + 5], NULL, &rect);
                }
                else {
                    SDL_RenderCopy(renderer, images[piece - 1], NULL, &rect);
                }
            }

            isWhite = !isWhite;
        }
        isWhite = !isWhite;
    }

    if (promoting) {
        // draw box
        SDL_Rect rect;
        rect.w = BOX_WIDTH - 10;
        rect.h = BOX_WIDTH * 4 - 10;
        rect.x = FILE(63 - selectedPos2) * BOX_WIDTH + 5;
        rect.y = RANK(63 - selectedPos2) * BOX_HEIGHT + 5;
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderFillRect(renderer, &rect);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderDrawRect(renderer, &rect);

        // add pieces
        rect.h = BOX_HEIGHT - 10;
        SDL_RenderCopy(renderer, images[7], NULL, &rect);
        rect.y += BOX_HEIGHT;
        SDL_RenderCopy(renderer, images[8], NULL, &rect);
        rect.y += BOX_HEIGHT;
        SDL_RenderCopy(renderer, images[9], NULL, &rect);
        rect.y += BOX_HEIGHT;
        SDL_RenderCopy(renderer, images[10], NULL, &rect);
    }
    else if (isPieceSelected) {
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, 130, 130, 130, 150);
        vector<Move> *moves = b->getAllValidMoves();
        for (Move m : *moves) {
            if (m.from == selectedPos) {
                // manually draw pawns that could promote bc promotion piece is store in destination rank
                if (b->getPos(selectedPos) == blackPawn && RANK(selectedPos) == 6) {
                    if (RANK(m.to) == whiteQueen)
                        drawCircle(FILE(63 - m.to) * BOX_WIDTH + BOX_WIDTH_CENTER, BOX_HEIGHT_CENTER, CIRCLE_RADIUS);
                }
                else
                    drawCircle(FILE(63 - m.to) * BOX_WIDTH + BOX_WIDTH_CENTER, RANK(63 - m.to) * BOX_HEIGHT + BOX_HEIGHT_CENTER, CIRCLE_RADIUS);
            }
        }
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
    }

    SDL_RenderPresent(renderer);
}

void Gui::loop() {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            done = true;
            return;
        }
        if (e.type == SDL_MOUSEBUTTONUP) {
            int x, y;
            SDL_GetMouseState( &x, &y );
            int mousePos = (y / BOX_HEIGHT * 8) + (x / BOX_WIDTH);
            if (!isWhitePieces)
                mousePos = 63 - mousePos;
            if (promoting) {
                if (FILE(mousePos) == FILE(selectedPos2)) {
                    int promoteTo = -1;
                    int mouseY = y / BOX_HEIGHT;
                    if (mouseY == 0)
                        promoteTo = whiteQueen;
                    else if (mouseY == 1)
                        promoteTo = whiteRook;
                    else if (mouseY == 2)
                        promoteTo = whiteBishop;
                    else if (mouseY == 3)
                        promoteTo = whiteKnight;

                    if (promoteTo != -1) {
                        Move m = {selectedPos, (selectedPos2 & 0b111) | (promoteTo << 3)};

                        if (b->isValidMove(m)) {
                            if (sharedMove->from == -1)
                                *sharedMove = m;
                        }
                    }
                }
                promoting = false;
            }
            else if (!isPieceSelected) {
                selectedPos = mousePos;
                isPieceSelected = true;
            }
            else {
                Move m;
                if ((b->getPos(selectedPos) == whitePawn && RANK(selectedPos) == 1) ||
                    (b->getPos(selectedPos) == blackPawn && RANK(selectedPos) == 6)) 
                {
                    m = {selectedPos, (FILE(mousePos) | (whiteQueen << 3))};
                    if (b->isValidMove(m)) {
                        selectedPos2 = mousePos;
                        promoting = true;
                    }
                }
                else {
                    m = {selectedPos, mousePos};

                    if (b->isValidMove(m)) {
                        if (sharedMove->from == -1)
                            *sharedMove = m;
                    }
                }

                isPieceSelected = false;
            }
        }
    }

    int status = b->getStatus();
    if (status == whiteWins) {
        for (int pos = 0; pos < 64; pos++) {
            if (b->getPos(pos) == blackKing)
                checkmatePos = pos;
        }
    }
    else if (status == blackWins) {
        for (int pos = 0; pos < 64; pos++) {
            if (b->getPos(pos) == whiteKing)
                checkmatePos = pos;
        }
    }

    lastMove = b->getLastMove();

    if (isWhitePieces)
        drawChessBoard();
    else
        drawChessBoardFlipped();
}

int Gui::runGui() {
    if(SDL_Init(SDL_INIT_VIDEO) != 0)
        return 1;

    window = SDL_CreateWindow("Chess Board", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_MAXIMIZED);
    if(!window)
        return 1;

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if(!renderer)
        return 1;

    char path[32];
    memcpy(path, IMG_FOLDER, strlen(IMG_FOLDER));
    for (int i = 0; i < 12; i++) {
        memcpy(path + strlen(IMG_FOLDER), FILENAMES[i], strlen(FILENAMES[i]) + 1);
        images[i] = IMG_LoadTexture(renderer, path);
    }

    unsigned int a = SDL_GetTicks();
    unsigned int b = SDL_GetTicks();
    double delta = 0;

    while (!done) {
        a = SDL_GetTicks();
        delta = a - b;

        SDL_Delay((1000.0/FPS_CAP) - delta);

        loop();

        b = SDL_GetTicks();
    }

    for (SDL_Texture *img : images) {
        SDL_DestroyTexture(img);
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
