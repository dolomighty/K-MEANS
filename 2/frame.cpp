
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

    static int step = 0;
    const int steps = 30;

    kmeans_render();

    {
        const int barh = 3;
        SDL_Rect r ={
            .x = 0,
            .y = WINH-barh,
            .w = step*WINW/steps,
            .h = barh,
        };
        SDL_SetRenderDrawColor( renderer, 255,255,255, 255 );
        SDL_RenderFillRect( renderer, &r );
    }


    // attract mode
    static Uint32 step_deadline = 0;
    if( SDL_GetTicks() < step_deadline )return;
    step_deadline = SDL_GetTicks() + 100;

    step++;
    if(step>=steps){
        step=0;
        kmeans_init();
        return;
    }
    kmeans_update();
}


