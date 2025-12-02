#include "Game.h"
#include <iostream>
#include <cmath>

// Our target FPS
static double const FPS{ 60.0f };

////////////////////////////////////////////////////////////
Game::Game()
    : m_window(sf::VideoMode(sf::Vector2u(ScreenSize::s_width, ScreenSize::s_height), 32), "SFML Playground", sf::Style::Default)
#ifdef TEST_FPS
    , x_updateFPS(m_arialFont)
    , x_drawFPS(m_arialFont)
#endif
    , m_leftScoreText(m_arialFont)
    , m_rightScoreText(m_arialFont)
    , m_overlayText(m_arialFont)
{
    init();
}

////////////////////////////////////////////////////////////
void Game::init()
{
	// Really only necessary is our target FPS is greater than 60.
	m_window.setVerticalSyncEnabled(true);
    
	if (!m_arialFont.openFromFile("ASSETS/FONTS/ariblk.ttf"))
	{
		std::cout << "Error loading font file";
	}

	// Initialize paddles
	m_leftPaddle.setSize(sf::Vector2f(20.f, 120.f));
	m_leftPaddle.setFillColor(sf::Color::White);
	m_leftPaddle.setPosition(sf::Vector2f(50.f, (float)ScreenSize::s_height / 2.f - m_leftPaddle.getSize().y / 2.f));

	m_rightPaddle.setSize(sf::Vector2f(20.f, 120.f));
	m_rightPaddle.setFillColor(sf::Color::White);
	m_rightPaddle.setPosition(sf::Vector2f((float)ScreenSize::s_width - 50.f - m_rightPaddle.getSize().x, (float)ScreenSize::s_height / 2.f - m_rightPaddle.getSize().y / 2.f));

	// Initialize ball
	m_ball.setRadius(10.f);
	m_ball.setFillColor(sf::Color::White);
	m_ball.setPosition(sf::Vector2f((float)ScreenSize::s_width / 2.f - m_ball.getRadius(), (float)ScreenSize::s_height / 2.f - m_ball.getRadius()));

	// Center line
	m_centerLine.setSize(sf::Vector2f(4.f, (float)ScreenSize::s_height));
	m_centerLine.setFillColor(sf::Color(80, 80, 80));
	m_centerLine.setPosition(sf::Vector2f((float)ScreenSize::s_width / 2.f - 2.f, 0.f));

	// Score texts
	m_leftScoreText.setFont(m_arialFont);
	m_leftScoreText.setCharacterSize(48);
	m_leftScoreText.setFillColor(sf::Color::White);
	m_leftScoreText.setPosition(sf::Vector2f((float)ScreenSize::s_width * 0.25f - 20.f, 20.f));
	m_leftScoreText.setString(std::to_string(m_leftScore));

	m_rightScoreText.setFont(m_arialFont);
	m_rightScoreText.setCharacterSize(48);
	m_rightScoreText.setFillColor(sf::Color::White);
	m_rightScoreText.setPosition(sf::Vector2f((float)ScreenSize::s_width * 0.75f - 20.f, 20.f));
	m_rightScoreText.setString(std::to_string(m_rightScore));

	// Overlay
	m_overlayRect.setSize(sf::Vector2f((float)ScreenSize::s_width, (float)ScreenSize::s_height));
	m_overlayRect.setFillColor(sf::Color(0, 0, 0, 150)); // semi-transparent black
	m_overlayText.setFont(m_arialFont);
	m_overlayText.setCharacterSize(64);
	m_overlayText.setFillColor(sf::Color::White);
	// position will be set when overlay text is created so it can be centered

#ifdef TEST_FPS
	x_updateFPS.setFont(m_arialFont);
	x_updateFPS.setPosition(sf::Vector2f(20, 300));
	x_updateFPS.setCharacterSize(24);
	x_updateFPS.setFillColor(sf::Color::White);
	x_drawFPS.setFont(m_arialFont);
	x_drawFPS.setPosition(sf::Vector2f(20, 350));
	x_drawFPS.setCharacterSize(24);
	x_drawFPS.setFillColor(sf::Color::White);
#endif

	// Ensure initial state
	resetGame();
}

