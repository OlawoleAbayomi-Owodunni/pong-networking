#pragma once
#include <iostream>
#include <SFML/Network.hpp>
#include "HostNetworkController.h"

class GuestNetworkController
{
public:
	GuestNetworkController();
	bool bind(unsigned short port);

	//Discovery + Handshake
	void sendFindHost(unsigned short discoveryPort);
	bool recieveHostHere(sf::IpAddress& outAddress, unsigned short& outPort);
	void sendHello();
	bool recieveHelloAck();

	//Gameplay traffic
	void sendInput(int8_t inputY);
	bool recieveState(NetLogicStates& state);

	//Host connection info
	bool isConncected() const { return m_isConnected; }
	IpAddress getHostAddress() const { return m_hostAddress; }
	unsigned short getHostPort() const { return m_hostPort; }

private:
	UdpSocket m_socket;

	IpAddress m_hostAddress;
	unsigned short m_hostPort{ 0 };
	bool m_isConnected{ false };
};

