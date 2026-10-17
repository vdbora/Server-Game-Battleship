#pragma once
#include <string>
#include <thread>
#include <atomic>
#include <SFML/Network.hpp>
#include <nlohmann/json.hpp>

class Beacon
{
public:
	constexpr static uint32_t BROADCAST_DALAY_MS = 1000;



public:
	Beacon() = default;
	Beacon(int port);
	Beacon(const Beacon& other);
	Beacon& operator=(const Beacon& other);

	~Beacon();

	void start();
	void stop();

private:
	void BroadcastMessage(const std::string& message);
	std::string GetLocalIPAddress();

private:


	int port;
	std::string broadcastIP;
	std::string localIP;
	std::atomic<bool> isWorking{ false };
	std::thread mainThread;
};

