#pragma once
#include "global.h"
#include "bufferCompiler.h"
#include "Tab1_Timeline.h"
#include "Tab2_Synth.h"

using namespace juce;
//==============================================================================
class MainComponent   : public Component
{
public:
    MainComponent()
    {
        setSize (1400, 600);

        tab1.reset( new Tab1(synthAudioSource) );
        tab2.reset( new Tab2(synthAudioSource, bufferCompiler, tab1->splinePitch, tab1->splineWF) );

        addAndMakeVisible(*tab1);
        addChildComponent(*tab2);

        addAndMakeVisible(tab1Switch);
        addAndMakeVisible(tab2Switch);
        tab1Switch.setBounds(1200, 0, 100, 50);
        tab2Switch.setBounds(1300, 0, 100, 50);
        tab1Switch.setClickingTogglesState(true);
        tab2Switch.setClickingTogglesState(true);
        tab1Switch.setToggleState(true, dontSendNotification);
        tab1Switch.setRadioGroupId(tabsRadioGroupID);
        tab2Switch.setRadioGroupId(tabsRadioGroupID);

        tab1->setBounds(getBounds());
        tab2->setBounds(getBounds());


        tab1Switch.onClick = [&] {
            tab1->setVisible(tab1Switch.getToggleState());
            tab2->setVisible(tab2Switch.getToggleState());
        };
        tab2Switch.onClick = [&] {
            tab1->setVisible(tab1Switch.getToggleState());
            tab2->setVisible(tab2Switch.getToggleState());
        };

        updateAudioSource = [this]
        {
            bufferCompiler.compileRenderedBuffer(&(tab1->splinePitch.points), &(tab1->splineWF.points));
        };
        
    }
    ~MainComponent() override
    {
    }

    bool keyPressed(const KeyPress &key)    override
    {
        if(key == KeyPress::spaceKey)
        {
            tab1->startStopButton.setToggleState(!tab1->startStopButton.getToggleState(), dontSendNotification);
            tab1->startStopFunction();
        }
        return 1;
    }

    BufferCompiler bufferCompiler;
    TextButton tab1Switch {"Tab1"}, tab2Switch{"Tab2"};
    enum RadioGroupIds { tabsRadioGroupID = 100 };
    SynthAudioSource synthAudioSource;
    std::unique_ptr<Tab1> tab1;
    std::unique_ptr<Tab2> tab2;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
