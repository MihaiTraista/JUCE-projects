#pragma once

#include "global.h"

//==============================================================================
struct Interpolation
{
    static std::vector<float> bezierBuffer;
    static std::vector<float> interpolated;
    static std::vector<PointStruct> points;

    static std::vector<float>* getInterpolation(const std::vector<PointStruct>& pointsArg, int numValues)
    {
        points = pointsArg;
        interpolated.resize(numValues);
        
        if(points.size() <= 2)
        {
            for(float& f : interpolated)
                f = points[0].y;
            return &interpolated;
        }
        
        int segLen, loc=0;
        float segProgress;
        float dx[3] = {0, 0, 0};            //  x delta
        float dy[3] = {0, 0, 0};            //  y delta
        float yVals[4] = {0, 0, 0, 0};      //  y values of the four points for catmull
        float catP[4];                      //  final y values for catmull

        for(int p=0; p<points.size()-1; p++)
        {
            segLen = round((float)( points[p+1].x * (float)numValues - points[p].x * (float)numValues ));

            if(points[p].interpMode == iLinear){            //      iLinear
                for(int i=0; i<segLen; i++)
                {
                    segProgress = (float)i / (float)segLen;
                    interpolated[i+loc] = segProgress * (points[p+1].y - points[p].y) + points[p].y;
                }
            }

            else if(points[p].interpMode == iNoInterp){    //      iNoInterp
                for(int i=0; i<segLen; i++)
                {
                    segProgress = (float)i / (float)segLen;
                    interpolated[i+loc] = points[p].y;
                }
            }

            else if(points[p].interpMode == iBezier){       //      iBezier
                fillBezierBuffer(segLen, p);
                for(int i=0; i<segLen; i++)
                    interpolated[i+loc] = bezierBuffer[i];
            }
            
            else if(points[p].interpMode == iCatmull){      //      iCatmull
                {
                    std::vector<PointStruct> pts;

                    if(p != 0 && p != (points.size() - 2))  //  mid segment
                        pts = {points[p-1], points[p], points[p+1], points[p+2]};
                    else if(p == 0){                        //  first segment
                        pts = {points[points.size()-2], points[p], points[p+1], points[p+2]};
                        pts[0].x -= 1;
                    }
                    else{                                   //  last segment
                        pts = {points[p-1], points[p], points[p+1], points[1]};
                        pts[3].x += 1;
                    }
                    
                    dx[0] = pts[1].x - pts[0].x;                //    post(dx[0]);
                    dy[0] = pts[1].y - pts[0].y;                //    post(dy[0]);
                    dx[1] = pts[2].x - pts[1].x;
                    dy[1] = pts[2].y - pts[1].y;
                    dx[2] = pts[3].x - pts[2].x;                //    post(dx[2]);
                    dy[2] = pts[3].y - pts[2].y;                //    post(dy[2]);
 
/*
                    if(dy[0] != 0)
                        yVals[0] = pts[1].y - dy[0] * (dx[1] / dx[0]);
                    else
                        yVals[0] = pts[1].y;
                    if(dy[2] != 0)
                        yVals[3] = dy[2] * (dx[1] / dx[2]) + pts[1].y;
                    else
                        yVals[3] = pts[1].y;
*/
                    yVals[0] = pts[1].y - dy[0] * (dx[1] / dx[0]);
                    yVals[1] = pts[1].y;
                    yVals[2] = pts[2].y;
                    yVals[3] = pts[2].y + dy[2] * (dx[1] / dx[2]);

                    catP[0] = (-0.5*yVals[0])+(3./2.*yVals[1])-(3./2.*yVals[2])+(0.5*yVals[3]);
                    catP[1] = (yVals[0]-(5./2.*yVals[1])+(2.*yVals[2])-(0.5*yVals[3]));
                    catP[2] = (-0.5*yVals[0])+(0.5*yVals[2]);
                    catP[3] = yVals[1];
                }   //  calculate catmull points

                for(int i=0; i<segLen; i++)
                {
                    segProgress = (float)i / (float)segLen;
                    interpolated[i+loc] = catP[0] * pow(segProgress, 3) + catP[1] * pow(segProgress, 2) + catP[2] * segProgress + catP[3];
                    if( interpolated[i+loc] > 1 )       interpolated[i+loc] = 1;
                    else if( interpolated[i+loc] < 0 )  interpolated[i+loc] = 0;
                }
            }

            loc += segLen;
        }

        for(int i=loc; i<numValues; i++)
            interpolated[interpolated.size()-1] = interpolated[0];

        return &interpolated;
    }

    static void fillBezierBuffer(int numOfItemsInArray, int pointIndex)
    {
        float p0[2];
        float p1[2];
        float p2[2];
        float t, x, y, scaledX;
        
        p0[0] = points[pointIndex].x;
        p0[1] = points[pointIndex].y;
        p1[0] = points[pointIndex].param1;
        p1[1] = points[pointIndex].param2;
        p2[0] = points[pointIndex+1].x;
        p2[1] = points[pointIndex+1].y;
        
        bezierBuffer.clear();
        bezierBuffer.resize(numOfItemsInArray);
        std::fill (bezierBuffer.begin(), bezierBuffer.end(), 44);

        for(int i=0; i<numOfItemsInArray; i++)
        {
            t = (float)i / (float)numOfItemsInArray;
            x = (1-t)*( (1-t)*p0[0] + t*p1[0] ) + t*( (1-t)*p1[0] + t*p2[0] );
            y = (1-t)*( (1-t)*p0[1] + t*p1[1] ) + t*( (1-t)*p1[1] + t*p2[1] );
            if(x > 1)   x=1;
            if(x < 0)   x=0;
            if(y > 1)   y=1;
            if(y < 0)   y=0;
            scaledX = (x - p0[0]) / (p2[0] - p0[0]);
            scaledX = floor(scaledX * (float)numOfItemsInArray);
            if(scaledX < 0)     scaledX = 0;
            if(scaledX > numOfItemsInArray)     scaledX = numOfItemsInArray;

            bezierBuffer[(int)scaledX] = y;
        }
        for(int i=1; i<numOfItemsInArray; i++)
        {
            if(bezierBuffer[i] == 44)
                bezierBuffer[i] = bezierBuffer[i-1];
        }

        float fillVal;
        if(bezierBuffer[0] == 44)
        {
            int pp=1;
            while(true)
            {
                if(bezierBuffer[pp] != 44){
                    fillVal = bezierBuffer[pp];
                    break;
                }
                else
                    pp++;
            }
            for(int i=0; i<numOfItemsInArray; i++){
                if(bezierBuffer[i])
                    bezierBuffer[i] = fillVal;
                else
                    break;
            }
        }
    }
};
std::vector<float> Interpolation::bezierBuffer;
std::vector<float> Interpolation::interpolated;
std::vector<PointStruct> Interpolation::points;
