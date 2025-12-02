#include "HostNetworkController.h"
#include <iostream>

using namespace std;

HostNetworkController::HostNetworkController() 
	: m_guestAddress(IpAddress::Any),
	m_guestPort(0), 
	m_hasGuest(false), 
	m_latestGuestInput(0)
{
	m_socket.setBlocking(false);
}

bool HostNetworkController::bind(unsigned short port)
{
	if (m_socket.bind(port) != sf::Socket::Status::Done) {
		cout << "HostNetworkController: Failed to bind on port " << port << endl;
		return false;
	}
	cout << "HostNetworkController: Bound on port " << port << endl;
	return true;
}

