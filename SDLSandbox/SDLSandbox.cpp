#include <SDL.h>
#include <stdio.h>
#include <iostream>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

SDL_Window *gWindow = NULL;
SDL_Surface *gScreenSurface = NULL;
const int superPixelResolution = 2;
const int scale = 300;
const int surfaceWidth = superPixelResolution * scale;
const int surfaceHeight = superPixelResolution * scale;


/*
We're using a scaled up pixel structure, so the
image appears to be for example a 2x2 grid of 4 pixels
but it's scaled up so that it's just genuinely 2x2 pixels
(which would be tiny). The x and y passed here are the coordinates
of the conceptual "super" pixels, not the actual pixels of the image
*/
void setPixel(int superPixelX, int superPixelY, uint32_t color, uint32_t* pixels) {
    for (int surfacePixelY=superPixelY*scale; surfacePixelY<superPixelY*scale+scale; ++surfacePixelY) {
        for (int surfacePixelX=superPixelX*scale; surfacePixelX<superPixelX*scale+scale; ++surfacePixelX) {
            int surfacePixelIndex = surfaceWidth * surfacePixelY + surfacePixelX;
            pixels[surfacePixelIndex] = color;
        }
    }
}


int main(int argc, char* args[]) {

    SDL_Init(SDL_INIT_VIDEO);
    // Want to experiment with drawing pixels to SDL surface but it's hard to see
    // whats going on with a larger canvas size, so want to make a 10x10 canvas, but
    // of course 10x10 actual pixels is way too small, so want to implement some logic
    // to make a zoomed in 10x10 canvas.
    // const int superPixelResolution = 2;
    // const int scale = 300;
    // const int surfaceWidth = superPixelResolution * scale;
    // const int surfaceHeight = superPixelResolution * scale;
    SDL_Window* window = SDL_CreateWindow("Sandbox", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, surfaceWidth, surfaceHeight, SDL_WINDOW_SHOWN);
    SDL_Surface* surface = SDL_GetWindowSurface(window);

    uint32_t* pixels = (uint32_t*)surface->pixels;

    std::cout << SDL_GetPixelFormatName(surface->format->format) << std::endl;

    // so each time you draw a "pixel" you'd really draw 6 pixels
    for (int superPixelY=0; superPixelY<superPixelResolution; ++superPixelY) {
        for (int superPixelX=0; superPixelX<superPixelResolution; ++superPixelX) {
            for (int surfacePixelY=superPixelY*scale; surfacePixelY<superPixelY*scale+scale; ++surfacePixelY) {
                for (int surfacePixelX=superPixelX*scale; surfacePixelX<superPixelX*scale+scale; ++surfacePixelX) {
                    int surfacePixelIndex = surfaceWidth * surfacePixelY + surfacePixelX;
                    pixels[surfacePixelIndex] = 0xff0000;
                }
            }
        }
    }


    setPixel(0, 0, 0x00ff00, pixels);
    setPixel(1, 0, 0x0000ff, pixels);
    setPixel(0, 1, 0xff0000, pixels);
    setPixel(1, 1, 0xff00ff, pixels);

    SDL_UpdateWindowSurface(window);

    SDL_Event e;
    bool running = true;
    while (running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) running = false;
        }
    }

    SDL_DestroyWindow(window);
    SDL_Quit();


    return 0;

    
}