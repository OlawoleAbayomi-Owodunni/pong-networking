#include "Game.h"
#include <iostream>
#include <cmath>

// Our target FPS
static double const FPS{ 60.0f };

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

void Game::init()
{
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
	m_overlayRect.setFillColor(sf::Color(0, 0, 0, 150));
	m_overlayText.setFont(m_arialFont);
	m_overlayText.setCharacterSize(64);
	m_overlayText.setFillColor(sf::Color::White);

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

	// Main menu rectangles (Play, Online Mode, Exit)
	m_menuOption1.setSize(sf::Vector2f(300.f, 100.f));
	m_menuOption2.setSize(sf::Vector2f(300.f, 100.f));
	m_menuOption3.setSize(sf::Vector2f(300.f, 100.f));
	m_menuOption1.setFillColor(sf::Color(100, 100, 220));
	m_menuOption2.setFillColor(sf::Color(100, 220, 100));
	m_menuOption3.setFillColor(sf::Color(220, 100, 100));
	float spacing = 20.f;
	float totalHeight = m_menuOption1.getSize().y * 3.f + spacing * 2.f;
	float centerX = ScreenSize::s_width / 2.f - m_menuOption1.getSize().x / 2.f;
	float centerY = ScreenSize::s_height / 2.f - totalHeight / 2.f;
	m_menuOption1.setPosition(sf::Vector2f(centerX, centerY));
	m_menuOption2.setPosition(sf::Vector2f(centerX, centerY + m_menuOption1.getSize().y + spacing));
	m_menuOption3.setPosition(sf::Vector2f(centerX, centerY + (m_menuOption1.getSize().y + spacing) * 2.f));

	// Menu texts
	m_menuText1.setFont(m_arialFont);
	m_menuText2.setFont(m_arialFont);
	m_menuText3.setFont(m_arialFont);
	m_menuText1.setString("Play");
	m_menuText2.setString("Online Mode");
	m_menuText3.setString("Exit");
	m_menuText1.setCharacterSize(36);
	m_menuText2.setCharacterSize(36);
	m_menuText3.setCharacterSize(36);
	m_menuText1.setFillColor(sf::Color::White);
	m_menuText2.setFillColor(sf::Color::White);
	m_menuText3.setFillColor(sf::Color::White);

	auto centerTextInRect = [](sf::Text& txt, const sf::RectangleShape& rect)
	{
		auto bounds = txt.getLocalBounds();
		sf::Vector2f origin(bounds.position.x + bounds.size.x / 2.f, bounds.position.y + bounds.size.y / 2.f);
		txt.setOrigin(origin);
		auto pos = rect.getPosition();
		auto size = rect.getSize();
		sf::Vector2f finalPos(pos.x + size.x / 2.f, pos.y + size.y / 2.f);
		txt.setPosition(finalPos);
	};
	centerTextInRect(m_menuText1, m_menuOption1);
	centerTextInRect(m_menuText2, m_menuOption2);
	centerTextInRect(m_menuText3, m_menuOption3);

	// Multiplayer modal UI
	m_modalRect.setSize(sf::Vector2f(600.f, 400.f));
	m_modalRect.setFillColor(sf::Color(30, 30, 30, 220));
	m_modalRect.setOutlineThickness(2.f);
	m_modalRect.setOutlineColor(sf::Color::White);
	m_modalRect.setPosition(sf::Vector2f(ScreenSize::s_width / 2.f - 300.f, ScreenSize::s_height / 2.f - 200.f));

	m_modalTitle.setFont(m_arialFont);
	m_modalTitle.setString("Multiplayer");
	m_modalTitle.setCharacterSize(42);
	m_modalTitle.setFillColor(sf::Color::White);
	auto tb = m_modalTitle.getLocalBounds();
	sf::Vector2f tbOrigin(tb.position.x + tb.size.x / 2.f, tb.position.y + tb.size.y / 2.f);
	m_modalTitle.setOrigin(tbOrigin);
	m_modalTitle.setPosition(sf::Vector2f(ScreenSize::s_width / 2.f, ScreenSize::s_height / 2.f - 140.f));

	m_modalHostBtn.setSize(sf::Vector2f(220.f, 70.f));
	m_modalJoinBtn.setSize(sf::Vector2f(220.f, 70.f));
	m_modalHostBtn.setFillColor(sf::Color(80, 160, 255));
	m_modalJoinBtn.setFillColor(sf::Color(255, 160, 80));
	float modalCenterX = ScreenSize::s_width / 2.f;
	float modalCenterY = ScreenSize::s_height / 2.f;
	m_modalHostBtn.setPosition(sf::Vector2f(modalCenterX - 240.f, modalCenterY - 40.f));
	m_modalJoinBtn.setPosition(sf::Vector2f(modalCenterX + 20.f, modalCenterY - 40.f));

	m_modalHostText.setFont(m_arialFont);
	m_modalJoinText.setFont(m_arialFont);
	m_modalHostText.setString("Host");
	m_modalJoinText.setString("Join");
	m_modalHostText.setCharacterSize(32);
	m_modalJoinText.setCharacterSize(32);
	m_modalHostText.setFillColor(sf::Color::White);
	m_modalJoinText.setFillColor(sf::Color::White);
	centerTextInRect(m_modalHostText, m_modalHostBtn);
	centerTextInRect(m_modalJoinText, m_modalJoinBtn);

	m_modalStatusText.setFont(m_arialFont);
	m_modalStatusText.setCharacterSize(24);
	m_modalStatusText.setFillColor(sf::Color(200, 200, 200));
	m_modalStatusText.setString("");
	m_modalStatusText.setPosition(sf::Vector2f(modalCenterX, modalCenterY + 110.f));
	auto sb = m_modalStatusText.getLocalBounds();
	sf::Vector2f sbOrigin(sb.position.x + sb.size.x / 2.f, sb.position.y + sb.size.y / 2.f);
	m_modalStatusText.setOrigin(sbOrigin);

	resetGame();
	m_state = GameState::MainMenu;
}

