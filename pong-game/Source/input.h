#pragma once
using namespace juce;

class Input : public Component, private Timer
{
public:
    Input(){
        setWantsKeyboardFocus(true);
        startTimer(100);
    }

    bool keyPressed (const KeyPress& key) override
    {
        pressId = 0;
        if( key.isKeyCurrentlyDown(63233))  //  up key
            pressId = pressId | 1;
        else if( key.isKeyCurrentlyDown(63232))  //  up down
            pressId = pressId | 2;

        if( key.isKeyCurrentlyDown(87))  //  'W' key
            pressId = pressId | 8;
        else if( key.isKeyCurrentlyDown(83))  //  'S' down
            pressId = pressId | 4;

        std::cout << "pressId " << (int)pressId << ", " << key.getKeyCode() << std::endl;
        return 1;
    }
    
    bool keyStateChanged (bool isKeyDown) override
    {
        if(!isKeyDown)
            pressId = 0;
        return 1;
    }
    
    char getPressId(){
        return pressId;
    }
private:
    void timerCallback() override{
        stopTimer();
        grabKeyboardFocus();
    }
    
    char pressId;   //  first two bits represent the 'W' and 'S' keys, last two bits represent the arrow keys
};
