/**
    (c) 2012 shane gpl 3 or greater
*/

#define uint unsigned int
#define pu public

#include <cstdlib>
#include <vector>
#include <deque>
#include <string>
#include <SDL.h>
using namespace std;



// this is a message
// we subclass this, others might have arguments
class Msg {
public:
    Msg() {}
    ~Msg() {}

    // pretty print the message
    void pp() {
        puts("empty msg.");
    }
private:
};


// this is an actor
// actors have a queue of messages
// they process those messages and:
//  alter their data,
//  send themselves more messages,
//  send other actors messages
class Actor {
public:
    Actor() {
        msgs=deque<Msg>();
    }
    ~Actor() {
        //is the msgs deque auto deleted?
    }
    bool hasMsgs() {
        if(msgs.size()>0) {
            return true;
        }
        return false;

    }
    // this will need diff args, one for each sub of Msg
    void proc(Msg m) {

        // this will enforce that each msg have a pretty print method
        m.pp();
    }

    void add_msg(Msg m) {
        // add m to the msg queue
        msgs.push_back(m);
    }


    void proc_msgs() {
        while(hasMsgs()) {
            Msg op = msgs.front();
            proc(op);
            msgs.pop_front();
        }
    }
private:
    deque<Msg>msgs;
};

///     ^ base actors
///     v subd actors

// these are the messages that aWin processes
// thats why they have 'aWin_' as a prefix
class aWin_newWin    : pu Msg {};
class aWin_testVid   :pu Msg {};
class aWin_die       :pu Msg {};
class aWin_works     :pu Msg {};

// this is a window object
class aWin  : pu Actor {
public:
    //
    void proc(aWin_newWin n) {
        screen = SDL_SetVideoMode(800, 600, 16,SDL_HWSURFACE|SDL_DOUBLEBUF);
        add_msg(aWin_testVid());
    }
    void proc(aWin_testVid t) {
        bool working = (SDL_Init( SDL_INIT_VIDEO ) < 0 );
        if (working) {
            add_msg(aWin_works());
        } else {
            add_msg(aWin_die());
        }
    }
    void proc(aWin_die d) {
        exit(1);
    }
    void proc(aWin_works w){
        puts("works");
        }
private:
    //
    SDL_Surface* screen;
};

///     ^ actors
/// ////////////////////////////////////////////////////////////////////////////
///     v oops


class Image {
public:
    Image() {}
    ~Image() {}
    void draw() {
        /// TODO use this
    }
private:
    int x,y,w,h;
};

class Window {
public:
    Window() {
        printf("Window cons.\n");
        bitmaps=vector<SDL_Surface*>();
    }
    ~Window() {
        printf("window dtor.");
        // for each bmp
        //SDL_FreeSurface(bmp);
        uint i;
        for(i=0; i<bitmaps.size(); ++i) {
            printf("free bmp # %d ",i);
            SDL_FreeSurface(bitmaps[i]);
        }
    }
    void init() {
        if ( SDL_Init( SDL_INIT_VIDEO ) < 0 ) {
            printf( "Unable to init SDL: %s\n", SDL_GetError() );
            exit(1);
        } else {
            printf("video works\n");
        }
        atexit(SDL_Quit);
    }
    void new_win(int w, int h, int d) {
        SDL_Surface* surf= SDL_SetVideoMode(w, h, d,SDL_HWSURFACE|SDL_DOUBLEBUF);
        if(!surf) {
            printf("Unable to set %d x %x @ %d video: %s\n",w,h,d, SDL_GetError());
            exit(1);
        }
        screen=surf;
        //return surf;

        /////////////////////// goes else where
        dstrect.x = screen->w  / 2;
        dstrect.y = screen->h  / 2;
    }
    void add_bmp(string fn) {
        SDL_Surface* bmp=SDL_LoadBMP(fn.c_str());
        if (!bmp) {
            printf("Unable to load bitmap: %s\n", SDL_GetError());
            exit(1);
        }
        bitmaps.push_back(bmp);
    }
    void render() {
        clear();
        draw();
        flip();
    }
    void clear() {
        SDL_FillRect(screen, 0, SDL_MapRGB(screen->format, 0, 0, 0));
    }
    void draw() {
        /// TODO draw image, where image has ptr to bitmap
        uint i;
        printf("gonna print %d things.\n",bitmaps.size());
        for(i=0; i<bitmaps.size(); ++i) {
            // for each bmp
            // draw the bmp
            SDL_BlitSurface(bitmaps[i], 0, screen, &dstrect);
        }
        //SDL_BlitSurface(bmp, 0, window.screen, &dstrect);
    }
    void flip() {
        SDL_Flip(screen);
    }
private:
    SDL_Surface* screen;
    vector<SDL_Surface*>bitmaps;
    SDL_Rect dstrect;
};






/// MAIN

int main ( int argc, char** argv ) {


    aWin win;
    win.add_msg(aWin_newWin());



    /// the below works, make an actor version


    Window window;

    window.init();
    window.new_win(800,600,8);


    // load an image
    /*
    SDL_Surface* bmp = SDL_LoadBMP("cb.bmp");
    if (!bmp) {
        printf("Unable to load bitmap: %s\n", SDL_GetError());
        return 1;
    }
    */
    window.add_bmp("cb.bmp");

    // centre the bitmap on screen
    //SDL_Rect dstrect;
    //dstrect.x = (window.screen->w - bmp->w) / 2;
    //dstrect.y = (window.screen->h - bmp->h) / 2;

    // program main loop
    bool done = false;
    while (!done) {
        // message processing loop
        /// TODO make evnt oop
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            // check for messages
            switch (event.type) {
                // exit if the window is closed
            case SDL_QUIT:
                done = true;
                break;

                // check for keypresses
            case SDL_KEYDOWN: {
                // exit if ESCAPE is pressed
                if (event.key.keysym.sym == SDLK_ESCAPE)
                    done = true;
                break;
            }
            } // end switch
        } // end of message processing

        // DRAWING STARTS HERE
        window.render();


    } // end main loop



    // all is well ;)
    printf("Exited cleanly\n");
    return 0;
}