void Game::resetGame()
{
	m_leftScore = 0;
	m_rightScore = 0;
	m_leftScoreText.setString(std::to_string(m_leftScore));
	m_rightScoreText.setString(std::to_string(m_rightScore));
	m_gameOver = false;
	m_leftPaddle.setPosition(sf::Vector2f(50.f, (float)ScreenSize::s_height / 2.f - m_leftPaddle.getSize().y / 2.f));
	m_rightPaddle.setPosition(sf::Vector2f((float)ScreenSize::s_width - 50.f - m_rightPaddle.getSize().x, (float)ScreenSize::s_height / 2.f - m_rightPaddle.getSize().y / 2.f));
	m_ball.setPosition(sf::Vector2f((float)ScreenSize::s_width / 2.f - m_ball.getRadius(), (float)ScreenSize::s_height / 2.f - m_ball.getRadius()));
	m_ballVelocity = sf::Vector2f(-400.f, -250.f);
}

void Game::run()
{
	sf::Clock clock;
	sf::Time timeSinceLastUpdate = sf::Time::Zero;
	
	sf::Time timePerFrame = sf::seconds(1.0f / FPS);
	while (m_window.isOpen())
	{
		processEvents();

		//HOSTING THE LOBBY NETWORKING COMPONENT
		if(m_state == GameState::HostingLobby)
		{
			lookingForClient();
		}

		//HOST GAMEPLAY NETWORKING
		if (m_state == GameState::Playing && m_isNetworkedGame && m_isHost) 
		{
			RecieveTransferPacket();
		}

		//GUEST LOBBY NEWTORKING COMPONENT
		if (m_state == GameState::JoiningLobby)
		{
			lookingForHost();
		}

		//GUEST GAMEPLAY NETWORKING
		if (m_state == GameState::Playing && m_isNetworkedGame && !m_isHost) 
		{
			guestPaddleController();	// sending paddle position to host

			recieveNetworkState();		// receiving game state from host
		}

		timeSinceLastUpdate += clock.restart();
		while (timeSinceLastUpdate > timePerFrame)
		{
			timeSinceLastUpdate -= timePerFrame;
			processEvents();
			update(timePerFrame.asMilliseconds());
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
		render();
#ifdef TEST_FPS
		x_drawFrameCount++;
#endif
	}
}


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

void Game::processGameEvents(const sf::Event& event)
{
	if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>())
	{
		switch (keyPressed->scancode)
		{
		case sf::Keyboard::Scancode::Escape:
			// If in game, return to main menu instead of closing.
			if (m_state == GameState::MainMenu)
			{
				m_window.close();
			}
			else
			{
				m_state = GameState::MainMenu;
				m_showMultiplayerModal = false;
				m_modalStatusText.setString("");
				resetGame();
			}
			break;
		default:
			break;
		}
	}
	if (m_state == GameState::MainMenu)
	{
		if (const auto* mousePressed = event.getIf<sf::Event::MouseButtonPressed>())
		{
			if (static_cast<int>(mousePressed->button) == 0)
			{
				sf::Vector2f mousePos(static_cast<float>(mousePressed->position.x), static_cast<float>(mousePressed->position.y));
				auto inRect = [&](const sf::RectangleShape& r){ return r.getGlobalBounds().contains(mousePos); };
				if (m_showMultiplayerModal)
				{
					if (inRect(m_modalHostBtn))
					{
						waitingForClient();
						m_modalStatusText.setString("waiting for client");
						auto sb2 = m_modalStatusText.getLocalBounds();
						m_modalStatusText.setOrigin(sf::Vector2f(sb2.position.x + sb2.size.x / 2.f, sb2.position.y + sb2.size.y / 2.f));
					}
					else if (inRect(m_modalJoinBtn))
					{
						waitingForHost();
						m_modalStatusText.setString("waiting for host");
						auto sb3 = m_modalStatusText.getLocalBounds();
						m_modalStatusText.setOrigin(sf::Vector2f(sb3.position.x + sb3.size.x / 2.f, sb3.position.y + sb3.size.y / 2.f));
					}
				}
				else
				{
					if (inRect(m_menuOption1))
					{
						m_state = GameState::Playing;
						resetGame();
					}
					else if (inRect(m_menuOption2))
					{
						multiplayerMode();
						m_showMultiplayerModal = true;
					}
					else if (inRect(m_menuOption3))
					{
						m_window.close();
					}
				}
			}
		}
	}
}

