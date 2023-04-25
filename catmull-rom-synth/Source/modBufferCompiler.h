#pragma once
#include "global.h"

using namespace juce;

struct ModBuffersCompiler
{
    static void compileRandom(std::vector<PointStruct>* wFPoints, const int& modBufferLength)
    {
        std::vector<PointStruct>& points = *wFPoints;

        if(points.size() <= 2){
            modBuffers.clear();
            return;
        }

        modBuffers.clear();
        int numActivePoints = 0;

        for(int p=1; p<points.size()-1; p++) {
            modBuffers.push_back(std::vector<float>(modBufferLength));
            numActivePoints++;
        }

        juce::Random rand;
        for(int p=0; p<numActivePoints; p++)
        {
            rand.setSeed(juce::Time::currentTimeMillis());
            for(int s=0; s<modBufferLength; s++)
                modBuffers[p][s] = rand.nextFloat();
        }
    }

    static void compileShadow(std::vector<PointStruct>* wFPoints, const int& modBufferLength, std::vector<float>& refWavetable, std::vector<float>& ratios)
    {
        std::vector<PointStruct>& points = *wFPoints;

        if(points.size() <= 2){
            modBuffers.clear();
            return;
        }

        modBuffers.clear();
        int numActivePoints = 0;
        for(int p=1; p<points.size()-1; p++) {
            modBuffers.push_back(std::vector<float>(modBufferLength));
            numActivePoints++;
        }

        float position, lastPhase=0, constPhase=0;
        Random rand(Time::currentTimeMillis());
        ratios.clear();

        for(int p=0; p<numActivePoints; p++)
            ratios.push_back(rand.nextFloat() * 1 + 1);
        
        for(int p=0; p<numActivePoints; p++)
        {
            constPhase = ratios[p] * 1000;
            constPhase = ( (int)constPhase % 1000 ) / 1000.0f;
            constPhase = constPhase * (float)refWavetableSize;
            
            for(int s=0; s<modBufferLength; s++){
                position = (*wFPoints)[p+1].x * ratios[p] * 1000;
                position = ( (int)position % 1000 ) / 1000.0f;
                position = position * (float)refWavetableSize;
                position += lastPhase;
                position = (int)position % refWavetableSize;

//                if(s < 5)
//                    std::cout << "p " << p << ", s " << s << ", position " << position << ", lastPhase " << lastPhase << ", refWavetable[ position ] " << refWavetable[ position ] << std::endl;
                
                lastPhase += constPhase;
                lastPhase = (int)lastPhase % refWavetableSize;

                modBuffers[p][s] = refWavetable[ position ];
            }
        }
    }
};
