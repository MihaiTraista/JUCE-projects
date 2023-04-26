#pragma once
using namespace juce;

class Paddle : public Movable
{
public:
    Paddle(){
        setWantsKeyboardFocus(false);
        setInterceptsMouseClicks(false, false);
    }
    
    bool ballTouchesPaddle(int ballX, int ballY, int ballSize, bool rightPaddleCheck){
        bool inYRange = false;
//        std::cout << "rightPaddleCheck " << rightPaddleCheck << ", ballX+ballSize " << ballX+ballSize << ", " << x << std::endl;
        if(ballY-ballSize >= y && ballY <= y+height)
            inYRange = true;
        if(inYRange){
            if(rightPaddleCheck && ballX+ballSize > x)
                return true;
            else if(!rightPaddleCheck && ballX <= width)
                return true;
        }
        return false;
    }
    
    void checkLimitsY(){
        if(y > limitBottom)         y = limitBottom;
        else if(y < limitTop)       y = limitTop;
    }

    void paint(Graphics& g) override{
        g.fillAll(Colours::red);
    }
};
