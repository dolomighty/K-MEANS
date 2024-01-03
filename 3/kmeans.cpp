

#include <SDL2/SDL.h>
#include "main.h"
#include "XY.h"
#include <vector>
#include <random>
#include <thread>
#include <chrono>



auto means = std::vector<XY>(3);
std::vector<XY> samples;
std::vector<int> colors;




static float lerp( float a, float b, float x ){
    return a+(b-a)*x;
}


static float unirand(){
    // return [0,1]
    return rand()*1.0/RAND_MAX;
}


static float biporand(){
    // return [-1,1]
    return rand()*2.0/RAND_MAX-1.0;
}


static float gaussrand(){
    static std::random_device rd{};
    static std::mt19937 gen{rd()};
    static std::normal_distribution d{ 0.0, 0.5 };
    return d(gen);
}


static XY gaussian_blob( XY center, float radius ){
    return center+XY(gaussrand(),gaussrand())*radius;
}






static int kmeans_classify( XY sample ){
    // usa le medie attuali per colorare il sample
    float min_dd = 1E6; // +inf
    int color; // un sample è sicuramente di un colore, non serve init
    for( int m=0; m<means.size(); m++ ){
        XY d = sample-means[m];
        float dd = sqrt(d*d);
//        float dd = d*d;
        if( min_dd < dd )continue;
        min_dd = dd;
        color = m;
    }
    return color;
}



static void kmeans(){
    
//    means[0] = XY( 
//        lerp( WINW*0.0/2, WINW*1.0/2, unirand()),
//        lerp( WINH*0.0/2, WINH*2.0/2, unirand())
//    );
//
//    means[1] = XY( 
//        lerp( WINW*1.0/2, WINW*2.0/2, unirand()),
//        lerp( WINH*0.0/2, WINH*2.0/2, unirand())
//    );




//    for( int s=0; s<samples.size(); s++ ){
//
//        auto xy = XY(
//            lerp( WINW*0.0/2, WINW*2.0/2, unirand()),
//            lerp( WINH*0.0/2, WINH*2.0/2, unirand())
//        );
//
//        samples[s] = xy;
//        int color = kmeans_classify(xy);
//        colors[s] = color;
//    }


    samples.clear();

    // blobs/splotches
    for( int i=0; i<means.size(); i++ ){
        auto center = XY(
            lerp( WINW*1/10, WINW*9/10, unirand()),
            lerp( WINH*1/10, WINH*9/10, unirand())
        );
        auto radius = lerp( WINH/10, WINH/5, unirand());
        for( int j=0; j<100; j++ ){
            samples.push_back( gaussian_blob( center, radius ));
        }
    }

    // noise
    for( int i=0; i<20; i++ ){
        samples.push_back(XY(
            lerp( 0, WINW, unirand()),
            lerp( 0, WINH, unirand())
        ));
    }

    colors.clear();
    colors.resize(samples.size());


    const int max_iters = 100;
    for( int n=0; n<max_iters; n++ ){ 
        std::this_thread::sleep_for(std::chrono::milliseconds( 100 ));
//        fprintf(stderr,"iter %d\n",n);

        auto means_pre = means;

        struct ACCU_N {
            XY accu;
            int n;
        };

        std::vector<ACCU_N> tmp_means;
        tmp_means.resize(means.size());

        // apparentemente .resize non chiama i costruttori...
        for( int color=0; color<means.size(); color++ ){
            tmp_means[color].accu = XY();
            tmp_means[color].n = 0;
//            fprintf(stderr,"%f %f %d\n"
//                , tmp_means[color].accu.x
//                , tmp_means[color].accu.y
//                , tmp_means[color].n
//            );
        }


        for( int s=0; s<samples.size(); s++ ){
            int color = kmeans_classify(samples[s]);
            colors[s]=color;
            tmp_means[color].n++;
            tmp_means[color].accu = tmp_means[color].accu + samples[s];
        }

        bool stable = true;

        for( int color=0; color<means.size(); color++ ){
//            fprintf(stderr,"%d\n",tmp_means[color].n);
            if(tmp_means[color].n<=0)continue;
            means[color] = tmp_means[color].accu / tmp_means[color].n;
//            fprintf(stderr,"%f %f == %f %f\n"
//                , means[color].x
//                , means[color].y
//                , means_pre[color].x
//                , means_pre[color].y
//            );
            if(stable && means[color].x != means_pre[color].x) stable=false;
            if(stable && means[color].y != means_pre[color].y) stable=false;
        }

        if(stable)break;
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


void kmeans_render( float dt ){ // HEADER

    const int mac_palette[][3]={
        { 255, 255, 255 },
        { 252, 244, 4   },
        { 255, 100, 4   },
        { 220, 8,   8   },
        { 240, 8,   132 },
        { 72,  0,   164 },
        { 0,   0,   212 },
        { 0,   172, 232 },
        { 32,  184, 20  },
        { 0,   100, 16  },
        { 88,  44,  4   },
        { 144, 112, 56  },
        { 192, 192, 192 },
        { 128, 128, 128 },
        { 64,  64,  64  },
        { 0,   0,   0   },
    };

    for( int s=0; s<samples.size(); s++ ){
        const auto rgb = mac_palette[colors[s]];
        SDL_SetRenderDrawColor( renderer, rgb[0],rgb[1],rgb[2], 255 );
        pix( samples[s] );
    }

    static float blink;
    blink += dt;

    for( int m=0; m<means.size(); m++ ){
        const auto rgb = fmod(blink,1)>0.5 ? mac_palette[m] : mac_palette[14];
        SDL_SetRenderDrawColor( renderer, rgb[0],rgb[1],rgb[2], 255 );
        pix( means[m], 10 );
    }
}












static void kmeans_loop(){
    while(true){
        kmeans();
        std::this_thread::sleep_for(std::chrono::seconds( 2 ));
    }
}


bool kmeans_run(){ // HEADER
    static std::thread helper(kmeans_loop);
    return true;
}

