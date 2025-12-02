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

void HostNetworkController::pollForDiscoveryRequests()
{
	//Buffer for incoming data
	char data[16];
	size_t recieved = 0;
	std::optional<sf::IpAddress> sender;
	unsigned short senderPort = 0;

	Socket::Status status = m_socket.receive(data, sizeof(data), recieved, sender, senderPort);

	if(status != Socket::Status::Done)
		return;

	if(recieved < 1)
		return;

	uint8_t messageType = data[0];

	if (messageType == MessageTypes::FIND_HOST)
	{
		// Build HOST_HERE reply (message type 2)
		uint8_t reply[3];
		reply[0] = MessageTypes::HOST_HERE; // HOST_HERE
		// next two bytes = port as unit16 (big-endian). not important for Lan but good practice anyway
		reply[1] = (m_socket.getLocalPort() >> 8) & 0xFF;
		reply[2] = m_socket.getLocalPort() & 0xFF;

		// Send reply
		auto sendStatus = m_socket.send(reply, sizeof(reply), sender.value(), senderPort);
		if(sendStatus != Socket::Status::Done)
		{
			cout << "HostNetworkController: Failed to send HOST_HERE reply to "
				<< sender->toString() << ":" << senderPort << endl;
			return;
		}

		cout << "HostNetworkController: Recieved FIND_HOST from "
			<< sender->toString() << ":" << senderPort
			<< " -> sent from HOST_HERE\n" << endl;
	}
}

bool HostNetworkController::pollForHello()
{
	// only proceed if we haven't accepted a guest yet
	if (m_hasGuest)
		return false;

	//Buffer for incoming data
	char data[16];
	size_t recieved = 0;
	std::optional<sf::IpAddress> sender;
	unsigned short senderPort = 0;

	auto status = m_socket.receive(data, sizeof(data), recieved, sender, senderPort);

	// ---- ERROR CHECKS ----

	if (status != Socket::Status::Done)
		return false;
	if (!sender.has_value() || recieved < 1)
		return false;

	uint8_t messageType = data[0];
	if(messageType != MessageTypes::HELLO)
		return false;

	// The HELLO packet should contain the gameplay recieve port for the guest
	if (recieved < 3)
		return false; // invalid HELLO packet

	// ---- Extract guest port (from bytes 1 and 2, big-endian) ----
	unsigned short guestPort =
		(static_cast<uint8_t>(data[1]) << 8) |
		static_cast<uint8_t>(data[2]);

	m_guestAddress = sender.value();
	m_guestPort = guestPort;
	m_hasGuest = true;

	// ---- Send HELLO_ACK ----
	uint8_t reply[1];
	reply[0] = MessageTypes::HELLO_ACK;

	auto sendStatus = m_socket.send(reply, sizeof(reply), m_guestAddress, m_guestPort);
	if (sendStatus != Socket::Status::Done)
	{
		cout << "HostNetworkController: Failed to send HELLO_ACK to "
			<< m_guestAddress.toString() << ":" << m_guestPort << endl;
		m_hasGuest = false; // reset guest info
		return false;
	}
	cout << "HostNetworkController: Guest connected from "
		<< m_guestAddress.toString() << ":" << m_guestPort <<
		" -> HELLO_ACK sent" << endl;
}