////////////////////////////////////////////////////////////
void Game::resetGame()
{
	m_leftScore = 0;
	m_rightScore = 0;
	m_leftScoreText.setString(std::to_string(m_leftScore));
	m_rightScoreText.setString(std::to_string(m_rightScore));
	m_gameOver = false;
	// reset positions
	m_leftPaddle.setPosition(sf::Vector2f(50.f, (float)ScreenSize::s_height / 2.f - m_leftPaddle.getSize().y / 2.f));
	m_rightPaddle.setPosition(sf::Vector2f((float)ScreenSize::s_width - 50.f - m_rightPaddle.getSize().x, (float)ScreenSize::s_height / 2.f - m_rightPaddle.getSize().y / 2.f));
	m_ball.setPosition(sf::Vector2f((float)ScreenSize::s_width / 2.f - m_ball.getRadius(), (float)ScreenSize::s_height / 2.f - m_ball.getRadius()));
	m_ballVelocity = sf::Vector2f(-400.f, -250.f);
}

////////////////////////////////////////////////////////////
void Game::run()
{
	sf::Clock clock;
	sf::Time timeSinceLastUpdate = sf::Time::Zero;
	
	sf::Time timePerFrame = sf::seconds(1.0f / FPS); // 60 fps
	while (m_window.isOpen())
	{
		processEvents(); // as many as possible
		timeSinceLastUpdate += clock.restart();
		while (timeSinceLastUpdate > timePerFrame)
		{
			timeSinceLastUpdate -= timePerFrame;
			processEvents(); // at least 60 fps
			update(timePerFrame.asMilliseconds()); //60 fps
#ifdef TEST_FPS
		x_secondTime += timePerFrame;
		x_updateFrameCount++;
		if (x_secondTime.asSeconds() > 1)
		{
			std::string updatesPS = "UPS " + std::to_string(x_updateFrameCount - 1);
			x_updateFPS.setString(updatesPS);
			std::string drawsPS = "DPS " + std::to_string(x_drawFrameCount);
			x_drawFPS.setString(drawsPS);
			x_updateFrameCount = 0;
			x_drawFrameCount = 0;
			x_secondTime = sf::Time::Zero;
		}
#endif
		}
		render(); // as many as possible
#ifdef TEST_FPS
		x_drawFrameCount++;
#endif
	}
}

////////////////////////////////////////////////////////////
void Game::processEvents()
{
    while (const std::optional event = m_window.pollEvent())
    {
        if (event->is<sf::Event::Closed>())
        {
            m_window.close();
        }
        processGameEvents(*event);
    }
}


////////////////////////////////////////////////////////////
void Game::processGameEvents(const sf::Event& event)
{
	if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>())
	{
		switch (keyPressed->scancode)
		{
		case sf::Keyboard::Scancode::Escape:
			m_window.close();
			break;
		case sf::Keyboard::Scancode::Up:
			// Up key was pressed...
			break;
		default:
			break;
		}
	}
}


