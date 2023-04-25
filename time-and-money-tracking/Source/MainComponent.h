#pragma once
#include <JuceHeader.h>
#include "tab1.h"
#include "tab2.h"

//==============================================================================
class MainComponent  : public juce::Component
                      
{
public:
    MainComponent()
    {
        setSize(600, 600);
        
        addChildComponent(tab1);
        tab1.setBounds(getBounds());

        addChildComponent(tab2);
        tab2.setBounds(getBounds());
        tab1.setVisible(true);

        addAndMakeVisible(tab1Button);
        addAndMakeVisible(tab2Button);
        tab1Button.setBounds(270, 10, 45, 20);
        tab2Button.setBounds(320, 10, 45, 20);
        tab1Button.setClickingTogglesState(true);
        tab2Button.setClickingTogglesState(true);
        tab1Button.setToggleState(true, juce::dontSendNotification);
        tab1Button.setRadioGroupId(tabsGroup);
        tab2Button.setRadioGroupId(tabsGroup);
        
        tab1Button.onClick = [this] { switchTab(); };
        tab2Button.onClick = [this] {
            switchTab();
            tab2.updateToggleActivityButtons();
        };
    }

    ~MainComponent()
    {}
    
private:
    void switchTab()
    {
        tab1.setVisible(tab1Button.getToggleState());
        tab2.setVisible(tab2Button.getToggleState());
    }
    
    Tab1 tab1;
    Tab2 tab2;
    juce::TextButton tab1Button{"tab1"}, tab2Button{"tab2"};
    enum RadioGroupIdEnum { tabsGroup = 100};
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};

