#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <cmath>
#define WIDTH 1024
#define HEIGHT 768
#define FPS 60
using namespace std;

// Global engine variables.
bool g_bRunning = false; // Loop control flag.
int g_iSpeed = 0; // Speed of box.
const Uint8* g_iKeystates; // Keyboard state container.
Uint32 g_start, g_end, g_delta, g_fps; // Fixed timestep variables.
SDL_Window* g_pWindow; // This represents the SDL window.
SDL_Renderer* g_pRenderer; // This represents the buffer to draw to.

// Player variables
SDL_Texture* g_PTexture;
SDL_Rect g_Psrc, g_Pdst;
int g_frame = 0, g_FrameMax = 30, g_sprite = 0, g_spriteMax = 8, g_direction = 0;
double g_dSpeed = 0;

// Background
SDL_Texture* g_backTexture;
SDL_Rect g_Bsrc, g_Bdst;

// Ground
SDL_Texture* g_pGTexture;
SDL_Rect g_gSrc, g_gDst;
double g_dMoved;


bool init(const char* title, int xpos, int ypos, int width, int height, int flags)
{
	cout << "Initializing game." << endl;
	// Attempt to initialize SDL.
	if (SDL_Init(SDL_INIT_EVERYTHING) == 0)
	{
		// Create the window.
		g_pWindow = SDL_CreateWindow(title, xpos, ypos, width, height, flags);
		if (g_pWindow != nullptr) // Window init success.
		{
			g_pRenderer = SDL_CreateRenderer(g_pWindow, -1, 0);
			if (g_pRenderer != nullptr) // Renderer init success.
			{
				if (IMG_Init(IMG_INIT_PNG))
				{
					g_PTexture = IMG_LoadTexture(g_pRenderer, "PlayerRun.png");
					g_backTexture = IMG_LoadTexture(g_pRenderer, "Background.png");
					g_pGTexture = IMG_LoadTexture(g_pRenderer, "Ground.png");
				}
			}
			else return false; // Renderer init fail.
		}
		else return false; // Window init fail.
	}
	else return false; // SDL init fail.
	g_fps = (Uint32)round((1 / (double)FPS) * 1000); // Sets FPS in milliseconds and rounds.
	g_iKeystates = SDL_GetKeyboardState(nullptr);
	// Player
	g_Psrc = { 0, 0, 128, 128 };
	g_Pdst = { 100, height - 240 - g_Psrc.h, g_Psrc.w, g_Psrc.h };
	// Background
	g_Bsrc = { 0, 0, 8192 , 1024 };
	g_Bdst = { 0, 0, width, height - 200 };
	// Ground
	g_gSrc = { 0, 0, 1612, 277 };
	g_gDst = { 0, height - g_gSrc.h, g_gSrc.w, g_gSrc.h };
	g_bRunning = true; // Everything is okay, start the engine.
	cout << "Success!" << endl;
	cout << "Use 'A' to move left, and 'D' to move right\n";
	return true;
}

void wake()
{
	g_start = SDL_GetTicks();
}

void sleep()
{ 
	g_end = SDL_GetTicks();
	g_delta = g_end - g_start;
	if (g_delta < g_fps) // Engine has to sleep.
		SDL_Delay(g_fps - g_delta);
}

void handleEvents()
{
	SDL_Event event;

	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_QUIT: // User pressed window's 'x' button.
			g_bRunning = false;
			break;
		case SDL_KEYDOWN: // Try SDL_KEYUP instead.
			if (event.key.keysym.sym == SDLK_ESCAPE) 
				g_bRunning = false;
			break;
		}
	}
}

// Keyboard utility function.
bool keyDown(SDL_Scancode c)
{
	if (g_iKeystates != nullptr)
	{
		if (g_iKeystates[c] == 1)
			return true;
		else
			return false;
	}
	return false;
}

void animate()
{
	if (g_frame >= g_FrameMax)
	{
		g_frame = 0;
		g_sprite++;
		if (g_sprite == g_spriteMax)
			g_sprite = 0;
		g_Psrc.x = g_Psrc.w * g_sprite;
	}
	//g_frame++;
}

void update()
{
	// Player movement
	if (keyDown(SDL_SCANCODE_A) && g_Pdst.x > 0)
	{
		if (g_dSpeed > -30)
			g_dSpeed -= 0.5;
	}
	if (keyDown(SDL_SCANCODE_D))
	{
		if (g_dSpeed < 30)
			g_dSpeed += 0.5;
	}
	// Slow down when stopped moving
	if (g_dSpeed >= 0.2)
	{
		g_direction = 0;
		g_dSpeed -= 0.2;
	}
	else if (g_dSpeed <= -0.2)
	{
		g_direction = 1;
		g_dSpeed += 0.2;
	}
	else
	{
		g_dSpeed = 0;
	}
	// Boundaries
	if (g_Pdst.x < 0)
	{
		g_Pdst.x = 0;
		g_dSpeed = 0;
	}
	if (g_Pdst.x >= 2 * WIDTH / 3 - g_Pdst.w && g_dSpeed > 0)
	{
		g_Pdst.x = 2 * WIDTH / 3 - g_Pdst.w;
		g_dMoved += g_dSpeed;
	}
	else
	{
		g_iSpeed = g_dSpeed;
		g_Pdst.x += g_iSpeed;
	}
	g_frame += abs(g_dSpeed);
	animate();
}

void render()
{
	// Background
	g_Bdst.x = 0 - (int)(g_dMoved/5) % g_Bdst.w;
	SDL_RenderCopy(g_pRenderer, g_backTexture, &g_Bsrc, &g_Bdst);
	g_Bdst.x = g_Bdst.w - (int)(g_dMoved/5) % g_Bdst.w;
	SDL_RenderCopy(g_pRenderer, g_backTexture, &g_Bsrc, &g_Bdst);
	// Ground
	g_gDst.x = 0 - (int)(g_dMoved) % g_gDst.w;
	SDL_RenderCopy(g_pRenderer, g_pGTexture, &g_gSrc, &g_gDst);
	g_gDst.x = g_gDst.w - (int)(g_dMoved) % g_gDst.w;
	SDL_RenderCopy(g_pRenderer, g_pGTexture, &g_gSrc, &g_gDst);
	// Render stuff.
	if(g_direction == 0)
		SDL_RenderCopyEx(g_pRenderer, g_PTexture, &g_Psrc, &g_Pdst, 0, nullptr, SDL_FLIP_NONE);
	else if(g_direction == 1)
		SDL_RenderCopyEx(g_pRenderer, g_PTexture, &g_Psrc, &g_Pdst, 0, nullptr, SDL_FLIP_HORIZONTAL);
	// Draw anew.
	SDL_RenderPresent(g_pRenderer); 
}

void clean()
{
	cout << "Cleaning game." << endl;
	SDL_DestroyTexture(g_PTexture);
	SDL_DestroyTexture(g_backTexture);
	SDL_DestroyTexture(g_pGTexture);
	SDL_DestroyRenderer(g_pRenderer);
	SDL_DestroyWindow(g_pWindow);
	SDL_Quit();
}

// Main function.
int main(int argc, char* args[]) // Main MUST have these parameters for SDL.
{
	if (init("A - Move left, D - Move right", SDL_WINDOWPOS_CENTERED, 
		SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, 0) == false)
		return 1;
	while (g_bRunning)
	{
		wake();
		handleEvents();
		update();
		render();
		if (g_bRunning)
			sleep();
	}
	clean();
	return 0;
}