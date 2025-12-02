#include "GuestNetworkController.h"

using namespace std;

GuestNetworkController::GuestNetworkController()
	: m_hostAddress(IpAddress::Any),
	m_hostPort(0),
	m_isConnected(false)
{
	m_socket.setBlocking(false);
}

bool GuestNetworkController::bind(unsigned short port)
{
	if (m_socket.bind(port) != sf::Socket::Status::Done) {
		cout << "GuestNetworkController: Failed to bind on port " << port << endl;
		return false;
	}
	cout << "GuestNetworkController: Bound on port " << port << endl;
	return true;
}

void GuestNetworkController::sendFindHost(unsigned short discoveryPort)
{

}

bool GuestNetworkController::recieveHostHere(sf::IpAddress& outAddress, unsigned short& outPort)
{
	return false;
}

void GuestNetworkController::sendHello()
{
}

bool GuestNetworkController::recieveHelloAck()
{
	return false;
}

void GuestNetworkController::sendInput(int8_t inputY)
{
}

bool GuestNetworkController::recieveState(NetLogicStates& state)
{
	return false;
}