#pragma once
using namespace juce;

struct Player{

    void addPoint(){
        points++;
    }
    int points = 0;
};

struct GameManager{
    static Player playerOne;
    static Player playerTwo;
    
    static void lostPoint(int playerWhoLostPoints){
        switch(playerWhoLostPoints){
            case 1:
                playerTwo.addPoint();
                break;
            case 2:
                playerOne.addPoint();
                break;
        }
        std::cout << "current score " << playerOne.points << ", " << playerTwo.points << std::endl;
    }
};

Player GameManager::playerOne = Player();
Player GameManager::playerTwo = Player();
