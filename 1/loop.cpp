
#include <SDL2/SDL.h>
#include <stdint.h>
#include <time.h>
#include "frame.h"
#include "main.h"
#include "kmeans.h"




void loop(){    // HEADER
    
    while(1){
        
        SDL_Event event;
        while( SDL_PollEvent(&event)){
            switch( event.type ){
                
//                case SDL_MOUSEWHEEL:
//                    fprintf( stderr, "SDL_MOUSEWHEEL %d,%d\n"
//                        , event.wheel.x
//                        , event.wheel.y
//                    );
//                    break;

                case SDL_KEYDOWN:
                    fprintf( stderr, "SDL_KEYDOWN %s\n", SDL_GetKeyName(event.key.keysym.sym));
                    switch( event.key.keysym.sym ){
                        case SDLK_1:
                            kmeans_init();
                        break;
                        case SDLK_2:
                            kmeans_update();
                        break;
                    }
                    break;

//                case SDL_KEYUP:
//                    fprintf( stderr, "SDL_KEYUP %s\n", SDL_GetKeyName(event.key.keysym.sym));
//                    break;
//
//                case SDL_MOUSEMOTION:
//                    fprintf( stderr, "SDL_MOUSEMOTION %d,%d abs %d,%d\n"
//                        ,event.motion.xrel
//                        ,event.motion.yrel
//                        ,event.motion.x
//                        ,event.motion.y
//                    );
//                    break;
//
//                case SDL_MOUSEBUTTONDOWN:
//                    fprintf( stderr, "SDL_MOUSEBUTTONDOWN %d @ %d,%d\n",
//                        event.button.button, 
//                        event.button.x, 
//                        event.button.y
//                    );
//                    switch( event.button.button ){
//                        case SDL_BUTTON_MIDDLE: fprintf( stderr, "SDL_BUTTON_MIDDLE\n" ); break;
//                        case SDL_BUTTON_LEFT:   fprintf( stderr, "SDL_BUTTON_LEFT\n"   ); break;
//                        case SDL_BUTTON_RIGHT:  fprintf( stderr, "SDL_BUTTON_RIGHT\n"  ); break;
//                    }
//                    break;
//
//                case SDL_MOUSEBUTTONUP:
//                    fprintf( stderr, "SDL_MOUSEBUTTONUP %d @ %d,%d\n",
//                        event.button.button, 
//                        event.button.x, 
//                        event.button.y
//                    );
//                    switch( event.button.button ){
//                        case SDL_BUTTON_MIDDLE: fprintf( stderr, "SDL_BUTTON_MIDDLE\n" ); break;
//                        case SDL_BUTTON_LEFT:   fprintf( stderr, "SDL_BUTTON_LEFT\n"   ); break;
//                        case SDL_BUTTON_RIGHT:  fprintf( stderr, "SDL_BUTTON_RIGHT\n"  ); break;
//                    }
//                    break;

                case SDL_QUIT:
//                    fprintf( stderr, "SDL_QUIT\n" );
                    return;
            }
        }

        struct timespec start, end;
        clock_gettime(CLOCK_REALTIME, &start);

        frame( event );

        clock_gettime(CLOCK_REALTIME, &end);
        float lap = (end.tv_sec-start.tv_sec)+(end.tv_nsec-start.tv_nsec)*1E-9;
//        fprintf(stderr,"lap %.4f secs\n",lap);

        SDL_RenderPresent(renderer);
        SDL_Delay(1);
    }
}    




