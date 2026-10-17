#pragma once
#include <memory>
#include <SFML/Network.hpp>
#include <mutex>
#include <nlohmann/json.hpp>
#include <vector>
#include"Beacon.h"

class Game
{

public:
    Game();

    ~Game();
    void Run();
    void watchForNewClients();
    
    
    void WinCheck(char symbol );
    void handleClient(int clientIndex, char symbol);


private:
 
    void ResetMap();
private:
    sf::TcpSocket client;
    sf::TcpSocket Timeclient;

    std::vector<sf::TcpSocket*> clients = { nullptr,nullptr };

    sf::TcpListener listener;

    std::mutex clientsMutex, coutMutex;
 
    char map[3][3];
    int mFirstplayerX = 0;
    int mFirstplayerY = 0;
    int mSecondplayerX = 0;
    int mSecondplayerY = 0;

    char SymbolX = 'X';
    char SymbolO = 'O';


    char mFirstPlayerSymbol = SymbolX;
    char mSecondPlayerSymbol = SymbolO;

    std::atomic<bool>mIsFirstPlayer = false;
    std::atomic<int> connectedClients = 0;

    int mMaximumNumberOfMove = 0;
    bool misGameFinish = false;

    Beacon beacon;

};

struct User
{
    sf::TcpSocket clientSocket;

    char clientSymbol;




};