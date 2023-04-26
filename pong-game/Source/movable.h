#pragma once
using namespace juce;

class Movable : public Component
{
public:
    Movable(){}
    virtual ~Movable(){}
    
    void setPosition(int _x, int _y) { x = _x; y = _y; }
    int getMovableX(){ return x; }
    int getMovableY(){ return y; }

    virtual void setDelta(int _deltaX, int _deltaY){
        deltaX = _deltaX;
        deltaY = _deltaY;
    }

    virtual void setLimits(int l, int r, int t, int b){
        limitLeft = l;
        limitRight = r;
        limitTop = t;
        limitBottom = b;
    }
    
    void resized() override     // this is a setter
    {
        x = getX();
        y = getY();
        width = getWidth();
        height = getHeight();
    }
    
    virtual void move(){
        x += deltaX;
        y += deltaY;
        setBounds(x, y, getWidth(), getHeight());
    }
    
protected:
    int x, y, deltaX, deltaY, width, height, limitLeft, limitRight, limitTop, limitBottom;;
};
