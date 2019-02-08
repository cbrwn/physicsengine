/* =================================
 *  Game HUD
 *  Handles drawing all 2D elements over the game itself
 * ================================= */
#pragma once

#include <Renderer2D.h>
#include <Font.h> // forward declares Renderer2D

class GameHud
{
public:
	GameHud();
	~GameHud();

	void update(float delta);
	void draw(aie::Renderer2D* renderer);

	void setWindowSize(float width, float height);

	void showReset(bool e);
	void showGameWon(bool e);
	void showGameOver(bool e);

private:
	aie::Font* m_smallFont;
	aie::Font* m_largeFont;

	float m_windowWidth, m_windowHeight;

	// timer for win text tween
	float m_winTimer;
	// timer for lose text tween
	float m_loseTimer;
	// timer for "space to restart" flash
	float m_flashTimer;

	bool m_showReset;
	bool m_showGameWon;
	bool m_showGameOver;

	// give each section its own function to keep draw() clean
	void drawWin(aie::Renderer2D* renderer);
	void drawLose(aie::Renderer2D* renderer);
	void drawReset(aie::Renderer2D* renderer);
};
