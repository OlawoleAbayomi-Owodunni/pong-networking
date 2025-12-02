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
	// Build FIND_HOST packet (message type 1)
	uint8_t msg = MessageTypes::FIND_HOST;

	sf::Socket::Status status = m_socket.send(&msg, sizeof(msg), sf::IpAddress::Broadcast, discoveryPort);

	if(status!= sf::Socket::Status::Done)
	{
		cout << "GuestNetworkController: Failed to send FIND_HOST (status "
			<< static_cast<int>(status) << ")" << endl;
		return;
	}
}

bool GuestNetworkController::recieveHostHere(sf::IpAddress& outAddress, unsigned short& outPort)
{
	Buffer buffer;
	Socket::Status status = m_socket.receive(buffer.data, sizeof(buffer.data), buffer.recieved, buffer.sender, buffer.senderPort);

	// ---- ERROR CHECKS ----
	if (status != Socket::Status::Done)
		return false;

	if (!buffer.sender.has_value() || buffer.recieved < 3)
		return false;

	uint8_t msgType = buffer.data[0];
	if (msgType != MessageTypes::HOST_HERE)
		return false;

	// ---- Extract host port (from bytes 1 and 2, big-endian) ----
	uint8_t hi = static_cast<uint8_t>(buffer.data[1]);
	uint8_t lo = static_cast<uint8_t>(buffer.data[2]);
	unsigned short hostPort =
		(static_cast<unsigned short>(hi) << 8) |
		static_cast<unsigned short>(lo);

/*potential point of failure!*/ m_hostAddress = buffer.sender.value();
	m_hostPort = hostPort;

	outAddress = m_hostAddress;
	outPort = m_hostPort;

	cout << "GuestNetworkController: Found host at "
		<< m_hostAddress.toString() << ":" << m_hostPort << endl;
	return true;
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