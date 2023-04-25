#pragma once

int projectLengthInSamples;
int refWavetableSize = 1000;
juce::Rectangle<int> editArea;
enum PointValueName {pX, pY, pType, pNote, pInterpMode, pParam1, pParam2};
enum InterpModesEnum {iLinear, iExp, iNoInterp, iBezier, iCatmull, iSine, iBell, iSteps, iBrownian};
float pointsDiameter = 10, masterLevel = 0, currentSampleRate = 44100;
juce::AudioSampleBuffer renderedBuffer;
std::vector<std::vector<float>> modBuffers;

//==============================================================================
struct PointStruct
{
    float x, y, param1, param2;
    InterpModesEnum interpMode;
    PointStruct(const float& xArg, const float& yArg, const InterpModesEnum& interpModeArg, const float& param1Arg, const float& param2Arg)
    :   x(xArg), y(yArg), param1(param1Arg), param2(param2Arg), interpMode(interpModeArg)
    {}
};

bool pointXPosComparator(const PointStruct& p1, const PointStruct& p2)
{
    return (p1.x < p2.x);
}

std::function<void()> updateAudioSource;
