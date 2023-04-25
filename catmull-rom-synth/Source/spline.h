#pragma once
#include "interpolation.h"

using namespace juce;

class Spline : public Component
{
public:
    Spline(){    }

    void clearAllPoints()
    {
        points.clear();
        points.push_back( PointStruct(0, firstLastYValues, iCatmull, 0, 0) );   //  first point
        points.push_back( PointStruct(1, firstLastYValues, iCatmull, 0, 0) );   //  last point
        repaint();
        //updateAudioSource();
    }

//___________________________________________________________________________________________ paint functions
    void paint(Graphics& g)   override
    {
        paintPath(g);
        paintPoints(g);
    }
            
    void resized()   override
    {
        paintVector.clear();
        paintVector.resize(getWidth());
    }

    void paintPoints(Graphics& g)
    {
        for(PointStruct p : points)
        {
            g.setColour(Colours::red);
            g.fillEllipse(p.x * getWidth() - (float)(pointsDiameter/2),
                          (1-p.y) * getHeight()- (float)(pointsDiameter/2),
                          pointsDiameter,
                          pointsDiameter);
        }
    }
    
    void paintPath(Graphics& g)
    {
        if(points.size() <= 2)
            return;
        paintVector = *Interpolation::getInterpolation(points, getWidth());
        path.clear();
        int height = getHeight();
        path.startNewSubPath(0, (1-paintVector[0]) * height);
        for(int i=1; i<getWidth(); i++)
            path.lineTo(i, (1-paintVector[i]) * height);
        g.setColour(Colours::white);
        g.strokePath(path, PathStrokeType(2));
    }

//___________________________________________________________________________________________ add / move / remove functions
    void addFreePointOrFindIndexForPointToDrag(const MouseEvent& e)
    {
        int revY = getHeight() - e.y;
        mouseX = (float)e.x / (float)getWidth();
        mouseY = (float)revY / (float)getHeight();
        foundOverlapForPointToDrag = false;

// if click overlaps a point, find the index of that point and return ______________________________________________________
        for(int p=1; p<points.size()-1; p++)
        {
            float check = pow(pointsDiameter/2.0, 2.0);
            check -= (pow( points[p].x * (float)getWidth() - e.x, 2.0) +
                      pow( points[p].y * (float)getHeight() - revY, 2.0));
            if(check > 0)   //  does click overlap?
            {
                indexOfPointToMove = p;
                foundOverlapForPointToDrag = true;
                return;
            }
        }

// if click doesn't overlap a point, add a new point and set indexOfPointToMove ____________________________________________

        points.push_back( PointStruct(mouseX, mouseY+1, currentInterpMode, 0, 0));
        std::sort(points.begin(), points.end(), pointXPosComparator );// REORDER
        for(int i=1; i<points.size()-1; i++){
            if(points[i].y > 1.0f){
                foundOverlapForPointToDrag = true;
                points[i].y -= 1.0f;
                indexOfPointToMove = i;
                return;
            }
        }
    }

    void removePoint(const MouseEvent& e)
    {
        int revY = getHeight() - e.y;
        mouseX = (float)e.x / (float)getWidth();
        mouseY = (float)revY / (float)getHeight();
        for(int p=1; p<points.size()-1; p++)    //  does click overlap an existing point?
        {
            float check = pow(pointsDiameter/2.0, 2.0);
            check -= (pow( points[p].x * (float)getWidth() - e.x, 2.0) +
                      pow( points[p].y * (float)getHeight() - revY, 2.0));
            if(check > 0)   //  does click overlap?
            {
                points.erase(points.begin() + p);
                break;
            }
        }
    }

//__________________________________________________________________________________________ edit curvature
    void editCurvatureFunction(const MouseEvent& e)
    {
        float pParamVal = 0;
        int revY = getHeight() - e.y;
        mouseX = (float)e.x / (float)getWidth();
        mouseY = (float)revY / (float)getHeight();
        if(mouseX > 1)  mouseX = 1;
        if(mouseX < 0)  mouseX = 0;
        if(mouseY > 1)  mouseY = 1;
        if(mouseY < 0)  mouseY = 0;

        points[indexOfCurvatureSeg].interpMode = currentInterpMode;

        if(currentInterpMode == iExp)
        {
            if(points[indexOfCurvatureSeg+1].y > points[indexOfCurvatureSeg].y)
                mouseY = 1-mouseY;
            if(mouseY < 0.5)
                pParamVal = pow(mouseY, 1.05) / (0.5 - 0.0005) + 0.0005;
            else if(mouseY >= 0.5)
                pParamVal = pow(((mouseY - 0.5) * 2), 5) * 2000 + 1;
            points[indexOfCurvatureSeg].param1 = pParamVal;
        }

        if(currentInterpMode == iBezier)
        {
            float mouseYForBezier = (float)revY / (float)getHeight();   //  not limited to component bounds
            points[indexOfCurvatureSeg].param1 = mouseX;
            points[indexOfCurvatureSeg].param2 = mouseYForBezier;
        }
            
        repaint();
    }

//__________________________________________________________________________________________ mouse override functions
    void mouseDown(const MouseEvent& e)   override
    {
        int revY = getHeight() - e.y;
        mouseX = (float)e.x / (float)getWidth();
        mouseY = (float)revY / (float)getHeight();

        for(int i=0; i<points.size()-1; i++){       //  find indexOfCurvatureSeg
            if( e.x >= points[i].x*getWidth() && e.x <= points[i+1].x*getWidth() ){
                indexOfCurvatureSeg = i;
                break;
            }
        }

        if(editCurvature)
            editCurvatureFunction(e);
        else if(!cmdIsDown)
            addFreePointOrFindIndexForPointToDrag(e);
        else
            removePoint(e);
        repaint();
    }

