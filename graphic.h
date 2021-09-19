#define WINDOW_HEIGHT 640
#define WINDOW_WIDTH 1280
#define SCREEN_WIDTH 64
#define SCREEN_HEIGHT 32

void initGraphic(SDL_Window**, SDL_Renderer**, SDL_Texture**);
void bufferGraphics(uint8_t screen[SCREEN_HEIGHT][SCREEN_WIDTH], uint32_t*, SDL_Renderer*);
void drawGraphics(uint32_t*, SDL_Renderer*, SDL_Texture*);
void freeGraphic(SDL_Window*, SDL_Renderer*, SDL_Texture*);