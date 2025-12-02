#pragma once

// If VS Debug build is enabled, then any block of code enclosed within 
//  the preprocessor directive #ifdef TEST_FPS is compiled and executed.
#ifdef _DEBUG
#define TEST_FPS
#endif // _DEBUG

#include <SFML/Graphics.hpp>
#include <string>
#include <SFML/Network.hpp>

#include "HostNetworkController.h"
#include "GuestNetworkController.h"

using namespace std;
using namespace sf;

/// <summary>
/// @author OA-O
/// @date November 2025
/// @version 1.0
/// 
/// </summary>

/// <summary>
/// @brief Main class for the SFML Playground project.
/// 
/// This will be a single class framework for learning about SFML. 
/// Example usage:
///		Game game;
///		game.run();
/// </summary>

struct ScreenSize
{
public:
	static const int s_width{ 1440 };

	static const int s_height{ 900 };
};

enum class GameState
{
	MainMenu,
	Playing,
	HostingLobby,
	JoiningLobby
};

class Game
{
public:
	/// <summary>
	/// @brief Default constructor that initialises the SFML window, 
	///   and sets vertical sync enabled. 
	/// </summary>
	Game();

	/// <summary>
	/// @brief the main game loop.
	/// 
	/// A complete loop involves processing SFML events, updating and drawing all game objects.
	/// The actual elapsed time for a single game loop is calculated. If this value is 
	///  greater than the target time for one loop (1 / 60), then (and only then) is an update 
	///  operation performed.
	/// The target is at least one update and one render cycle per game loop, but typically 
	///  more render than update operations will be performed as we expect our game loop to
	///  complete in less than the target time.
	/// </summary>
	void run();

protected:
	/// <summary>
	/// @brief Once-off game initialisation code
	/// </summary>	
	void init();
	/// <summary>
	/// @brief Placeholder to perform updates to all game objects.
	/// </summary>
	/// <param name="time">update delta time</param>
	void update(double dt);

	/// <summary>
	/// @brief Draws the background and foreground game objects in the SFML window.
	/// The render window is always cleared to black before anything is drawn.
	/// </summary>
	void render();

	/// <summary>
	/// @brief Checks for events.
	/// Allows window to function and exit. 
	/// Events are passed on to the Game::processGameEvents() method.
	/// </summary>	
	void processEvents();

	/// <summary>
	/// @brief Handles all user input.
	/// </summary>
	/// <param name="event">system event</param>
	void processGameEvents(const sf::Event&);

	/// <summary>
	/// @brief Resets game state to start a new match.
	/// </summary>
	void resetGame();
	void multiplayerMode(); // start multiplayer/network mode stub
	void waitingForClient(); // after Host selected
	void waitingForHost();   // after Join selected
	void RecieveTransferPacket();

	void lookingForClient();
	void lookingForHost();

	// Font used for all text
	sf::Font m_arialFont{ "ASSETS/FONTS/ariblk.ttf" };
	// main window
	sf::RenderWindow m_window;

	// Simple Pong objects
	sf::RectangleShape m_leftPaddle;
	sf::RectangleShape m_rightPaddle;
	sf::CircleShape m_ball;
	// center line
	sf::RectangleShape m_centerLine;

	// overlay for end game (dimming + text)
	sf::RectangleShape m_overlayRect;
	sf::Text m_overlayText{ m_arialFont };

	// Main menu rectangles
	sf::RectangleShape m_menuOption1; // Play
	sf::RectangleShape m_menuOption2; // Online Mode
	sf::RectangleShape m_menuOption3; // Exit
	// Menu texts
	sf::Text m_menuText1{ m_arialFont };
	sf::Text m_menuText2{ m_arialFont };
	sf::Text m_menuText3{ m_arialFont };

	// Multiplayer modal state and UI
	bool m_showMultiplayerModal{ false };
	sf::RectangleShape m_modalRect;
	sf::RectangleShape m_modalHostBtn;
	sf::RectangleShape m_modalJoinBtn;
	sf::Text m_modalTitle{ m_arialFont };
	sf::Text m_modalHostText{ m_arialFont };
	sf::Text m_modalJoinText{ m_arialFont };
	sf::Text m_modalStatusText{ m_arialFont };

	GameState m_state{ GameState::MainMenu };

	// velocities
	sf::Vector2f m_ballVelocity{ -400.f, -250.f };
	float m_paddleSpeed{ 600.f };

	// Scores
	int m_leftScore{ 0 };
	int m_rightScore{ 0 };
	// Score text
	sf::Text m_leftScoreText{ m_arialFont };
	sf::Text m_rightScoreText{ m_arialFont };

	// game state
	bool m_gameOver{ false };
	int m_winScore{ 5 };

#ifdef TEST_FPS
	sf::Text x_updateFPS{ m_arialFont };	// text used to display updates per second.
	sf::Text x_drawFPS{  m_arialFont };	// text used to display draw calls per second.
	sf::Time x_secondTime{ sf::Time::Zero };			// counter used to establish when a second has passed.
	int x_updateFrameCount{ 0 };						// updates per second counter.
	int x_drawFrameCount{ 0 };							// draws per second counter.
#endif // TEST_FPS

	bool m_isNetP2Up{ false }; // is networked player 2 moving up
	bool m_isNetP2Down{ false }; // is networked player 2 moving down

	// network related variables
	HostNetworkController m_hostNet;
	bool m_isNetworkedGame{ false };
	bool m_isHost{ false };
	int m_seq{ 0 };
	GuestNetworkController m_guestNet;
};
