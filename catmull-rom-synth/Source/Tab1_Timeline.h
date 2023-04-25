#pragma once
#define tickResolution 10
#define projectLength 5000
#define numOfPitchZones 3
#include <math.h>
#include "global.h"
#include "background.h"
#include "spline.h"
#include "synthAudioSource.h"

using namespace juce;
//==============================================================================
class Tab1   :          public AudioAppComponent,
                        private Timer
{
public:
    Tab1(SynthAudioSource& sAS) : synthAudioSource(sAS)
    {
        setSize (1400, 600);
        setAudioChannels (0, 2);
        
        lookAndFeelFunctions();
        addAndMakeVisibleFunctions();
        setBoundsFunctions();

// lambda functions _________________________________________________________________
        startStopButton.onClick = [this] { startStopFunction(); };

        editSplinePitchButton.onClick = [this] {
            background.update(true);
            splinePitch.setVisible(true);
            splineWF.setVisible(false);
        };
        editSplineWFButton.onClick = [this] {
            background.update(false);
            splinePitch.setVisible(false);
            splineWF.setVisible(true);
        };

        clearAllSplinePointsButton.onClick = [this]
        {
            splinePitch.clearAllPoints();
            splineWF.clearAllPoints();
        };
        
        startTimer(100);
    }

    ~Tab1() override
    {
        shutdownAudio();
    }
        
    void startStopFunction()
    {
        if(startStopButton.getToggleState()){
            synthAudioSource.setMasterLevel(1);
            playheadComponent.startUpdating();
            playheadComponent.setVisible(true);
        }
        else{
            synthAudioSource.setMasterLevel(0);
            playheadComponent.stopUpdating();
            playheadComponent.setVisible(false);
        }
    }

    void lookAndFeelFunctions()
    {
        getLookAndFeel().setColour (TextButton::buttonColourId, Colour(0xff303030));
        getLookAndFeel().setColour (TextButton::buttonOnColourId, Colours::green);
        getLookAndFeel().setColour (TextButton::textColourOffId, Colours::white);
        getLookAndFeel().setColour (TextButton::textColourOnId, Colours::black);
    }

    void addAndMakeVisibleFunctions()
    {
        addAndMakeVisible(background);
        addAndMakeVisible(startStopButton);
        addAndMakeVisible(clearAllSplinePointsButton);
        addAndMakeVisible(editModesLabel);
        addAndMakeVisible(editSplinePitchButton);
        addAndMakeVisible(editSplineWFButton);
        addChildComponent(playheadComponent);
        addAndMakeVisible(splinePitch);
        addChildComponent(splineWF);
        addChildComponent(thumbnail);
    }

    void setBoundsFunctions()
    {
        editArea = getLocalBounds();
        background.setBounds(getBounds());
        background.defineEditArea();

        editModesLabel.setBounds(600, 2, 300, 50);
        editModesLabel.setText("Edit Mode", dontSendNotification);
        editModesLabel.setJustificationType(Justification::centredTop);
        editModesLabel.setColour(Label::backgroundColourId, Colour(0xFF302020));
        startStopButton.setBounds(10, 10, 100, 30);
        clearAllSplinePointsButton.setBounds(120, 10, 100, 30);
        startStopButton.setClickingTogglesState(true);
        editSplinePitchButton.setBounds(600, 20, 100, 30);
        editSplineWFButton.setBounds(700, 20, 100, 30);

        editSplinePitchButton.setRadioGroupId(editModesGroupId);
        editSplineWFButton.setRadioGroupId(editModesGroupId);
        editSplinePitchButton.setClickingTogglesState(true);
        editSplineWFButton.setClickingTogglesState(true);
        editSplinePitchButton.setToggleState(true, dontSendNotification);

        playheadComponent.setBounds(getBounds());
        playheadComponent.setEditZoneBounds(editArea);
                
        splinePitch.initialize(0.0f);   //  firstLastYValues
        splineWF.initialize(0.5f);      //  firstLastYValues

        splinePitch.setBounds(editArea);
        splineWF.setBounds(editArea);
        splinePitch.setBounds(editArea);
        splineWF.setBounds(editArea);

        thumbnail.setBounds(editArea.withTop(300));
    }

    void mouseDown (const MouseEvent& e) override {}
    void mouseUp (const MouseEvent& e) override   {}
    void mouseDrag (const MouseEvent& e) override {}
    void mouseMove (const MouseEvent& e) override{}
    void modifierKeysChanged(const ModifierKeys& m) override {}
    
//_________________________________________________________________________________________________________________
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override    {
        synthAudioSource.prepareToPlay (samplesPerBlockExpected, sampleRate);       }

    void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill) override  {
        synthAudioSource.getNextAudioBlock (bufferToFill);                              }

    void releaseResources() override            {
        synthAudioSource.releaseResources();    }
    
    void timerCallback() override
    {
        splinePitch.grabKeyboardFocus();
        stopTimer();
    }

    Spline splinePitch, splineWF;
    int editModesGroupId = 1;
    PlayHead playheadComponent;
    TextButton startStopButton {"Start/Stop"};
    TextButton clearAllSplinePointsButton {"Clear"};
    TextButton editSplinePitchButton {"Edit Pitch"};
    TextButton editSplineWFButton {"Edit Waveform"};
    Label editModesLabel;
    Background background;
    Thumbnail thumbnail;
    SynthAudioSource& synthAudioSource;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Tab1)
};
