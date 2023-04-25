#pragma once
#include "parameters.h"
using namespace juce;

struct DSPProcess
{
    void prepare(const dsp::ProcessSpec& specArg)
    {
        spec = specArg;
        sampleRate = spec.sampleRate;
        dsp_gain.setGainDecibels (-20.0f);
        dsp_iir.state = dsp::IIR::Coefficients<float>::makeLowPass (spec.sampleRate, 20000, 0.01);
        dsp_iir.prepare (spec);
        dsp_oversampler.initProcessing (spec.maximumBlockSize);
    }
    
    void process(dsp::ProcessContextReplacing<float> context )
    {
        dsp_gain.process (context);
        dsp_iir.process (context);

        if(oversampleChoiceParam.getCurrentSelectedID() > 1)
        {
            const auto& inputBlock = context.getInputBlock();
            auto ovBlock = dsp_oversampler.processSamplesUp (inputBlock);

            dsp::ProcessContextReplacing<float> newContext (ovBlock);

            //dsp_iir.process (newContext);

            auto& outputBlock = context.getOutputBlock();
            dsp_oversampler.processSamplesDown (outputBlock);
        }
    }
    
    void reset()
    {
        dsp_gain.reset();
        dsp_iir.reset();
        dsp_oversampler.reset();
    }
    
    void updateParameters()
    {
        dsp_gain.setGainLinear( static_cast<float> (gainParam.getCurrentValue()) );
        auto freq = static_cast<float> (cutoffParam.getCurrentValue());
        auto q = static_cast<float> (qFactorParam.getCurrentValue());
        switch (filterTypeParam.getCurrentSelectedID())
        {
            case 1: *dsp_iir.state = *dsp::IIR::Coefficients<float>::makeLowPass (spec.sampleRate, freq, q);  break;
            case 2: *dsp_iir.state = *dsp::IIR::Coefficients<float>::makeHighPass (spec.sampleRate, freq, q); break;
            case 3: *dsp_iir.state = *dsp::IIR::Coefficients<float>::makeBandPass (spec.sampleRate, freq, q); break;
        }

        dsp_oversampler.reset();
        dsp_oversampler.clearOversamplingStages();

        switch (oversampleChoiceParam.getCurrentSelectedID())
        {
            case 1: dsp_oversampler.addDummyOversamplingStage();
                break;

            case 2: dsp_oversampler.addOversamplingStage (dsp::Oversampling<float>::filterHalfBandPolyphaseIIR,
                                                          0.01, -100, 0.01, -100);
                break;
            case 3: dsp_oversampler.addOversamplingStage (dsp::Oversampling<float>::filterHalfBandFIREquiripple,
                                                          0.1, -20, 0.1, -20);
                break;
        }
 
        dsp_oversampler.initProcessing (spec.maximumBlockSize);
    }

    dsp::Gain<float> dsp_gain;
    dsp::ProcessorDuplicator<dsp::IIR::Filter<float>, dsp::IIR::Coefficients<float>> dsp_iir;
    dsp::Oversampling<float> dsp_oversampler { 2, 2, dsp::Oversampling<float>::filterHalfBandPolyphaseIIR, true, false };
    int sampleRate;
    dsp::ProcessSpec spec;
 
    SliderParameter gainParam{ {0.0f, 1.0f}, 1, 0.5, "Gain"};
    ChoiceParameter oversampleChoiceParam{{"No Oversampling", "PolyphaseIIR", "FIREquiripple"}, 1, "Oversample Choice"};
    ChoiceParameter filterTypeParam{{"LowPass", "HighPass", "BandPass"}, 1, "Filter Type"};
    SliderParameter cutoffParam{ {20, 20000}, 1, 19500, "Cutoff Freq", "hz" };
    SliderParameter qFactorParam{ {0.01f, 50.0f}, 1, 0.01f, "Q Factor"};

    std::vector<ParameterBase*> parameters { &gainParam, &oversampleChoiceParam, &filterTypeParam, &cutoffParam, &qFactorParam };
};

