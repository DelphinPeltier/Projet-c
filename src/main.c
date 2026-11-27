#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdio.h>
#include <SDL2/SDL_ttf.h>
#include "../include/system_metrics.h"
#include "../include/memory.h"
#include "../include/cpu.h"

int main(int argc, char *argv[]) {
    (void)argc;
    (void)argv;

    SystemSnapshot snap;
    if (get_cpu_info(&snap) == 0){
        printf("CPU : %.1f%%\n", snap.cpu_total_usage);
        printf("Nombre de coeurs : %d\n", snap.cpu_core_count);
        printf("Fréquence du CPU : %.1f MHz\n", snap.cpu_frequency_mhz);
    }
    if (get_memory_info(&snap) == 0){
        printf("Mémoire totale : %lu KB\n", snap.mem_total_kb);
        printf("Mémoire utilisée : %lu KB\n", snap.mem_used_kb);
        printf("Mémoire libre : %lu KB\n", snap.mem_free_kb);
        printf("Pourcentage de mémoire utilisée : %.1f%%\n", snap.mem_used_pourcentage);
    }

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("Error: %s\n", SDL_GetError());
        return 1;
    }

    if (TTF_Init() != 0) {
        printf("TTF_Init Error: %s\n", TTF_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow(
        "SDL2 window",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        680, 480,
        SDL_WINDOW_SHOWN
    );

    if (window == NULL) {
        printf("Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        printf("Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    //texte
    TTF_Font *font = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 28);
    if (!font) {
        printf("Erreur chargement police: %s\n", TTF_GetError());
    }
    SDL_Color blanc = {255, 255, 255, 255};
    SDL_Surface *surface = TTF_RenderText_Blended(font, "Hello World !",  blanc);
    if (!surface) {
        printf("Erreur chargement surface: %s\n", TTF_GetError());
    }
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
        printf("Erreur chargement texture: %s\n", SDL_GetError());
    }
    SDL_Rect text_rect;
    text_rect.x = 200; 
    text_rect.y = 200; 
    text_rect.w = surface->w; 
    text_rect.h = surface->h; 
    SDL_FreeSurface(surface);


    bool running = true;
    SDL_Event event;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
        }

        SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, NULL, &text_rect);
        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    SDL_DestroyTexture(texture);
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}

