#include <SDL2/SDL.h>

#include "graphic.h"


void initGraphic(SDL_Window* window, SDL_Renderer* renderer, SDL_Texture* texture) {

  //init sdl
  if( SDL_Init(SDL_INIT_VIDEO) != 0) {
    printf("[Error] Could not initialize SDL: %s\n", SDL_GetError());
    exit(EXIT_FAILURE);
  }

  //create window
  window = SDL_CreateWindow("Chip 8",
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
  renderer = SDL_CreateRenderer(window, -1, 0);

  //if renderer creation fails
  if(renderer == NULL) {
    printf("[Error] Could not create renderer: %s\n", SDL_GetError());
    SDL_DestroyWindow(window);
    SDL_Quit(); //shutdown subsystems
    exit(EXIT_FAILURE);
  }

  //create texture
  texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, SCREEN_WIDTH, SCREEN_HEIGHT);

  //if texture creation fails
  if(texture == NULL) {
    printf("[Error] Could not create texture: %s\n", SDL_GetError());
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit(); //shutdown subsystems
    exit(EXIT_FAILURE);
  }

  // Set the screen to black
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0); //Set the color used for drawing operations (Rect, Line and Clear).
  SDL_RenderClear(renderer); //Clear the current rendering target with the drawing color.
  SDL_RenderPresent(renderer); //Update the screen with any rendering performed since the previous call.


}

//close window
void freeGraphic(SDL_Window* window, SDL_Renderer* renderer, SDL_Texture* texture) {

  SDL_DestroyTexture(texture); //clears and destroy the texture

  SDL_DestroyRenderer(renderer); //clears and destroy the renderer

  SDL_DestroyWindow(window); //clears and destroy the window

  SDL_Quit(); //shutdown subsystems
}
