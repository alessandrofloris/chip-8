#include <SDL2/SDL.h>

#include "graphic.h"


void initGraphic() {

  if( SDL_Init(SDL_INIT_VIDEO) != 0) {
    printf("[Error] Could not initialize SDL: %s\n", SDL_GetError());
    exit(EXIT_FAILURE);
  }

  printf("Initialization succesful!\n");

  SDL_Quit();
}