    void mouseDrag(const MouseEvent& e)   override
    {
        //  move existing point
        if(foundOverlapForPointToDrag && !cmdIsDown)
        {
            int revY = getHeight() - e.y;
            mouseX = (float)e.x / (float)getWidth();
            mouseX = mouseX >=points[indexOfPointToMove+1].x ? points[indexOfPointToMove+1].x-0.002 : mouseX;
            mouseX = mouseX <=points[indexOfPointToMove-1].x ? points[indexOfPointToMove-1].x+0.002 : mouseX;
            mouseY = (float)revY / (float)getHeight();
            if(mouseY >= 1)                mouseY = 1;
            if(mouseY <= 0)                mouseY = 0;

            points[indexOfPointToMove].x = mouseX;
            points[indexOfPointToMove].y = mouseY;
            
            repaint();
        }

        //  edit curvature
        else if(editCurvature)
            editCurvatureFunction(e);
    }

    void mouseUp(const MouseEvent& e)   override
    {
        foundOverlapForPointToDrag = false;
        updateAudioSource();
    }
    
//_________________________________________________________________________________________ key change functions
    bool keyStateChanged(const bool keyIsDown)  override
    {
// {iLinear, iExp, iNoInterp, iBezier, iCatmull, iSine, iBell, iSteps, iBrownian};
        editCurvature = true;
        
        if(KeyPress::isKeyCurrentlyDown('1'))                 currentInterpMode = iLinear;

        else if(KeyPress::isKeyCurrentlyDown('2'))            currentInterpMode = iCatmull;

        else if(KeyPress::isKeyCurrentlyDown('3'))            currentInterpMode = iNoInterp;

        else if(KeyPress::isKeyCurrentlyDown('4'))            currentInterpMode = iBezier;
        
        else if(KeyPress::isKeyCurrentlyDown('5'))            currentInterpMode = iExp;
        
        else if(KeyPress::isKeyCurrentlyDown('6'))            currentInterpMode = iNoInterp;
        
        else if(KeyPress::isKeyCurrentlyDown('7'))            currentInterpMode = iNoInterp;
        
        else                                                        editCurvature = false;

        return 1;
    }

    void modifierKeysChanged(const ModifierKeys& m) override
    {
        cmdIsDown = m.isCommandDown();
    }

    void initialize(float firstLastYValuesArg){
        firstLastYValues = firstLastYValuesArg;
        setWantsKeyboardFocus(true);
        clearAllPoints();
    }

    std::vector<PointStruct> points;
private:
    float mouseX, mouseY, mouseXDownBeg, mouseXDownEnd, firstLastYValues;
    int indexOfPointToMove, indexOfCurvatureSeg;
    bool foundOverlapForPointToDrag = false, editCurvature = false, cmdIsDown = false;
    Path path;
    enum keyPressEnum {noKeyPressed, numKey1, numKey2};
    keyPressEnum currentlyPressedKey;
    InterpModesEnum currentInterpMode = iCatmull;
    std::vector<float> paintVector;
};

