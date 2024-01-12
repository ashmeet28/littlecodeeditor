#include "SDL.h"
#include "SDL_ttf.h"
#include <stdio.h>

#define DEFAULT_WIN_WIDTH 1280
#define DEFAULT_WIN_HEIGHT 720

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *window = SDL_CreateWindow("Text Editor",
                            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                            1280, 720, 0);

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);
    SDL_RenderClear(renderer);

    SDL_Texture *texture = SDL_CreateTexture(renderer,
                            SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING,
                            DEFAULT_WIN_WIDTH, DEFAULT_WIN_HEIGHT);

    SDL_Surface *window_surface = SDL_GetWindowSurface(window);


    TTF_Init();

    TTF_Font *default_font = TTF_OpenFont("default-fonts/Hack-Regular.ttf", 40);

    SDL_Color fg;
    SDL_Color bg;

    fg.r = 0xff;
    fg.a = 0xff;

    bg.r = 0xff;
    bg.g = 0xff;
    bg.b = 0xff;
    bg.a = 0xff;

    SDL_Surface *surface_1 = TTF_RenderUTF8_LCD(default_font, "Hello World", fg, bg);
    SDL_LockSurface(surface_1);
    SDL_Surface *surface_2 = SDL_ConvertSurfaceFormat(surface_1, SDL_PIXELFORMAT_RGBA8888, 0);
    SDL_LockSurface(surface_2);

    SDL_Rect r = surface_2->clip_rect;
    r.x += 100;
    r.y += 100;
    SDL_UpdateTexture(texture, &(surface_2->clip_rect), surface_2->pixels, surface_2->pitch);

    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);

    SDL_Delay(5000);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    TTF_CloseFont(default_font);
    TTF_Quit();
    SDL_Quit();
    return 0;
}