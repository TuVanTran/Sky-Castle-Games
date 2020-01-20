#pragma once
#include "SDL.h"


enum state { running, jumping, rolling, dying };

class Sprite
{
protected:
	SDL_Rect m_rSrc; // Rectangle for source pixels.
	SDL_Rect m_rDst; // Rectangle for destination window.

public:
	Sprite(SDL_Rect s, SDL_Rect d) :m_rSrc(s), m_rDst(d) {}
	SDL_Rect* GetSrcP() { return &m_rSrc; }
	SDL_Rect* GetDstP() { return &m_rDst; }

};

class Background : public Sprite
{
private:
	int m_iSpeed; // Scroll speed of background.
public:
	Background(SDL_Rect s, SDL_Rect d, int spd) :Sprite(s, d), m_iSpeed(spd) {}
	void Update() { m_rDst.x -= m_iSpeed; }
};

class Obstacle
{
private:
	Sprite* m_pSprite; // Only used for initialized obstacles.
	bool m_bIsPlatform; // Is it a platform or an instadeath hazard.
	bool m_bRotates; // Does the hazard rotate.
	int m_iX; // X coordinate.
	double m_dAngle = 0;
public:
	Obstacle(int, bool = false, SDL_Rect = { 0,0,0,0 },
		SDL_Rect = { 0,0,0,0 }, bool = false, bool = false);
	~Obstacle();
	int GetX() { return m_iX; }
	bool IsPlatform() { return m_bIsPlatform; } // Make sure to set ground object to true.
	Sprite* GetSprite() { return m_pSprite; }
	void Update();
	bool Rotates() { return m_bRotates; }
	double GetAngle() { return m_dAngle; }
};
 // Start to add
class AnimatedSprite : public Sprite
{
protected:
	state m_state;
	int m_iSprite = 0,	// Which sprite to display for animation.
		m_iSpriteMin,
		m_iSpriteMax,	// How many sprites total.
		m_iFrame = 0,	// Frame counter.
		m_iFrameMax,	// Number of frames per sprite.
		m_iDeathCtr = 0,
		m_iDeathCtrMax;
	int m_iAnimData[4][5] = { {6,0,8,515,0},     // Running data.
							  {1,8,9,515,0},     // Jumping data.
							  {1,0,3,640,0},     // Rolling data.
							  {12,4,9,640,60} }; // Dying data.;

public:
	AnimatedSprite(SDL_Rect, SDL_Rect); // Constructor.
	virtual void Update() = 0;
	void Animate();
	void SetAnim(state s);
};

class Player : public AnimatedSprite
{
private:
	bool m_bGrounded;
	double m_dAccelX, m_dAccelY,
		m_dMaxAccelX,
		m_dVelX, m_dVelY,
		m_dMaxVelX, m_dMaxVelY,
		m_dDrag, m_dGrav;
	int m_iDir;
public:
	Player(SDL_Rect, SDL_Rect);
	void SetDir(int d) { m_iDir = d; }
	void SetAccelX(double a) { m_dAccelX = a; }
	void SetAccelY(double a) { m_dAccelY = a; }
	void SetGravity(double g) { m_dGrav = g; }
	bool IsGrounded() { return m_bGrounded && (m_dVelY == 0); }
	void SetGrounded(bool g) { m_bGrounded = g; }
	void SetVelX(double v) { m_dVelX = v; }
	void SetVelY(double v) { m_dVelY = v; }
	double GetVelX() { return m_dVelX; }
	double GetVelY() { return m_dVelY; }
	void SetX(int x) { m_rDst.x = x; }
	void SetY(int y) { m_rDst.y = y; }
	void Stop()
	{
		SetVelX(0.0);
		SetVelY(0.0);
	}
	void MoveX();
	void Update();
	friend class Game;
};