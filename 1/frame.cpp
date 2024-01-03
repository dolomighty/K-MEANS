
#include <SDL2/SDL.h>   // HEADER
#include "main.h"
#include "kmeans.h"





static void pix( int x, int y, int r, int g, int b ){
    SDL_SetRenderDrawColor( renderer, r,g,b, 255 );
    SDL_RenderDrawPoint( renderer, x, y );
}



void frame( SDL_Event event ){  // HEADER

    SDL_SetRenderDrawColor( renderer, 0,0,0, 255 );
    SDL_RenderClear( renderer );
                
//    pix( rand()%WINW, rand()%WINH, rand()%256, rand()%256, rand()%256 );
//    pix( event.motion.x, event.motion.y, 255, 255, 255 );

    kmeans_render();

    // attract mode
    static Uint32 step_deadline = 0;
    if( SDL_GetTicks() < step_deadline )return;
    step_deadline = SDL_GetTicks() + 200;

    static int step = 0;
    step++;
    if(step>=10){
        step=0;
        kmeans_init();
        return;
    }
    kmeans_update();
}


