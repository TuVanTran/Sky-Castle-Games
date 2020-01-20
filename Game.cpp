#include <chrono>
#include <iostream>
#include "Game.h"
#include "Classes.h"
#define FPS 60
#define COLS 8
#define SIZE 128
using namespace std;
using namespace chrono;

Game::Game() :m_bStarted(false), m_bRunning(false)
{
	cout << "Constructing engine." << endl;
	m_fps = (Uint32)round((1 / (double)FPS) * 1000); // Sets FPS in milliseconds and rounds.
}

Game* Game::Instance()
{
	static Game* instance = new Game();
	return instance;
}

void Game::Run(const char * title, int xPos, int yPos, int width, int height, int flags)
{
	if (m_bStarted == true)
		return;
	cout << "Starting game." << endl;
	m_bStarted = true;
	if (Init(title, xPos, yPos, width, height, flags) == false)
	{
		cout << "Cannot initialize game." << endl;
		return;
	}
	// Main engine loop here.
	while (Running())
	{
		Wake();
		HandleEvents(); // Not restricted to framerate.
		Update();
		Render();
		if (Running())
			Sleep();
	}
	// End main engine loop.
	Clean();
}

bool Game::Init(const char* title, const int xpos, const int ypos, 
			    const int width, const int height, const int flags)
{
	// Attempt to initialize SDL.
	if (SDL_Init(SDL_INIT_EVERYTHING) == 0) // 0 is error code meaning success.
	{
		cout << "SDL init success!" << endl;
		// Initialize the window
		m_pWindow = SDL_CreateWindow(title, xpos, ypos, width, height, flags);
		if (m_pWindow != nullptr) // Window init success. 
		{
			cout << "Window creation successful!" << endl;
			m_pRenderer = SDL_CreateRenderer(m_pWindow, -1, 0);
			if (m_pRenderer != nullptr) // Renderer init success. 
			{
				//SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "best"); // Antialiases an ugly box around spinny blade. 
				cout << "Renderer creation success!" << endl;
				if (IMG_Init(IMG_INIT_PNG) != 0)
				{
					cout << "Image creation success!" << endl;
					m_pPTexture = IMG_LoadTexture(m_pRenderer, "Img/Player.png");
					m_pBGTexture = IMG_LoadTexture(m_pRenderer, "Img/Backgrounds.png");
					m_pOTexture = IMG_LoadTexture(m_pRenderer, "Img/Obstacles.png");	
				}
				else
				{
					cout << "Image init fail!" << endl;
					return false;// Image init fail.
				}
			}
			else
			{
				cout << "Renderer init fail!" << endl;
				return false; // Renderer init fail. 
			}
		}
		else
		{
			cout << "Window init fail!" << endl;
			return false; // Window init fail. 
		}
	}
	else
	{
		cout << "SDL init fail!" << endl;
		return false; // SDL init fail. 
	}
	
	srand((unsigned)time(NULL));
	m_iKeystates = SDL_GetKeyboardState(nullptr);
	m_iObsCtr = 0;
	m_iObsMax = 2;
	for (int col = 0; col <= COLS; col++) // 9 times.
		m_vObstacles.push_back(new Obstacle(col*SIZE));
	m_pPlayer = new Player({ 0,256,128,128 },
		{ 100,100,100,100 }); // { 100,100,64,64 }); size of Player
	m_bRunning = true;
	return true;
}

bool Game::Running()
{
	return m_bRunning;
}

bool Game::KeyDown(SDL_Scancode c)
{
	if (m_iKeystates != nullptr)
	{
		if (m_iKeystates[c] == 1)
			return true;
		else
			return false;
	}
	return false;
}

