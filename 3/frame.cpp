
#include <SDL2/SDL.h>   // HEADER
#include "main.h"
#include "kmeans.h"



void frame( SDL_Event event, float dt ){  // HEADER

    SDL_SetRenderDrawColor( renderer, 0,0,0, 255 );
    SDL_RenderClear( renderer );
                
    static bool started;
    if(!started) started=kmeans_run();

    kmeans_render(dt);
}


