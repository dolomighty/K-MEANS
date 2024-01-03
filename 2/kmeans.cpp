
#include <SDL2/SDL.h>
#include "main.h"
#include "XY.h"




XY means[3];    // i gruppi
XY samples[350];
int colors[COUNT(samples)];





static float unirand(){
    // return [0,1]
    return rand()*1.0/RAND_MAX;
}

static float biporand(){
    // return [-1,1]
    return rand()*2.0/RAND_MAX-1.0;
}



static float lerp( float a, float b, float x ){
    return a+(b-a)*x;
}


static float randgauss(){
    return (biporand()+biporand()+biporand()+biporand()+biporand())/5.0;
}


static XY gaussian( XY center, float radius ){
    return center+XY(randgauss(),randgauss())*radius;
}







static int find_color( XY sample ){
    // usa le medie attuali per colorare il sample
    float min_dd = 1E6; // +inf
    int color; // un sample è sicuramente di un colore, non serve init
    for( int m=0; m<COUNT(means); m++ ){
        XY d = sample-means[m];
        float dd = sqrt(d*d);
        if( min_dd < dd )continue;
        min_dd = dd;
        color = m;
    }
    return color;
}



void kmeans_init(){ // HEADER
    
//    means[0] = XY( 
//        lerp( WINW*0.0/2, WINW*1.0/2, unirand()),
//        lerp( WINH*0.0/2, WINH*2.0/2, unirand())
//    );
//
//    means[1] = XY( 
//        lerp( WINW*1.0/2, WINW*2.0/2, unirand()),
//        lerp( WINH*0.0/2, WINH*2.0/2, unirand())
//    );




//    for( int s=0; s<COUNT(samples); s++ ){
//
//        auto xy = XY(
//            lerp( WINW*0.0/2, WINW*2.0/2, unirand()),
//            lerp( WINH*0.0/2, WINH*2.0/2, unirand())
//        );
//
//        samples[s] = xy;
//        int color = find_color(xy);
//        colors[s] = color;
//    }



    int s = 0;

    for( int i=0; i<COUNT(means); i++ ){
        auto center = XY(
            lerp( 0, WINW, unirand()),
            lerp( 0, WINH, unirand())
        );
        auto radius = lerp( WINH/10, WINH/5, unirand());
        for( int j=0; j<100; j++ ){
            auto xy = gaussian( center, radius );
            samples[s++] = xy;
        }
    }

    while( s<COUNT(samples)){
        auto xy = XY(
            lerp( 0, WINW, unirand()),
            lerp( 0, WINH, unirand())
        );
        samples[s++] = xy;
    }
}





void kmeans_update(){ // HEADER    

    struct {
        XY accu;
        int n;
    } tmp_means[COUNT(means)];

    bzero(tmp_means,sizeof(tmp_means));

    for( int s=0; s<COUNT(samples); s++ ){
        int color = find_color(samples[s]);
        colors[s]=color;
        tmp_means[color].n++;
        tmp_means[color].accu = tmp_means[color].accu + samples[s];
    }

    for( int color=0; color<COUNT(means); color++ ){
        if(tmp_means[color].n<=0)continue;
        means[color] = tmp_means[color].accu / tmp_means[color].n;
    }
}



static void pix( XY xy, int radius=3 ){
    SDL_Rect r ={
        .x = (int)(xy.x-(radius/2)),
        .y = (int)(xy.y-(radius/2)),
        .w = radius,
        .h = radius,
    };
    SDL_RenderFillRect( renderer, &r );
}


void kmeans_render(){ // HEADER
    for( int s=0; s<COUNT(samples); s++ ){
        if(colors[s]==0) SDL_SetRenderDrawColor( renderer, 255,64,0,   255 );
        if(colors[s]==1) SDL_SetRenderDrawColor( renderer, 0,255,0,   255 );
        if(colors[s]==2) SDL_SetRenderDrawColor( renderer, 255,64,255, 255 );
        pix( samples[s] );
    }
    SDL_SetRenderDrawColor( renderer, 255,255,255, 255 );
    for( int m=0; m<COUNT(means); m++ ){
        pix( means[m], 5 );
    }
}



