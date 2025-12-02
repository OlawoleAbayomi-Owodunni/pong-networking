#pragma once
#include <SFML/Network.hpp>

using namespace sf;

struct NetLogicStates {
	uint8_t messageType;
	int seqNum;
	float p1Y = 0.f;
	float p2Y = 0.f;
	float ballX = 0.f;
	float ballY = 0.f;
	float ballVelX = 0.f;
	float ballVelY = 0.f;
	unsigned int p1Score = 0;
	unsigned int p2Score = 0;
};

struct Buffer {
	char data[16];
	size_t recieved = 0;
	std::optional<sf::IpAddress> sender;
	unsigned short senderPort = 0;
};

enum MessageTypes : uint8_t {
	FIND_HOST = 1,
	HOST_HERE = 2,
	HELLO = 3,
	HELLO_ACK = 4,
	GUEST_INPUT = 5,
	STATE_UPDATE = 6
};

class HostNetworkController
{
public:
	HostNetworkController();
	bool bind(unsigned short port);

	//Discovery + Handshake
	void pollForDiscoveryRequests();		//for FIND_HOST messages
	bool pollForHello();					//returns true if guest is connected

	//Gameplay traffic
	int8_t recieveGuestInput();				//returns -1, 0 or 1
	void sendStateUpdate(const NetLogicStates& state);

	//check if guest is still connected
	bool isGuestConnected() const { return m_hasGuest; }

private:
	UdpSocket m_socket;

	//Guest info
	IpAddress m_guestAddress;
	unsigned short m_guestPort;
	bool m_hasGuest;

	int8_t m_latestGuestInput;
};