void Game::Update()
{
	// Movement of Player

		// Parse player states.
	if (m_pPlayer->m_state == dying)
	{

		m_pPlayer->Animate();
		m_pPlayer->m_iDeathCtr++;
		if (m_pPlayer->m_iDeathCtr == m_pPlayer->m_iDeathCtrMax)
		{
			// With FSM, change to lose state.
			return;
		}
		return;
	}
	if (m_pPlayer->m_state == running)
	{
		if (KeyDown(SDL_SCANCODE_S))
			m_pPlayer->SetAnim(rolling); // state enum is global in Classes.h.
		else if (KeyDown(SDL_SCANCODE_W))
		{
			m_pPlayer->SetAccelY(-GRAV * 2); // Sets the jump force.
			m_pPlayer->SetAnim(jumping);
		}
	}
	else if (m_pPlayer->m_state == rolling)
	{
		if (!KeyDown(SDL_SCANCODE_S))
			m_pPlayer->SetAnim(running);
	}
	// Move player.
	if (KeyDown(SDL_SCANCODE_A))
	{
		m_pPlayer->SetDir(-1);
		m_pPlayer->MoveX();
	}
	else if (KeyDown(SDL_SCANCODE_D))
	{
		m_pPlayer->SetDir(1);
		m_pPlayer->MoveX();
	}
	else
		m_pPlayer->SetAccelX(0.0);
	m_pPlayer->Update();
	m_pPlayer->SetAccelY(0.0); // After jump, reset vertical acceleration.

	// Test for collision.
	for (int i = 0; i <3; i++) // For all the platforms.
	{
		if (SDL_HasIntersection(m_pPlayer->GetDstP(),
			m_Foregrounds[i]->GetDstP()))
		{
			if ((m_pPlayer->GetDstP()->y + m_pPlayer->GetDstP()->h) -
				m_pPlayer->GetVelY() <= m_Foregrounds[i]->GetDstP()->y) // From top.
			{
				if (m_pPlayer->m_state == jumping)
					m_pPlayer->SetAnim(running);
				m_pPlayer->SetVelY(0.0);
				m_pPlayer->SetY(m_Foregrounds[i]->GetDstP()->y -
					m_pPlayer->GetDstP()->h);
			}
			else if (m_pPlayer->GetDstP()->y - m_pPlayer->GetVelY() >=
				m_Foregrounds[i]->GetDstP()->y + m_Foregrounds[i]->GetDstP()->h) // From bottom.
			{
				m_pPlayer->SetVelY(0.0);
				m_pPlayer->SetY(m_Foregrounds[i]->GetDstP()->y +
					m_Foregrounds[i]->GetDstP()->h);
			}
			else if ((m_pPlayer->GetDstP()->x + m_pPlayer->GetDstP()->w) -
				m_pPlayer->GetVelX() <= m_Foregrounds[i]->GetDstP()->x) // From left.
			{
				m_pPlayer->SetVelX(0.0);
				m_pPlayer->SetX(m_Foregrounds[i]->GetDstP()->x -
					m_pPlayer->GetDstP()->w);
			}
			else if (m_pPlayer->GetDstP()->x - m_pPlayer->GetVelX() >=
				m_Foregrounds[i]->GetDstP()->x + m_Foregrounds[i]->GetDstP()->w) // From bottom.
			{
				m_pPlayer->SetVelX(0.0);
				m_pPlayer->SetX(m_Foregrounds[i]->GetDstP()->x +
					m_Foregrounds[i]->GetDstP()->w);
			}
			break;
		}
	}
	for (int i = 0; i < (int)m_vObstacles.size(); i++)
	{
		if (m_vObstacles[i]->GetSprite() != nullptr)
		{
			if (SDL_HasIntersection(m_pPlayer->GetDstP(), m_vObstacles[i]->GetSprite()->GetDstP()))
				//m_pPlayer->m_state = dying;
			{
				m_pPlayer->SetAnim(dying);
				m_bRunning = true;
			}
		}
	}
	// For Obstacle
	for (int i = 0; i < 2; i++)
		m_Backgrounds[i].Update();
	for (int i = 0; i < 5; i++)
		m_Midgrounds[i].Update();
	for (int i = 0; i < 3; i++)
		m_Foregrounds[i]->Update();
	// The next bit shifts the background images back.
	if (m_Backgrounds[0].GetDstP()->x <= -(m_Backgrounds[0].GetDstP()->w))
	{
		for (int i = 0; i < 2; i++)
			m_Backgrounds[i].GetDstP()->x += m_Backgrounds[i].GetDstP()->w;
	}
	if (m_Midgrounds[0].GetDstP()->x <= -(m_Midgrounds[0].GetDstP()->w))
	{
		for (int i = 0; i < 5; i++)
			m_Midgrounds[i].GetDstP()->x += m_Midgrounds[i].GetDstP()->w;
	}
	if (m_Foregrounds[0]->GetDstP()->x <= -(m_Foregrounds[0]->GetDstP()->w))
	{
		for (int i = 0; i < 3; i++)
			m_Foregrounds[i]->GetDstP()->x += m_Foregrounds[i]->GetDstP()->w;
	}
	// Wrap the obstacles.
	if (m_vObstacles.front()->GetX() <= -128)
	{
		// Pop the obstacle.
		delete m_vObstacles.front(); // De-allocated obstacle.
		m_vObstacles.erase(m_vObstacles.begin()); // Remove pointer.
		// Push new obstacle.
		if (m_iObsCtr == 0)
		{
			// Pushing back buzzsaw every time.
			m_vObstacles.push_back(new Obstacle(8 * SIZE, true,
				//{ 128,128,128,128 }, { 8 * SIZE,rand()% 500 ,128,128 }, 0, 1));
				{ 128,128,128,128 }, { 3 * SIZE, 200 ,128,128 }, 0, 1));
		}
		else
			m_vObstacles.push_back(new Obstacle(8 * SIZE)); // Empty obstacle.
		if (m_iObsCtr == m_iObsMax)
			m_iObsCtr = 0;
		else m_iObsCtr++;
	}
	// Scroll the obstacles.
	for (int col = 0; col <= COLS; col++)
		m_vObstacles[col]->Update();

	
}

