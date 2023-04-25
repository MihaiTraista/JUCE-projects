#pragma once
#include "bufferCompiler.h"
#include "dspProcess.h"

using namespace juce;

class SynthAudioSource   : public AudioSource,
                           public Time
{
public:
    SynthAudioSource(){}

    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override
    {
        currentSampleRate = sampleRate;
        projectLengthInSamples = (float)projectLength * (currentSampleRate/1000.0);
        renderedBuffer.setSize(1, projectLengthInSamples);
        dsp::ProcessSpec spec{sampleRate, (uint32)samplesPerBlockExpected, 2};
        dspProcess.prepare(spec);
    }

    void releaseResources() override {
        dspProcess.reset();
    }
    
    void setMasterLevel(float lev)
    {
        masterLevel = lev;
        renderedBufferReader = 0;
    }
    
    void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill) override
    {
        bufferToFill.clearActiveBufferRegion();
        auto* leftBuffer  = bufferToFill.buffer->getWritePointer (0, bufferToFill.startSample);
        auto* rightBuffer = bufferToFill.buffer->getWritePointer (1, bufferToFill.startSample);

        for (auto sample = 0; sample < bufferToFill.numSamples; ++sample)
        {
            leftBuffer[sample] = renderedBuffer.getSample(0, renderedBufferReader) * masterLevel;
            rightBuffer[sample] = renderedBuffer.getSample(0, renderedBufferReader) * masterLevel;

            renderedBufferReader++;
            if(renderedBufferReader >= projectLengthInSamples)
                renderedBufferReader = 0;
        }

        dsp::AudioBlock<float> block (*bufferToFill.buffer,
                                 (size_t) bufferToFill.startSample);

//        ScopedLock audioLock (audioCallbackLock);
        dspProcess.process( dsp::ProcessContextReplacing<float> (block) );
    }

    DSPProcess dspProcess;
private:
    int renderedBufferReader = 0;
    std::vector<float> pitchVec, wFVec;
};

