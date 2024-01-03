

#include <SDL2/SDL.h>
#include "main.h"
#include "XY.h"
#include <vector>
#include <random>
#include <thread>
#include <chrono>
#include <deque>


// per generare i datapoints
auto gauss_centers = std::vector<XY>(3);
auto gauss_rads    = std::vector<float>(3);

auto means = std::vector<XY>(3);
auto stdev = std::vector<float>(3);

struct POINT {
    XY xy;
    int color;
    POINT(){
        color = -1; // non categorizzato
    }
};

std::deque<POINT> samples;




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


static XY gaussian_blob( XY& center, float radius ){
    return center+XY(gaussrand(),gaussrand())*radius;
}







static void datasource_init(){
    for( int i=0; i<gauss_centers.size(); i++ ){
        gauss_centers[i] = XY(
            lerp( WINW*1/10, WINW*9/10, unirand()),
            lerp( WINH*1/10, WINH*9/10, unirand())
        );
        gauss_rads[i] = lerp( WINH/10, WINH/5, unirand());
    }
}



static POINT datasource_gen(){

    POINT p;

    // gauss blobs or noise
    int source = rand()%(gauss_centers.size()+1);

    // gauss blob
    if( source < gauss_centers.size()){
        p.xy = gaussian_blob( gauss_centers[source], gauss_rads[source] );
        return p;
    }

    // noise
    p.xy = XY(
        lerp( 0, WINW, unirand()),
        lerp( 0, WINH, unirand())
    );
    return p;
}






static int kmeans_classify( XY& sample ){
    // usa le medie attuali per colorare il sample
    float min_dd = 1E6; // +inf
    int color; // un sample è sicuramente di un colore, non serve init
    for( int m=0; m<means.size(); m++ ){
        XY scarto = sample-means[m];
        float dd = scarto*scarto;
//        dd = std::sqrt(dd);
        if( min_dd < dd )continue;
        min_dd = dd;
        color = m;
    }
    return color;
}







static void kmeans(){

    const int max_iters = 100;
    for( int n=0; n<max_iters; n++ ){ 
        std::this_thread::sleep_for(std::chrono::milliseconds( 10 ));
//        fprintf(stderr,"iter %d\n",n);

        auto means_pre = means;

        struct ACCU_N {
            XY accu;
            int n;
        };

        std::vector<ACCU_N> tmp_means;
        tmp_means.resize(means.size());

        // apparentemente .resize non chiama i costruttori... strano
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
            // TODO: scartare i samples la cui dev std è troppo grande
            int color = kmeans_classify(samples[s].xy);
            samples[s].color=color;
            tmp_means[color].n++;
            tmp_means[color].accu = tmp_means[color].accu + samples[s].xy;
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



    // calcolo stdev per ogni punto
    stdev.clear();
    stdev.resize(means.size());

    std::vector<int> count;
    count.resize(means.size());

    for( int s=0; s<samples.size(); s++ ){
        int color = samples[s].color;
        auto scarto = samples[s].xy - means[color];
        stdev[color] += scarto*scarto;
        count[color]++;
    }

    for( int color=0; color<means.size(); color++ ){
        stdev[color] = stdev[color]/count[color];
        fprintf(stderr,"stdev[%d]=%f\n",color,std::sqrt(stdev[color]));
    }

    // ora declassiamo i punti troppo distanti dalla stddev
    for( int s=0; s<samples.size(); s++ ){
        int color = samples[s].color;
        auto scarto = samples[s].xy - means[color];
        if( scarto*scarto > stdev[color]) samples[s].color = -1;
    }



}








static void loop(){
    
    datasource_init();

    while(true){
        // aggiungiamo datapoints
        // i vecchi vengono bruciati

        for( int i=0; i<10; i++ ){
            samples.push_back(datasource_gen());
        }

        // manteniamo la coda ad un numero fisso di samples
        while(samples.size()>300) samples.pop_front();

        // i centroidi rimangono gli stessi della generazione precedente

        std::this_thread::sleep_for(std::chrono::milliseconds( 100 ));

        kmeans();

        std::this_thread::sleep_for(std::chrono::milliseconds( 200 ));
    }
}





static void run(){
    static bool started;
    if(started)return;
    started=true;
    static std::thread helper(loop);
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

    run();

    const int mac_palette[][3]={
        { 255, 255, 255 }, // 0
        { 252, 244, 4   }, // 1
        { 255, 100, 4   }, // 2
        { 220, 8,   8   }, // 3
        { 240, 8,   132 }, // 4
        { 72,  0,   164 }, // 5
        { 0,   0,   212 }, // 6
        { 0,   172, 232 }, // 7
        { 32,  184, 20  }, // 8
        { 0,   100, 16  }, // 9
        { 88,  44,  4   }, // 10
        { 144, 112, 56  }, // 11
        { 192, 192, 192 }, // 12
        { 128, 128, 128 }, // 13
        { 64,  64,  64  }, // 14
        { 0,   0,   0   }, // 15
    };

    for( int s=0; s<samples.size(); s++ ){
        int color = samples[s].color;
        const auto rgb = color<0 ? mac_palette[4] : mac_palette[color];
        SDL_SetRenderDrawColor( renderer, rgb[0],rgb[1],rgb[2], 255 );
        pix( samples[s].xy );
    }

    static float blink;
    blink += dt;

    for( int m=0; m<means.size(); m++ ){
        const auto rgb = fmod(blink,1)>0.5 ? mac_palette[m] : mac_palette[14];
        SDL_SetRenderDrawColor( renderer, rgb[0],rgb[1],rgb[2], 255 );
        pix( means[m], 10 );
    }
}









