/* =================================
 *  Game HUD
 *  Handles drawing all 2D elements over the game itself
 * ================================= */
#include "gamehud.h"

#include <gmath.h>
#include <Renderer2D.h>

GameHud::GameHud()
{
	m_smallFont = new aie::Font("./font/roboto.ttf", 32);
	m_largeFont = new aie::Font("./font/roboto.ttf", 128);

	m_windowWidth = 0.0f;
	m_windowHeight = 0.0f;

	m_showReset = false;
	m_showGameWon = false;
	m_showGameOver = false;

	m_winTimer = 0.0f;
	m_loseTimer = 0.0f;
	m_flashTimer = 0.0f;
}

GameHud::~GameHud()
{
	delete m_smallFont;
	delete m_largeFont;
}

void GameHud::update(float delta)
{
	if (m_showGameWon)
		m_winTimer += delta;
	if (m_showGameOver)
		m_loseTimer += delta;

	if (m_showReset)
		m_flashTimer += delta;
}

void GameHud::draw(aie::Renderer2D* renderer)
{
	if (m_showGameOver)
		drawLose(renderer);
	if (m_showGameWon)
		drawWin(renderer);

	if (m_showReset)
		drawReset(renderer);
}

void GameHud::drawWin(aie::Renderer2D* renderer)
{
	const char* message = "you won :)";
	float sWidth = m_largeFont->getStringWidth(message);

	// stick the text in the middle of the screen
	float sPosX = (m_windowWidth / 2.0f) - (sWidth / 2.0f);
	// text should end up in the middle
	float destY = (m_windowHeight / 2.0f);
	// after starting below the screen
	float startY = -200.0f;

	// cap the timer at 1 to keep the easing from continuing
	float tmr = m_winTimer;
	if (tmr > 1.0f)
		tmr = 1.0f;

	float sPosY = easeOutElastic(startY, destY, tmr);

	renderer->setRenderColour(0, 1, 0);
	renderer->drawText(m_largeFont, message, sPosX, sPosY);
}

void GameHud::drawLose(aie::Renderer2D* renderer)
{
	const char* message = "you lost :(";
	float sWidth = m_largeFont->getStringWidth(message);

	// stick the text in the middle of the screen
	float sPosX = (m_windowWidth / 2.0f) - (sWidth / 2.0f);
	// text should end up in the middle
	float destY = (m_windowHeight / 2.0f);
	// after starting above the screen
	float startY = m_windowHeight * 1.2f;

	// cap the timer at 1 to keep the easing from continuing
	float tmr = m_loseTimer;
	if (tmr > 1.0f)
		tmr = 1.0f;

	float sPosY = easeOutElastic(startY, destY, tmr);

	renderer->setRenderColour(1, 0, 0);
	renderer->drawText(m_largeFont, message, sPosX, sPosY);
}

void GameHud::drawReset(aie::Renderer2D* renderer)
{
	// base the flashing on the timer
	int flash = (int)(m_flashTimer * 7.0f) % 10;
	// display when it's under 8 - makes it flash briefly and show for longer
	if (flash < 8)
	{
		const char* message = "press space to restart";
		float sWidth = m_smallFont->getStringWidth(message);

		float sPosX = (m_windowWidth / 2.0f) - (sWidth / 2.0f);
		renderer->setRenderColour(1, 1, 1);
		renderer->drawText(m_smallFont, message, sPosX, 32.0f);
	}
}

void GameHud::setWindowSize(float width, float height)
{
	m_windowWidth = width;
	m_windowHeight = height;
}

void GameHud::showGameOver(bool e)
{
	// we don't want to reset the timer if the text is already showing
	if (m_showGameOver == e)
		return;
	// don't show both elements at once
	if (m_showGameWon)
		return;
	m_showGameOver = e;
	m_loseTimer = 0.0f;
}

void GameHud::showGameWon(bool e)
{
	// we don't want to reset the timer if the text is already showing
	if (m_showGameWon == e)
		return;
	// don't show both elements at once
	if (m_showGameOver)
		return;
	m_showGameWon = e;
	m_winTimer = 0.0f;
}

void GameHud::showReset(bool e)
{
	// don't reset the timer if the text is already flashing
	if (m_showReset == e)
		return;
	m_showReset = e;
	m_flashTimer = 0.0f;
}
