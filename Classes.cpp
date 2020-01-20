#include "Classes.h"
#include "Game.h"
#include <algorithm>
#include <iostream>

Obstacle::Obstacle(int x, bool b, SDL_Rect src,
	SDL_Rect dst, bool p, bool r)
{
	m_iX = x;
	if (b) // Construct the Sprite
	{
		m_pSprite = new Sprite(src, dst);
		m_bIsPlatform = p;
		m_bRotates = r;
	}
}

Obstacle::~Obstacle()
{
	if (m_pSprite != nullptr)
	{
		delete m_pSprite;
		m_pSprite = nullptr;
	}
}

void Obstacle::Update()
{
	m_iX -= 3;
	if (m_pSprite != nullptr)
	{
		m_pSprite->GetDstP()->x = m_iX; // -= 3;
		if (m_bRotates)
			m_dAngle += 5;
	}
}

AnimatedSprite::AnimatedSprite(SDL_Rect s, SDL_Rect d) : Sprite(s, d) {}

void AnimatedSprite::Animate()
{
	m_iFrame++;
	if (m_iFrame == m_iFrameMax)
	{
		m_iFrame = 0;
		m_iSprite++;
		if (m_iSprite == m_iSpriteMax)
		{
			m_iSprite = m_iSpriteMin;
		}
	}
	m_rSrc.x = m_rSrc.w * m_iSprite;
}

void AnimatedSprite::SetAnim(state s)
{
	m_state = s;
	m_iFrame = 0;
	m_iFrameMax = m_iAnimData[m_state][0];
	m_iSprite = m_iSpriteMin = m_iAnimData[m_state][1];
	m_iSpriteMax = m_iAnimData[m_state][2];
	m_rSrc.y = m_iAnimData[m_state][3];
	m_iDeathCtrMax = m_iAnimData[m_state][4];
}

Player::Player(SDL_Rect s, SDL_Rect d) :AnimatedSprite(s, d)
{
	m_bGrounded = false;
	m_dAccelX = m_dAccelY = m_dVelX = m_dVelY = 0.0;
	m_dGrav = (double)GRAV;
	m_dMaxAccelX = 2.0;
	m_dMaxVelX = 3.0;
	m_dMaxVelY = m_dGrav;
	m_dDrag = 0.925;
	m_iDir = 1;
	SetAnim(running);
}

void Player::MoveX()
{
	m_dAccelX += m_iDir * 0.1;
}

void Player::Update()
{
	// X axis stuff.
	m_dAccelX = min(max(m_dAccelX, -m_dMaxAccelX), m_dMaxAccelX);
	m_dVelX = (m_dVelX + m_dAccelX) * m_dDrag;
	m_dVelX = min(max(m_dVelX, -m_dMaxVelX), m_dMaxVelX);
	m_rDst.x += (int)m_dVelX;

	// Y axis/jump stuff.
	m_dVelY = (m_dVelY + m_dAccelY) + m_dGrav / 10; // Applies downward force.
	m_dVelY = min(max(m_dVelY, -(m_dMaxVelY * 10)), (m_dMaxVelY));

	if (fabs(m_dVelY) > m_dGrav / 4) // So we go into jump when we move off a platform.
		SetAnim(jumping);

	m_rDst.y += (int)m_dVelY;
	Animate();
}
