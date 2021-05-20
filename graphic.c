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
  WINDOW_WIDTH,
  WINDOW_HEIGH,
  0);

  //if window creation failed
  if(window == NULL) {
    printf("[Error] Could not create window: %s\n", SDL_GetError());
    SDL_Quit(); //shutdown subsystems
    exit(EXIT_FAILURE);
  }

  //create renderer
  SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);

  //if renderer creation fails
  if(renderer == NULL) {
    printf("[Error] Could not create renderer: %s\n", SDL_GetError());
    SDL_DestroyWindow(window);
    SDL_Quit(); //shutdown subsystems
    exit(EXIT_FAILURE);
  }

  //create texture
  SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, SCREEN_WIDTH, SCREEN_HEIGHT);

  //if texture creation fails
  if(texture == NULL) {
    printf("[Error] Could not create texture: %s\n", SDL_GetError());
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit(); //shutdown subsystems
    exit(EXIT_FAILURE);
  }

  // Set the screen to black 
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
  SDL_RenderClear(renderer);
  SDL_RenderPresent(renderer);

  //monochrome display
  //Chip-8 draws graphics on screen through the use of sprites

  SDL_Delay(5000); //pause execution for 5 seconds

  SDL_DestroyTexture(texture);

  SDL_DestroyRenderer(renderer);

  SDL_DestroyWindow(window); //clears and destroy the window

  SDL_Quit(); //shutdown subsystems
}
