#include <SDL.h>
#include <stdio.h>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

SDL_Window *gWindow = NULL;
SDL_Surface *gScreenSurface = NULL;
SDL_Surface *gMediaSurface = NULL;


bool init() {
    bool success = true;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize. SDL_Error: %s\n", SDL_GetError());
        return false;
    }
    gWindow = SDL_CreateWindow("SDL Sandbox" , SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (gWindow == NULL) {
        printf("Window could not be created. SDL_Error: %s\n", SDL_GetError());
        return false;
    }

    gScreenSurface = SDL_GetWindowSurface(gWindow);

    return true;
}

bool loadMedia() {
    char *imagePath = "image.bmp";
    gMediaSurface = SDL_LoadBMP(imagePath);
    if (gMediaSurface == NULL) {
        printf("Unable to load image %s. SDL_Error: %s\n", imagePath, SDL_GetError());
        return false;
    }

    return true;
}

void close() {
    SDL_FreeSurface(gMediaSurface);
    gMediaSurface = NULL;

    SDL_DestroyWindow(gWindow);
    gWindow = NULL;

    SDL_Quit();
}


int main(int argc, char* args[]) {
    if (!init()) {
        printf("Failed to initialize\n");
        return -1;
    }

    if (!loadMedia()) {
        printf("Failed to load media\n");
        return -1;
    }
    SDL_BlitSurface(gMediaSurface, NULL, gScreenSurface, NULL);
    SDL_UpdateWindowSurface(gWindow);

    SDL_Event e;
    bool quit = false;
    while (quit == false) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
        }
    }

    close();


    return 0;

    
}