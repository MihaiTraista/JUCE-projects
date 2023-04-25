#pragma once
#include "interpolation.h"
#include "modBufferCompiler.h"
#include "parameters.h"
#include "global.h"

struct BufferCompiler
{
    void compileRenderedBuffer(std::vector<PointStruct>* pitchPoints, std::vector<PointStruct>* wFPoints)
    {
//        if(masterLevel > 0)     //  DO NOT RE-RENDER WHILE PLAYING !!!!!!!!!!!!!
//            return;
        
        int64 startTime = juce::Time::currentTimeMillis();
        double phasorValue = 0, phasorDelta = 0;
        int wFSize = refWavetableSize;
        std::vector<float> pitchVec, referenceWavetable;
        std::vector<PointStruct> localwfPoints = *wFPoints;

        pitchVec = *Interpolation::getInterpolation(*pitchPoints, projectLengthInSamples);
        referenceWavetable = *Interpolation::getInterpolation(*wFPoints, wFSize);

        double frequency, val;
        int wFIndex, numCyclesInProject=0;
        
        for(int s=0; s<projectLengthInSamples; s++)
        {
            frequency = pitchVec[s] * 12.0f + 36.0f;
            frequency = 440.0f * std::pow (2.0, (frequency - 69.0f) / 12.0f);

            phasorDelta = frequency / currentSampleRate;

            phasorValue += phasorDelta;
            if(phasorValue >= 1.0){
                phasorValue -= 1.0;
                numCyclesInProject++;
            }
        }

        //ModBuffersCompiler::compileRandom(wFPoints, numCyclesInProject);
        std::vector<float> ratios = {0.5, 0.5, 0.5};
        ModBuffersCompiler::compileShadow(wFPoints, numCyclesInProject, referenceWavetable, ratios);

        int cycleIterator = 0;
        std::cout << "wet " << wetParam.getCurrentValue() << std::endl;
        for(int s=0; s<projectLengthInSamples; s++)
        {
            frequency = pitchVec[s] * 12.0f + 36.0f;
            frequency = 440.0f * std::pow (2.0, (frequency - 69.0f) / 12.0f);

            phasorDelta = frequency / currentSampleRate;

            phasorValue += phasorDelta;
            if(phasorValue >= 1.0){
                phasorValue -= 1.0;
                for(int p=1; p < wFPoints->size()-1; p++){
                    localwfPoints[p].y = (*wFPoints)[p].y + (modBuffers[p-1][cycleIterator] * 2.0f - 1.0f) * wetParam.getCurrentValue();
                    if(localwfPoints[p].y > 1)          localwfPoints[p].y = 1;
                    else if(localwfPoints[p].y < 0)     localwfPoints[p].y = 0;
                }
                
                referenceWavetable = *Interpolation::getInterpolation(localwfPoints, wFSize);
                cycleIterator++;
            }

            wFIndex = (float)(phasorValue * (float)(wFSize-1) );
            val = referenceWavetable[wFIndex] - 0.5f;

            renderedBuffer.setSample(0, s, val);
        }
        
        std::cout << "compile time " << (juce::Time::currentTimeMillis() - startTime) << " ms\n";
    }

    SliderParameter wetParam { {0.0f, 1.0f}, 1, 0.5, "Dry/Wet"};
    std::vector<ParameterBase*> parameters { &wetParam };
};

