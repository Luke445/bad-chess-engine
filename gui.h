#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "board.h"

class Gui {
private:
    SDL_Texture *images[12];
    SDL_Window *window;
    SDL_Renderer *renderer;
    Board *b;
    Move *sharedMove;
    Move lastMove;
    bool done = false;
    char selectedPos = -1;
    char selectedPos2 = -1;
    char checkmatePos = -1;
    bool isPieceSelected = false;
    bool promoting = false;


public:
    Gui(Board *board, Move *m);

    void drawCircle(int centerX, int centerY, int radius);

    void drawChessBoard();

    void loop();

    int runGui();
};
