#pragma once
#include "paddle.h"
using namespace juce;

class Ball : public Movable
{
public:
    Ball(){
        setWantsKeyboardFocus(false);
        setInterceptsMouseClicks(false, false);
    }
    
    void paint(Graphics& g) override{
        g.setColour(Colour(0xFF0070FF));
        g.fillEllipse(0, 0, getWidth(), getHeight());
    }
    
    void interactY(){
        if(y > limitBottom || y < limitTop)
            deltaY *= -1;
    }
    
    void interactX(Paddle& pA, Paddle& pB){
        if( pA.ballTouchesPaddle(x, y, getWidth(), false) ){
            x = pA.getMovableX()+pA.getWidth();
            deltaX *= -1;
        }
        else if( pB.ballTouchesPaddle(x, y, getWidth(), true) ){
            x = pB.getMovableX()-getWidth();
            deltaX *= -1;
        }
    }
    
    bool lostBall(bool leftOrRight){
        if(!leftOrRight && x <= limitLeft)
            return true;
        else if(leftOrRight && x >= limitRight)
            return true;
        return false;
    }
private:
};
