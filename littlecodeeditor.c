#include "SDL.h"
#include "SDL_ttf.h"
#include <stdio.h>
#include <stdlib.h>

#define DEFAULT_WIN_WIDTH 1280
#define DEFAULT_WIN_HEIGHT 720


const char *get_all_assci_char() {
    uint8_t *all_assci_char = (uint8_t*)malloc(96);
    uint8_t c = 0x20;
    while (c != 0x7f) {
        *(all_assci_char + (c - 0x20)) = c;
        c++;
    }
    *(all_assci_char + (c - 0x20)) = 0;

    return (const char*)all_assci_char;
}

int show_char(int row, int col, uint8_t c, SDL_Renderer *renderer, SDL_Texture *font_texture, SDL_Surface *font_surface) {
    SDL_Rect r1;
    SDL_Rect r2;
    int fw;
    int fh;
    fw = (font_surface->w) / 95;
    fh = font_surface->h;

    r1.x = fw * (int)(c - 0x20);
    r1.y = 0;
    r1.w = fw;
    r1.h = fh;

    r2.x = 0;
    r2.y = 0;
    r2.w = fw;
    r2.h = fh;
    SDL_RenderCopy(renderer, font_texture, &r1, &r2);
}

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *window = SDL_CreateWindow("Text Editor",
                            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                            1280, 720, 0);

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);
    SDL_RenderClear(renderer);

    TTF_Init();

    TTF_Font *default_font = TTF_OpenFont("default-fonts/Hack-Regular.ttf", 60);

    SDL_Color fg;
    SDL_Color bg;

    fg.r = 0xff;
    fg.g = 0xff;
    fg.b = 0xff;
    fg.a = 0xff;

    bg.r = 0x27;
    bg.g = 0x27;
    bg.b = 0x27;
    bg.a = 0xff;


    SDL_Surface *font_surface = TTF_RenderUTF8_LCD(default_font, get_all_assci_char(), fg, bg);
    SDL_Texture *font_texture = SDL_CreateTextureFromSurface(renderer, font_surface);
    show_char(0, 0, 'K', renderer, font_texture, font_surface);

    SDL_RenderPresent(renderer);

    SDL_Delay(5000);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    TTF_CloseFont(default_font);
    TTF_Quit();
    SDL_Quit();
    return 0;
}