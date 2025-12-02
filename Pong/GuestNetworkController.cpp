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
	if (m_hostAddress == IpAddress::Any || m_hostPort == 0) {
		cout << "GuestNetworkController: Cannot send HELLO - host address/port not set" << endl;
		return;
	}

	// Build HELLO packet (message type 3)
	uint8_t buffer[3];
	buffer[0] = MessageTypes::HELLO;

	// Include our gameplay recieve port (bytes 1 and 2, big-endian)
	unsigned short guestPort = m_socket.getLocalPort();
	buffer[1] = (guestPort >> 8) & 0xFF;
	buffer[2] = guestPort & 0xFF;

	auto status = m_socket.send(buffer, sizeof(buffer), m_hostAddress, m_hostPort);
	if(status != Socket::Status::Done)
	{
		cout << "GuestNetworkController: Failed to send HELLO to "
			<< m_hostAddress.toString() << ":" << m_hostPort << endl;
		return;
	}
	else
	{
		cout << "GuestNetworkController: Sent HELLO to "
			<< m_hostAddress.toString() << ":" << m_hostPort << endl;
	}
}

bool GuestNetworkController::recieveHelloAck()
{
	Buffer buffer;
	Socket::Status status = m_socket.receive(buffer.data, sizeof(buffer.data), buffer.recieved, buffer.sender, buffer.senderPort);

	// ---- ERROR CHECKS ----
	if (status != Socket::Status::Done)
		return false;
	if (!buffer.sender.has_value() || buffer.recieved < 1)
		return false;

	uint8_t msgType = buffer.data[0];
	if (msgType != MessageTypes::HELLO_ACK)
	{
		cout << "GuestNetworkController: Expected HELLO_ACK but recieved different message type" << endl;
		return false;
	}

	// Handshake complete
	m_isConnected = true;
	cout << "GuestNetworkController: Recieved HELLO_ACK from host "
		<< m_hostAddress.toString() << ":" << m_hostPort
		<< " -> connected!" << endl;
	return true;
}

void GuestNetworkController::sendInput(int8_t inputY)
{
	if(!m_isConnected) {
		cout << "GuestNetworkController: Cannot send input - not connected to host" << endl;
		return;
	}

	// Build GUEST_INPUT packet
	uint8_t buffer[4];
	buffer[0] = MessageTypes::GUEST_INPUT;

	// Tick placeholder (0) - host currently doesn't use this
	buffer[1] = 0;
	buffer[2] = 0;

	// Input Y (byte 3)
	buffer[3] = static_cast<uint8_t>(inputY);

	auto status = m_socket.send(buffer, sizeof(buffer), m_hostAddress, m_hostPort);

	if(status != Socket::Status::Done)
	{
		cout << "GuestNetworkController: Failed to send GUEST_INPUT to "
			<< m_hostAddress.toString() << ":" << m_hostPort << endl;
		return;
	}
}

bool GuestNetworkController::recieveStateUpdate(NetLogicStates& state)
{
	char buffer[64];
	size_t recieved = 0;
	optional<sf::IpAddress> sender;
	unsigned short senderPort = 0;

	Socket::Status status = m_socket.receive(buffer, sizeof(buffer), recieved, sender, senderPort);

	// ---- ERROR CHECKS ----
	if (status != Socket::Status::Done)
		return false;

	if (!sender.has_value() || recieved < 31) // 1 + 4 + 24 + 2 = 31 bytes minimum
		return false;

	uint8_t msgType = buffer[0];
	if (msgType != MessageTypes::STATE_UPDATE)
		return false;

	// ---- Extract state ----
	size_t offset = 1;

	// Sequence number (4 bytes, big-endian)
	uint32_t seq = 0;
	seq |= (static_cast<uint8_t>(buffer[offset]) << 24);
	seq |= (static_cast<uint8_t>(buffer[offset + 1]) << 16);
	seq |= (static_cast<uint8_t>(buffer[offset + 2]) << 8);
	seq |= static_cast<uint8_t>(buffer[offset + 3]);

	auto readFloat = [&](float f) { // lambda to read float from 4 bytes
		memcpy(&f, buffer + offset, sizeof(float));
		offset += sizeof(float);
		};

	// message size (1 byte) and seqNum (4 bytes)
	state.messageType = msgType;
	state.seqNum = static_cast<int>(seq);

	// Floats (4 bytes each)
	readFloat(state.p1Y);
	readFloat(state.p2Y);
	readFloat(state.ballX);
	readFloat(state.ballY);
	readFloat(state.ballVelX);
	readFloat(state.ballVelY);

	// Scores (2 bytes, big-endian)
	state.p1Score = static_cast<uint8_t>(buffer[offset++]);
	state.p2Score = static_cast<uint8_t>(buffer[offset++]);

	return true;
}