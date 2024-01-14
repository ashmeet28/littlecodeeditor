#include "SDL.h"
#include "SDL_ttf.h"
#include <stdio.h>
#include <stdlib.h>

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

#define DEFAULT_WIN_WIDTH 1280
#define DEFAULT_WIN_HEIGHT 720

#define ASCII_PRINTABLE_CHARS_COUNT 95

#define ASCII_RENDERER_CHAR_GRID_CAP 1048576

typedef struct {
    int w;
    int h;
    int fw;
    int fh;
    uint8_t *char_grid;
    SDL_Renderer *main_renderer;
    SDL_Surface *regular_font_surface;
    SDL_Texture *regular_font_texture;
} ASCII_RENDERER;

SDL_Surface *create_regular_ascii_font_surface(int ptsize) {
    TTF_Font *font = TTF_OpenFont("fonts/Hack-Regular.ttf", ptsize);

    uint8_t *printable_chars = (uint8_t*)malloc(ASCII_PRINTABLE_CHARS_COUNT + 1);
    uint8_t c = 0x20;
    while (c != 0x7f) {
        printable_chars[c - 0x20] = c;
        c++;
    }
    printable_chars[c - 0x20] = 0;

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

    SDL_Surface *font_surface = TTF_RenderUTF8_LCD(font, (const char *)printable_chars, fg, bg);

    TTF_CloseFont(font);
    free((void *)printable_chars);

    return font_surface;
}

ASCII_RENDERER *create_assci_renderer(SDL_Window *window, int ptsize) {
    ASCII_RENDERER *asc_rend = (ASCII_RENDERER *)malloc(sizeof(ASCII_RENDERER));

    SDL_GetWindowSize(window, &(asc_rend->w), &(asc_rend->h));

    asc_rend->main_renderer = SDL_CreateRenderer(window, -1, 0);
    asc_rend->regular_font_surface = create_regular_ascii_font_surface(ptsize);
    asc_rend->fw = (asc_rend->regular_font_surface->w) / ASCII_PRINTABLE_CHARS_COUNT;
    asc_rend->fh = (asc_rend->regular_font_surface->h);
    asc_rend->regular_font_texture = SDL_CreateTextureFromSurface(
                                        asc_rend->main_renderer, asc_rend->regular_font_surface);

    asc_rend->char_grid = (uint8_t *)malloc(ASCII_RENDERER_CHAR_GRID_CAP);
    for(int i = 0; i < ASCII_RENDERER_CHAR_GRID_CAP; i++) {
        asc_rend->char_grid[i] = 0x20;
    }

    return asc_rend;
}

void ascii_renderer_print_char(ASCII_RENDERER *asc_rend, uint8_t c, int x, int y) {
    SDL_Rect r1;
    SDL_Rect r2;

    int fw = asc_rend->fw;
    int fh = asc_rend->fh;

    r1.x = fw * (int)(c - 0x20);
    r1.y = 0;
    r1.w = fw;
    r1.h = fh;

    r2.x = fw * x;
    r2.y = fh * y;
    r2.w = fw;
    r2.h = fh;

    SDL_RenderCopy(asc_rend->main_renderer, asc_rend->regular_font_texture, &r1, &r2);
}

int ascii_renderer_cols(ASCII_RENDERER *asc_rend) {
    return asc_rend->w / asc_rend->fw;
}

int ascii_renderer_rows(ASCII_RENDERER *asc_rend) {
    return asc_rend->h / asc_rend->fh;
}

void ascii_renderer_set_char(ASCII_RENDERER *asc_rend, uint8_t c, int x, int y) {
    asc_rend->char_grid[ascii_renderer_cols(asc_rend) * y + x] = c;
}

uint8_t ascii_renderer_get_char(ASCII_RENDERER *asc_rend, int x, int y) {
   return asc_rend->char_grid[(ascii_renderer_cols(asc_rend) * y) + x];
}

void ascii_renderer_present(ASCII_RENDERER *asc_rend) {
    SDL_RenderClear(asc_rend->main_renderer);

    int rows = ascii_renderer_rows(asc_rend);
    int cols = ascii_renderer_cols(asc_rend);

    for(int y = 0; y < rows; y++) {
        for(int x = 0; x < cols; x++) {
            ascii_renderer_print_char(asc_rend,
                ascii_renderer_get_char(asc_rend, x, y), x, y);
        }
    }

    SDL_RenderPresent(asc_rend->main_renderer);
}

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);
    SDL_Window *window = SDL_CreateWindow("Text Editor",
                            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                            DEFAULT_WIN_WIDTH, DEFAULT_WIN_HEIGHT, 0);

    TTF_Init();

    ASCII_RENDERER *asc_rend = create_assci_renderer(window, 40);
    
    char c[] = "Hello World";

    for (int i = 0; i < ARRAY_SIZE(c) - 1; i++) {
        ascii_renderer_set_char(asc_rend, c[i], i, 0);
        ascii_renderer_present(asc_rend);
        SDL_Delay(300);
    }

    TTF_Quit();
    SDL_Quit();
    return 0;
}