#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "EnhancedBoard.h"

class Gui {
private:
    SDL_Texture *images[12];
    SDL_Window *window;
    SDL_Renderer *renderer;
    EnhancedBoard *b;
    Move *sharedMove;
    Move lastMove;
    bool done = false;
    int selectedPos = -1;
    int selectedPos2 = -1;
    char checkmatePos = -1;
    bool isPieceSelected = false;
    bool promoting = false;
    bool isWhitePieces;

public:
    Gui(EnhancedBoard *board, Move *m, bool isWhite);

    void drawCircle(int centerX, int centerY, int radius);

    void drawChessBoard();

    void drawChessBoardFlipped();

    void loop();

    int runGui();
};