////////////////////////////////////////////////////////////
void Game::update(double dt)
{
	// dt arrives in milliseconds in this project; convert to seconds
	float floatSeconds = static_cast<float>(dt) / 1000.f;

	// If game over listen for space to restart
	if (m_gameOver)
	{
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space))
		{
			resetGame();
		}
		return;
	}

	{
	// Player input - left paddle: W/S, right paddle: Up/Down
	if (!m_isNetworkedGame) {
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W))
		{
			m_leftPaddle.move(sf::Vector2f(0.f, -m_paddleSpeed * floatSeconds));
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))
		{
			m_leftPaddle.move(sf::Vector2f(0.f, m_paddleSpeed * floatSeconds));
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up))
		{
			m_rightPaddle.move(sf::Vector2f(0.f, -m_paddleSpeed * floatSeconds));
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down))
		{
			m_rightPaddle.move(sf::Vector2f(0.f, m_paddleSpeed * floatSeconds));
		}
	}

	// Keep paddles inside the screen

	auto clampPaddle = [&](sf::RectangleShape& paddle)
		{
			if (paddle.getPosition().y < 0.f)
				paddle.setPosition(sf::Vector2f(paddle.getPosition().x, 0.f));
			if (paddle.getPosition().y + paddle.getSize().y > (float)ScreenSize::s_height)
				paddle.setPosition(sf::Vector2f(paddle.getPosition().x, (float)ScreenSize::s_height - paddle.getSize().y));
		};

	clampPaddle(m_leftPaddle);
	clampPaddle(m_rightPaddle);
	}

	// Move ball
	m_ball.move(m_ballVelocity * floatSeconds);

	// Ball collision with top/bottom
	if (m_ball.getPosition().y <= 0.f)
	{
		m_ball.setPosition(sf::Vector2f(m_ball.getPosition().x, 0.f));
		m_ballVelocity.y = -m_ballVelocity.y;
	}
	if (m_ball.getPosition().y + m_ball.getRadius() * 2.f >= (float)ScreenSize::s_height)
	{
		m_ball.setPosition(sf::Vector2f(m_ball.getPosition().x, (float)ScreenSize::s_height - m_ball.getRadius() * 2.f));
		m_ballVelocity.y = -m_ballVelocity.y;
	}

	// Ball collision with paddles (AABB using positions and sizes)
	{
	
	auto ballPos = m_ball.getPosition();
	auto ballSize = sf::Vector2f(m_ball.getRadius() * 2.f, m_ball.getRadius() * 2.f);

	auto lp = m_leftPaddle.getPosition();
	auto lsize = m_leftPaddle.getSize();

	bool intersectsLeft = !(ballPos.x + ballSize.x < lp.x || ballPos.x > lp.x + lsize.x ||
				ballPos.y + ballSize.y < lp.y || ballPos.y > lp.y + lsize.y);

	if (intersectsLeft)
	{
		m_ball.setPosition(sf::Vector2f(lp.x + lsize.x + 0.1f, ballPos.y));
		m_ballVelocity.x = std::abs(m_ballVelocity.x);
	}

	auto rp = m_rightPaddle.getPosition();
	auto rsize = m_rightPaddle.getSize();

	bool intersectsRight = !(ballPos.x + ballSize.x < rp.x || ballPos.x > rp.x + rsize.x ||
				ballPos.y + ballSize.y < rp.y || ballPos.y > rp.y + rsize.y);

	if (intersectsRight)
	{
		m_ball.setPosition(sf::Vector2f(rp.x - ballSize.x - 0.1f, ballPos.y));
		m_ballVelocity.x = -std::abs(m_ballVelocity.x);
	}
	}

	// Ball out of bounds - simple reset
	if (m_ball.getPosition().x < -50.f)
	{
		// right player scores
		m_rightScore++;
		m_rightScoreText.setString(std::to_string(m_rightScore));
		m_ball.setPosition(sf::Vector2f((float)ScreenSize::s_width / 2.f - m_ball.getRadius(), (float)ScreenSize::s_height / 2.f - m_ball.getRadius()));
		m_ballVelocity = sf::Vector2f(-400.f, -250.f);
	}
	else if (m_ball.getPosition().x > (float)ScreenSize::s_width + 50.f)
	{
		// left player scores
		m_leftScore++;
		m_leftScoreText.setString(std::to_string(m_leftScore));
		m_ball.setPosition(sf::Vector2f((float)ScreenSize::s_width / 2.f - m_ball.getRadius(), (float)ScreenSize::s_height / 2.f - m_ball.getRadius()));
		m_ballVelocity = sf::Vector2f(400.f, 250.f);
	}

	// Check win conditions
	if (m_leftScore >= m_winScore)
	{
		m_gameOver = true;
		m_overlayText.setString("Player 1\nWins!\nPress Space to\nRestart");
		auto bounds = m_overlayText.getLocalBounds();
		sf::Vector2f origin(bounds.position.x + bounds.size.x / 2.f, bounds.position.y + bounds.size.y / 2.f);
		m_overlayText.setOrigin(origin);
		m_overlayText.setPosition(sf::Vector2f((float)ScreenSize::s_width / 2.f, (float)ScreenSize::s_height / 2.f));
	}
	else if (m_rightScore >= m_winScore)
	{
		m_gameOver = true;
		m_overlayText.setString("Player 2\nWins!\nPress Space to\nRestart");
		auto bounds = m_overlayText.getLocalBounds();
		sf::Vector2f origin(bounds.position.x + bounds.size.x / 2.f, bounds.position.y + bounds.size.y / 2.f);
		m_overlayText.setOrigin(origin);
		m_overlayText.setPosition(sf::Vector2f((float)ScreenSize::s_width / 2.f, (float)ScreenSize::s_height / 2.f));
	}
}

////////////////////////////////////////////////////////////
void Game::render()
{
	m_window.clear(sf::Color(0, 0, 0, 0));
#ifdef TEST_FPS
	m_window.draw(x_updateFPS); //ups is 60 and dps is 61
	m_window.draw(x_drawFPS);
#endif
	// draw center line, paddles, ball and scores
	m_window.draw(m_centerLine);
	m_window.draw(m_leftPaddle);
	m_window.draw(m_rightPaddle);
	m_window.draw(m_ball);
	m_window.draw(m_leftScoreText);
	m_window.draw(m_rightScoreText);

	if (m_gameOver)
	{
		m_window.draw(m_overlayRect);
		m_window.draw(m_overlayText);
	}

	m_window.display();
}