void Game::update(double dt)
{
	// dt arrives in milliseconds; convert to seconds
	float floatSeconds = static_cast<float>(dt) / 1000.f;

	if (m_state == GameState::Playing && m_isNetworkedGame && !m_isHost) { // ensures guest doesn't run gameplay update logic
		if (m_hasPrev && m_hasCurr) {
			float interpSpeed = 60.0f; //60hz host

			m_interpAlpha += interpSpeed * floatSeconds;
			if (m_interpAlpha > 1.0f) m_interpAlpha = 1.0f;
		}
		return;
	}

	// Do not update gameplay while in main menu
	if (m_state != GameState::Playing)
	{
		return;
	}

	// If game over listen for space to restart
	if (m_gameOver)
	{
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space))
		{
			resetGame();
		}
		return;
	}

	// Player input - left paddle: W/S, right paddle: Up/Down
	if (!m_isNetworkedGame)
	{
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
	else {
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W) ||
			sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up))
		{
			m_leftPaddle.move(sf::Vector2f(0.f, -m_paddleSpeed * floatSeconds));
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S) ||
			sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down))
		{
			m_leftPaddle.move(sf::Vector2f(0.f, m_paddleSpeed * floatSeconds));
		}

		//Moving networked player 2
		if(m_isNetP2Up) {
			m_rightPaddle.move(sf::Vector2f(0.f, -m_paddleSpeed * floatSeconds));
		}
		if(m_isNetP2Down) {
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

	// Ball out of bounds - simple reset and score
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

void Game::render()
{
	m_window.clear(sf::Color(0, 0, 0, 0));
#ifdef TEST_FPS
	m_window.draw(x_updateFPS);
	m_window.draw(x_drawFPS);
#endif
	if (m_state != GameState::Playing)
	{
		m_window.draw(m_menuOption1);
		m_window.draw(m_menuOption2);
		m_window.draw(m_menuOption3);
		m_window.draw(m_menuText1);
		m_window.draw(m_menuText2);
		m_window.draw(m_menuText3);
		if (m_showMultiplayerModal)
		{
			m_window.draw(m_modalRect);
			m_window.draw(m_modalTitle);
			m_window.draw(m_modalHostBtn);
			m_window.draw(m_modalJoinBtn);
			m_window.draw(m_modalHostText);
			m_window.draw(m_modalJoinText);
			m_window.draw(m_modalStatusText);
		}
		m_window.display();
		return;
	}
	
	// If guest in networked game, interpolate between previous and current state
	if(m_state == GameState::Playing && m_isNetworkedGame && !m_isHost) {
		if (m_hasPrev && m_hasCurr) {
			auto lerp = [](float a, float b, float alpha) {
				return a + (b - a) * alpha;
				};

			float p1Y = lerp(m_prevState.p1Y, m_currState.p1Y, m_interpAlpha);
			float p2Y = lerp(m_prevState.p2Y, m_currState.p2Y, m_interpAlpha);
			float ballX = lerp(m_prevState.ballX, m_currState.ballX, m_interpAlpha);
			float ballY = lerp(m_prevState.ballY, m_currState.ballY, m_interpAlpha);

			m_leftPaddle.setPosition(sf::Vector2f(m_leftPaddle.getPosition().x, p1Y));
			m_rightPaddle.setPosition(sf::Vector2f(m_rightPaddle.getPosition().x, p2Y));
			m_ball.setPosition(sf::Vector2f(ballX, ballY));

			m_leftScore = m_currState.p1Score;
			m_rightScore = m_currState.p2Score;
			m_leftScoreText.setString(std::to_string(m_leftScore));
			m_rightScoreText.setString(std::to_string(m_rightScore));
		}
	}
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

void Game::multiplayerMode()
{
	m_isNetworkedGame = true;
	m_modalStatusText.setString("");
}

void Game::waitingForClient()
{
	m_isNetworkedGame = true;
	m_isHost = true;

	const unsigned short hostPort = 54000;
	if (!m_hostNet.bind(hostPort))
	{
		m_modalStatusText.setString("Error: Could not bind to port");
		auto sb = m_modalStatusText.getLocalBounds();
		m_modalStatusText.setOrigin(sf::Vector2f(sb.position.x + sb.size.x / 2.f, sb.position.y + sb.size.y / 2.f));
		return;
	}

	//switch to hosting lobby mode
	m_state = GameState::HostingLobby;
	m_modalStatusText.setString("Hosting on port " + std::to_string(hostPort) + "\nWaiting for client...");
}

void Game::waitingForHost()
{
	m_isNetworkedGame = true;
	m_isHost = false;
	
	// Bind guest UDP socket on auto-assigned port
	if(!m_guestNet.bind(0))
	{
		m_modalStatusText.setString("Error: Could not bind to port");
		auto bounds = m_modalStatusText.getLocalBounds();
		m_modalStatusText.setOrigin(sf::Vector2f(bounds.position.x + bounds.size.x / 2.f,
			bounds.position.y + bounds.size.y / 2.f));
		return;
	}

	// First broadcast attempt
	const unsigned short discoveryPort = 54000;
	m_guestNet.sendFindHost(discoveryPort);

	// Switch to waiting joining lobby
	m_state = GameState::JoiningLobby;
	m_modalStatusText.setString("Searching for host on port " + std::to_string(discoveryPort) + "...");
	auto bounds = m_modalStatusText.getLocalBounds();
	m_modalStatusText.setOrigin(sf::Vector2f(bounds.position.x + bounds.size.x / 2.f,
		bounds.position.y + bounds.size.y / 2.f));
}

void Game::lookingForClient()
{
	//Listen for discovery broadcasts
	m_hostNet.pollForDiscoveryRequests();

	//Listen for HELLO handshakes from clients
	if (m_hostNet.pollForHello()) {
		// a client has connected
		m_modalStatusText.setString("Client connected!");
		m_state = GameState::Playing;
		resetGame();

		m_showMultiplayerModal = false;
	}
}

void Game::lookingForHost()
{
	const unsigned short discoveryPort = 54000;

	//Listen for host discovery responses
	m_guestNet.sendFindHost(discoveryPort);

	IpAddress hostAddr{IpAddress::Any};
	unsigned short hostPort{ 0 };

	//Check for HOST_HERE
	if(m_guestNet.recieveHostHere(hostAddr, hostPort)) {
		//Connected to host
		m_modalStatusText.setString("Host Found! Connecting...");
		auto sb = m_modalStatusText.getLocalBounds();
		m_modalStatusText.setOrigin(sf::Vector2f(sb.position.x + sb.size.x / 
			2.f, sb.position.y + sb.size.y / 2.f));

		//Send HELLO to host
		m_guestNet.sendHello();
	}

	//Wait for HELLO_ACK from host
	if(m_guestNet.recieveHelloAck()) {
		// Successfully connected to host
		m_modalStatusText.setString("Connected to host!");
		auto sb = m_modalStatusText.getLocalBounds();
		m_modalStatusText.setOrigin(sf::Vector2f(sb.position.x + sb.size.x /
			2.f, sb.position.y + sb.size.y / 2.f));

		//Connection complete, start game
		m_state = GameState::Playing;
		m_hasPrev = false;
		m_hasCurr = false;
		resetGame();

		m_showMultiplayerModal = false;
	}
}

void Game::recieveNetworkState()
{
	NetLogicStates incoming;
	if(!m_guestNet.recieveStateUpdate(incoming)) {
		//No state received
		return;
	}

	if (!m_hasCurr || incoming.seqNum > m_currState.seqNum) { //only accept newer states
		m_prevState = m_currState;
		m_currState = incoming;

		m_hasPrev = m_hasCurr;
		m_hasCurr = true;

		m_interpAlpha = 0.f; //reset interpolation alpha
	}
}

void Game::RecieveTransferPacket()
{
	//---- Get guest input ----
	int8_t guestInput = m_hostNet.recieveGuestInput();

	//apply guest input to right paddle
	//[guest input = -1 -> up , 1 -> down , 0 -> no input]
	if (guestInput == 0) {
		m_isNetP2Up = false;
		m_isNetP2Down = false;
	}
	else if (guestInput == -1)
	{
		m_isNetP2Up = true;
		m_isNetP2Down = false;
	}
	else if (guestInput == 1)
	{
		m_isNetP2Down = true;
		m_isNetP2Up = false;
	}

	//---- Build authoritative state packet ----
	NetLogicStates state;
	state.messageType = MessageTypes::STATE_UPDATE;
	state.seqNum = m_seq++;
	state.p1Y = m_leftPaddle.getPosition().y;
	state.p2Y = m_rightPaddle.getPosition().y;
	state.ballX = m_ball.getPosition().x;
	state.ballY = m_ball.getPosition().y;
	state.ballVelX = m_ballVelocity.x;
	state.ballVelY = m_ballVelocity.y;
	state.p1Score = m_leftScore;
	state.p2Score = m_rightScore;

	//---- Send authoritative state to guest ----
	m_hostNet.sendStateUpdate(state);
}

void Game::guestPaddleController()
{
	int8_t inputY = 0;

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up) ||
		sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W))
	{
		inputY = -1;
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down) ||
		sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))
	{
		inputY = 1;
	}

	m_guestNet.sendInput(inputY);
}

