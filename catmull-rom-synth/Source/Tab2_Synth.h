#pragma once
#include "dspProcess.h"
#include "bufferCompiler.h"
#include "spline.h"

class Tab2 : public Component, private ChangeListener
{
public:
    Tab2(SynthAudioSource& sAS, BufferCompiler& bC, Spline& splinePitchArg, Spline& splineWFArg) : synthAudioSource(sAS), bufferCompiler(bC), splinePitch(splinePitchArg), splineWF(splineWFArg)
    {
        setSize (1400, 600);

        std::vector<ParameterBase*> parameters = synthAudioSource.dspProcess.parameters;
        parameters.insert(parameters.end(), bufferCompiler.parameters.begin(), bufferCompiler.parameters.end());
        
        parametersComponent.reset( new ParametersComponent(parameters) );
        for( auto* p : *parametersComponent->getParameters() )
            p->addChangeListener(this);

        addAndMakeVisible(*parametersComponent);

        parametersComponent->setBounds(getBounds());
        parametersComponent->setBounds(50, 100, 700, parametersComponent->getHeightNeeded());
    }

private:
    void changeListenerCallback (ChangeBroadcaster* broadcaster) override
    {
        synthAudioSource.dspProcess.updateParameters();
        bufferCompiler.compileRenderedBuffer(&(splinePitch.points), &(splineWF.points));
    }

    SynthAudioSource& synthAudioSource;
    BufferCompiler& bufferCompiler;
    Spline& splinePitch;
    Spline& splineWF;
    std::unique_ptr<ParametersComponent> parametersComponent;
};