void Game::HandleEvents()
{
	SDL_Event event;
	if (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_QUIT:
			m_bRunning = false;
			break;
		case SDL_KEYUP:
			if (event.key.keysym.sym == SDLK_a ||
				event.key.keysym.sym == SDLK_d ||
				event.key.keysym.sym == SDLK_LEFT ||
				event.key.keysym.sym == SDLK_RIGHT)
			{
				m_pPlayer->SetAccelX(0.0);
			}
			break;
		}
	}
}

void Game::Wake()
{
	m_start = SDL_GetTicks();
}

void Game::Sleep()
{
	m_end = SDL_GetTicks();
	m_delta = m_end - m_start;
	if (m_delta < m_fps) // Engine has to sleep.
		SDL_Delay(m_fps - m_delta);
}

void Game::Render()
{
	SDL_SetRenderDrawColor(m_pRenderer, 0, 0, 0, 255);
	
	SDL_RenderClear(m_pRenderer); // Clear the screen to the draw color.
	
	// Now render the backgrounds.
	for (int i = 0; i < 2; i++)
		SDL_RenderCopy(m_pRenderer, m_pBGTexture, m_Backgrounds[i].GetSrcP(), m_Backgrounds[i].GetDstP());
	for (int i = 0; i < 5; i++)
		SDL_RenderCopy(m_pRenderer, m_pBGTexture, m_Midgrounds[i].GetSrcP(), m_Midgrounds[i].GetDstP());
	// Render the obstacles.
	for (int col = 0; col <= COLS; col++)
	{
		if (m_vObstacles[col]->GetSprite() != nullptr)
			SDL_RenderCopyEx(Game::Instance()->GetRenderer(), m_pOTexture,
				m_vObstacles[col]->GetSprite()->GetSrcP(),
				m_vObstacles[col]->GetSprite()->GetDstP(),
				m_vObstacles[col]->GetAngle(), NULL, SDL_FLIP_NONE);
	}
	// Render the foreground. For spinny blade.
	for (int i = 0; i < 3; i++)
		SDL_RenderCopy(m_pRenderer, m_pBGTexture, m_Foregrounds[i]->GetSrcP(), m_Foregrounds[i]->GetDstP());
	SDL_RenderCopy(Game::Instance()->GetRenderer(), m_pPTexture,
		m_pPlayer->GetSrcP(), m_pPlayer->GetDstP());
	SDL_RenderPresent(m_pRenderer); // Draw anew.
}

void Game::Clean()
{
	cout << "Cleaning game. Bye!" << endl;
	for (int i = 0; i < (int)m_vObstacles.size(); i++)
	{
		delete m_vObstacles[i];
		m_vObstacles[i] = nullptr;
	}
	m_vObstacles.clear();
	m_vObstacles.shrink_to_fit();
	SDL_DestroyTexture(m_pPTexture);
	SDL_DestroyTexture(m_pBGTexture);
	SDL_DestroyTexture(m_pOTexture);
	SDL_DestroyRenderer(m_pRenderer);
	SDL_DestroyWindow(m_pWindow);
	IMG_Quit();
	SDL_Quit();
}