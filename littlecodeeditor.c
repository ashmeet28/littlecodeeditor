#include "SDL.h"
#include "SDL_ttf.h"
#include <stdio.h>

int main(int argc, char* argv[]) {

    SDL_Window *window;

    SDL_Init(SDL_INIT_VIDEO);
    window = SDL_CreateWindow(
        "An SDL2 window",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        640, 480,
        SDL_WINDOW_RESIZABLE
    );

    SDL_Renderer *renderer;

    SDL_Surface *window_surface;
    window_surface = SDL_GetWindowSurface(window);


    TTF_Init();

    TTF_Font *default_font;

    default_font = TTF_OpenFont("/usr/share/fonts/TTF/Hack-Regular.ttf", 40);

    SDL_Color fg;
    SDL_Color bg;

    fg.r = 0xff;
    fg.a = 0xff;

    bg.r = 0xff;
    bg.g = 0xff;
    bg.b = 0xff;
    bg.a = 0xff;

    SDL_Surface *surface_1;
    surface_1 = TTF_RenderUTF8_LCD(default_font, "Hello World", fg, bg);
    SDL_BlitSurface(surface_1, NULL, window_surface, NULL);

    SDL_UpdateWindowSurface(window);
    SDL_Delay(5000);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    TTF_CloseFont(default_font);
    TTF_Quit();
    SDL_Quit();
    return 0;
}