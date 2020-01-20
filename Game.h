#pragma once
#include <vector>
#include "Classes.h"
#include "SDL.h"
#include "SDL_image.h"
#define GRAV 12.0

using namespace std;

class Game
{
private:
	bool m_bStarted, m_bRunning;
	bool m_bGotTick;
	SDL_Window* m_pWindow;
	SDL_Renderer* m_pRenderer;
	SDL_Texture* m_pBGTexture;
	SDL_Texture* m_pOTexture;
	SDL_Texture* m_pPTexture;
	const Uint8* m_iKeystates;
	Uint32 m_start, m_end, m_delta, m_fps;

	Player* m_pPlayer;

	// Create (number visible +1) objects for each background.
	Background m_Backgrounds[2] = { Background({0,0,1024,768},{0,0,1024,768},1),
									Background({0,0,1024,768},{1024,0,1024,768},1) };
	Background m_Midgrounds[5] = { Background({1024,0,256,512},{0,0,256,512},3),
								   Background({1024,0,256,512},{256,0,256,512},3),
								   Background({1024,0,256,512},{512,0,256,512},3),
								   Background({1024,0,256,512},{768,0,256,512},3),
								   Background({1024,0,256,512},{1024,0,256,512},3) };
	Background * m_Foregrounds[3] = { new Background({1024,512,512,256},{0,512,512,256},4),
									 new Background({1024,512,512,256},{512,512,512,256},4),
									 new Background({1024,512,512,256},{1024,512,512,256},4) };
	// Members for obstacles.
	vector<Obstacle*> m_vObstacles;
	int m_iObsCtr, m_iObsMax;

	Game();
	bool Init(const char* title, const int xpos, const int ypos,
		const int width, const int height, const int flags);
	bool Running();
	void Update();
	void HandleEvents();
	void Wake();
	void Sleep();
	void Render();
	void Clean();

public:
	static Game* Instance();
	void Run(const char*, int, int, int, int, int);
	SDL_Window* GetWindow() { return m_pWindow; }
	SDL_Renderer* GetRenderer() { return m_pRenderer; }
	bool KeyDown(SDL_Scancode c);
};