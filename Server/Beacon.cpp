#include "Beacon.h"
#include <iostream>
Beacon::Beacon(int port)
	:port(port),isWorking(false)
{
	broadcastIP = "192.168.1.255";
	//broadcastIP = "127.0.0.1";
	localIP = GetLocalIPAddress();

	if (localIP.empty())
	{
		throw std::runtime_error("Failed no Ip");
	}

	std::cout << "local IP Address" << localIP <<std::endl;
}

Beacon::Beacon(const Beacon& other)
	:port(other.port),broadcastIP(other.broadcastIP),isWorking(other.isWorking.load())
{
	localIP = GetLocalIPAddress();
}

Beacon& Beacon::operator=(const Beacon& other)
{
	if (this != &other)
	{
		port = other.port;
		broadcastIP = other.broadcastIP;
		isWorking.store(other.isWorking.load());
		localIP = GetLocalIPAddress();
	}
	return *this;
}

Beacon::~Beacon()
{
	stop();
}

void Beacon::start()
{
	if (isWorking)
		return;

	isWorking = true;
	mainThread = std::thread([this]()
	{
			
			while (isWorking)
			{

				std::string message = localIP + ":" + "53000";
				BroadcastMessage(message);
				
				std::this_thread::sleep_for(std::chrono::milliseconds(BROADCAST_DALAY_MS));
			}	
	});

}

void Beacon::stop()
{
	if (!isWorking)
		return;

	isWorking = false;
	if (mainThread.joinable())
	mainThread.join();
}

void Beacon::BroadcastMessage(const std::string& message)
{
	sf::UdpSocket socket;

		if (socket.send(message.c_str(),message.size(),broadcastIP,port)!=sf::Socket::Done)
		{

			std::cerr << "Failed to send message" << std::endl;
			return;

		}

	std::cout << "Sending to: " << broadcastIP << ":" << port << std::endl;


}
std::string Beacon::GetLocalIPAddress()
{
	return sf::IpAddress::getLocalAddress().toString();
}
//void Beacon::BroadcastMessage(const std::string& message) {
//	sf::UdpSocket socket;
//	socket.setBlocking(false);
//
//	nlohmann::json msg;
//	msg["type"] = "beacon";
//	msg["port"] = 53000; // фиксированный порт, куда клиент будет подключаться
//
//	std::string jsonStr = msg.dump();
//
//	if (socket.send(jsonStr.c_str(), jsonStr.size(), broadcastIP, port) != sf::Socket::Done) {
//		std::cerr << "Failed to send message" << std::endl;
//	}
//	else {
//		std::cout << "Sending to: " << broadcastIP << ":" << port << " - " << jsonStr << std::endl;
//	}
//}
//
