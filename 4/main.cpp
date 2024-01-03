
#include <stdlib.h>
#include <unistd.h>
#include <SDL2/SDL.h>   // HEADER
#include <assert.h>
#include "loop.h"

#define WINW 600    // HEADER
#define WINH 600    // HEADER

#define COUNT(ARR) (sizeof(ARR)/sizeof(ARR[0]))  // HEADER



SDL_Renderer *renderer;     // HEADER



int main( int argc, char *argv[] ){ 

    char *basename = strrchr(argv[0],'/')+1;

    SDL_Window *win;
    assert( win = 
        SDL_CreateWindow( basename, 
            SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 
                WINW, WINH, 0 ));

    assert( renderer = SDL_CreateRenderer( win, -1,
        SDL_RENDERER_PRESENTVSYNC  |
        SDL_RENDERER_TARGETTEXTURE
    ));

    loop();
    
    SDL_DestroyRenderer( renderer );
    SDL_DestroyWindow( win );
    
    return 0;
}



