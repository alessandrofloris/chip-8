#include <SDL2/SDL.h>

#include "graphic.h"


void initGraphic() {

  //init sdl
  if( SDL_Init(SDL_INIT_VIDEO) != 0) {
    printf("[Error] Could not initialize SDL: %s\n", SDL_GetError());
    exit(EXIT_FAILURE);
  }

  //create window
  SDL_Window* window = SDL_CreateWindow("Chip 8",
  SDL_WINDOWPOS_CENTERED,
  SDL_WINDOWPOS_CENTERED,
  640,
  320,
  0);

  //if window creation failed
  if(window == NULL) {
    printf("[Error] Could not create window: %s\n", SDL_GetError());
    SDL_Quit(); //shutdown subsystems
    exit(EXIT_FAILURE);
  }

  SDL_Delay(5000); //pause execution for 5 seconds

  SDL_DestroyWindow(window); //clears and destroy the window

  SDL_Quit(); //shutdown subsystems
}
