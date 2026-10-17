#include "Game.h"
#include <iostream>

Game::Game()
{


	ResetMap();
	beacon = Beacon(55000);
}

Game::~Game()
{

}



void Game::Run()
{
	beacon.start();

	if (listener.listen(53000) != sf::Socket::Done)
	{
		std::cout << "error Starring server" << std::endl;
		return;
	}
	std::cout << "Starring server ,wait for client" << std::endl;

	//for (int i = 0; i < 2; ++i)
	//{
	//	clients[i] = new sf::TcpSocket;
	//	if (listener.accept(*clients[i]) != sf::Socket::Done)
	//	{
	//		std::cerr << "Error accepting client " << i << std::endl;
	//		delete clients[i];
	//		return;
	//	}
	//	std::cout << "Client " << i << " connected" << std::endl;
	//	std::thread(&Game::handleClient, this, i, i == 0 ? SymbolX : SymbolO).detach();
	//}
	//std::thread(&Game::watchForNewClients, this).detach();

	while (true)
	{
		if (connectedClients < 2)
		{
			sf::TcpSocket* newClient = new sf::TcpSocket;
			if (listener.accept(*newClient) == sf::Socket::Done)
			{
				int index = (clients[0] == nullptr) ? 0 : 1;
				clients[index] = newClient;
				connectedClients++;

				std::cout << "Client " << index << " reconnected" << std::endl;
				std::thread(&Game::handleClient, this, index, index == 0 ? SymbolX : SymbolO).detach();
			}
			else
			{
				delete newClient;
			}
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}



	std::cin.get();




}
void Game::watchForNewClients()
{
	while (true)
	{
		if (connectedClients < 2)
		{
			sf::TcpSocket* newClient = new sf::TcpSocket;
			if (listener.accept(*newClient) == sf::Socket::Done)
			{
				int index = (clients[0] == nullptr) ? 0 : 1;
				clients[index] = newClient;
				connectedClients++;

				std::cout << "Client " << index << " reconnected" << std::endl;
				std::thread(&Game::handleClient, this, index, index == 0 ? SymbolX : SymbolO).detach();
			}
			else
			{
				delete newClient;
			}
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
}


void Game::WinCheck(char symbol)
{

	if (symbol == map[0][0])
		if (symbol == map[1][1])
			if (symbol == map[1][1])
				std::cout << symbol << "Win" << std::endl;
					misGameFinish = true;
	if (symbol == map[2][0])
		if (symbol == map[1][1])
			if (symbol == map[0][2])
				std::cout << symbol << "Win" << std::endl;
					misGameFinish = true;
	if (symbol == map[0][0])
		if (symbol == map[1][0])
			if (symbol == map[2][0])
				std::cout << symbol << "Win" << std::endl;
					misGameFinish = true;
	if (symbol == map[0][1])
		if (symbol == map[1][1])
			if (symbol == map[2][1])
				std::cout << symbol << "Win" << std::endl;
					misGameFinish = true;
	if (symbol == map[0][2])
		if (symbol == map[1][2])
			if (symbol == map[2][2])
				std::cout << symbol << "Win" << std::endl;
					misGameFinish = true;
	if (symbol == map[0][0])
		if (symbol == map[0][1])
			if (symbol == map[0][2])
				std::cout << symbol << "Win" << std::endl;
					misGameFinish = true;
	if (symbol == map[1][0])
		if (symbol == map[1][1])
			if (symbol == map[1][2])
				std::cout << symbol << "Win" << std::endl;
					misGameFinish = true;
	if (symbol == map[2][0])
		if (symbol == map[2][1])
			if (symbol == map[2][2])
				std::cout << symbol << "Win" << std::endl;
					misGameFinish = true;


}

void Game::handleClient(int clientIndex, char symbol)
{


	sf::TcpSocket* client = clients[clientIndex];

	char buffer[1024];

	std::size_t received;

	while (true)
	{
		sf::Socket::Status status = client->receive(buffer, sizeof(buffer), received);

		if (status == sf::Socket::Done)
		{
			if (clientIndex == mIsFirstPlayer)
			{
				mIsFirstPlayer = !mIsFirstPlayer;
			}
			else
			{
				continue;
			}

			std::string msg(buffer, received);
			{
				std::lock_guard<std::mutex>lock(coutMutex);
				std::cout << "client " << clientIndex << ":" << msg << std::endl;
			}

			try
			{
				nlohmann::json j = nlohmann::json::parse(msg);

				if (j["MessageType"] == "Set")
				{

					int x = j["XCoords"];
					int y = j["YCoords"];

					if (map[x][y] == ' ')
					{
						map[x][y] = symbol;



						// Рассылка обновления всем
						nlohmann::json response;
						response["MessageType"] = "Update";
						response["XCoords"] = x;
						response["YCoords"] = y;
						response["Symbol"] = std::string() + symbol;

						std::string outMsg = response.dump();
						std::size_t sent;

						for (int i = 0; i < 2; ++i)
						{
							if (clients[i])
								clients[i]->send(outMsg.c_str(), outMsg.size() + 1, sent);
						}

						WinCheck(symbol);
					}
				}

				if (j["MessageType"] == "Restart")
				{
					if (misGameFinish)
					{
					
						ResetMap();
						// Рассылка обновления всем
						nlohmann::json response;
						response["MessageType"] = "Restart";
						response["reason"] = "User chose";


						std::string outMsg = response.dump();
						std::size_t sent;

						for (int i = 0; i < 2; ++i)
						{
							if (clients[i])
								clients[i]->send(outMsg.c_str(), outMsg.size() + 1, sent);
						}
						misGameFinish = false;
					}

				}







			}
			catch (const std::exception& ex)
			{
				std::cerr << "Invalid JSON from client " << clientIndex << ": " << ex.what() << std::endl;
			}


		}
		else if (status == sf::Socket::Disconnected)
		{


			{
				std::lock_guard<std::mutex>lock(coutMutex);
				std::cout << "client Disconect " << clientIndex << std::endl;
				clients[clientIndex] = nullptr;
				connectedClients--;


				//restart for disconect
				ResetMap();
				// Рассылка обновления всем
				nlohmann::json response;
				response["MessageType"] = "Restart";
				response["reason"] = "User Disconnected";

				std::string outMsg = response.dump();
				std::size_t sent;

				for (int i = 0; i < 2; ++i)
				{
					if (clients[i])
						clients[i]->send(outMsg.c_str(), outMsg.size() + 1, sent);
				}
				misGameFinish = false;






			}
			break;

		}
		else
		{
			{
				std::lock_guard<std::mutex>lock(coutMutex);
				std::cout << "Error receiving data " << clientIndex << std::endl;
			}
			break;
		}

	}

	std::lock_guard<std::mutex>lock(coutMutex);

	client->disconnect();

	delete client;

	clients[clientIndex] = nullptr;

}



void Game::ResetMap()
{


	for (size_t i = 0; i < 3; i++)
	{
		for (size_t j = 0; j < 3; j++)
		{
			map[i][j] = ' ';
		}

	}

}


