#pragma once

#include <JuceHeader.h>


class MainComponent  : public juce::Component
{
public:
    MainComponent()
    {
        setSize(600, 600);
        x0 = 0.1;     //  starting point
        y0 = 0.1;
        x1 = 0.25;     //  control point
        y1 = 0.75;
        x2 = 0.9;     //  ending point
        y2 = 0.9;
    }

/*
    void paint (juce::Graphics& g) override
    {
        juce::Path path1, path2;
        
        path1.startNewSubPath(x0*getWidth(), y0*getHeight());
        path1.lineTo(x1*getWidth(), y1*getHeight());
        path1.lineTo(x2*getWidth(), y2*getHeight());
        float p4x, p4y, p5x, p5y, p6x, p6y;

        p4x = mouseY * (x1-x0) + x0;
        p4y = mouseY * (y1-y0) + y0;
        p5x = (1-mouseY) * (x1-x2) + x2;
        p5y = (1-mouseY) * (y1-y2) + y2;

        p4x *= getWidth();
        p4y *= getHeight();
        p5x *= getWidth();
        p5y *= getHeight();
        
//        std::cout << p4x << ", " << p4y << ", " << p5x << ", " << p5y << std::endl;
        path1.startNewSubPath(p4x, p4y);
        path2.startNewSubPath(0, 0);
        path1.lineTo(p5x, p5y);
        path2.lineTo(1, 1);
        g.setColour(juce::Colours::red);
        g.strokePath(path1, juce::PathStrokeType(2.5f));
        
        p6x = mouseY * (p5x - p4x) + p4x;
        p6y = mouseY * (p5y - p4y) + p4y;
        g.setColour(juce::Colours::blue);
        g.fillEllipse(p6x-5, p6y-5, 10, 10);

        //g.setColour(juce::Colours::blue);
        //g.strokePath(path2, juce::PathStrokeType(2.5f));
    }
*/
    

    void paint (juce::Graphics& g) override
    {
        float t, x, y;
        juce::Path path;
        path.startNewSubPath(x0*getWidth(), y0*getHeight());
        path.lineTo(x1*getWidth(), y1*getHeight());
        path.lineTo(x2*getWidth(), y2*getHeight());
        path.lineTo(x0*getWidth(), y0*getHeight());
        for(int i=0; i<getWidth(); i++)
        {
            t = (float)i / (float)getWidth();
            x = solveForBezier(x0, x1, x2, t);
            y = solveForBezier(y0, y1, y2, t);
            x *= getWidth();
            y *= getHeight();
            path.lineTo(x, y);
            
        }
        g.setColour(juce::Colours::red);
        g.strokePath(path, juce::PathStrokeType(2.5));
    }

    float solveForBezierNew(float t)
    {
//        float retVal = (1-t)*( (1-t)*p0 + t*p1 ) + t*( (1-t)*p1 + t*p2 );
        float retVal;
        
        float p4x, p4y, p5x, p5y, p6x, p6y;
        p4x = t * (x1-x0) + x0;
        p4y = t * (y1-y0) + y0;
        p5x = (1-t) * (x1-x2) + x2;
        p5y = (1-t) * (y1-y2) + y2;
        
        p6x = t * (p5x-p4x) + p4x;
        p6y = t * (p5y-p4y) + p4y;
        retVal = p6y;
        
        return retVal;
    }

    float solveForBezier(float p0, float p1, float p2, float t)
    {
        float retVal = (1-t)*( (1-t)*p0 + t*p1 ) + t*( (1-t)*p1 + t*p2 );
        return retVal;
    }
    
    void mouseDrag(const juce::MouseEvent& e) override
    {
        mouseY = (float)e.y / (float)getHeight();
        mouseY = 1-mouseY;
        
        x1 = (float)e.x / (float)getWidth();
        y1 = (float)e.y / (float)getHeight();
        repaint();
    }

private:
    float x0, y0, x1, y1, x2, y2;
    float mouseY = 0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
