
#include <SDL2/SDL.h>   // HEADER
#include "main.h"
#include "kmeans.h"



void frame( float dt ){  // HEADER

    SDL_SetRenderDrawColor( renderer, 0,0,0, 255 );
    SDL_RenderClear( renderer );
                
    kmeans_render(dt);
}


