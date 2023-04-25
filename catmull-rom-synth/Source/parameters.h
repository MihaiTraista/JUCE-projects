#pragma once

using namespace juce;

struct ParameterBase    : public ChangeBroadcaster
{
    ParameterBase (const String& labelName) : name (labelName) {}
    virtual ~ParameterBase() {}

    virtual Component* getComponent() = 0;

    virtual int getPreferredHeight()  = 0;
    virtual int getPreferredWidth()   = 0;

    String name;
};

struct SliderParameter   : public ParameterBase
{
    SliderParameter (Range<double> range, double skew, double initialValue,
                     const String& labelName, const String& suffix = {})
        : ParameterBase (labelName)
    {
        slider.setRange (range.getStart(), range.getEnd(), 0.01);
        slider.setSkewFactor (skew);
        slider.setValue (initialValue);

        if (suffix.isNotEmpty())
            slider.setTextValueSuffix (suffix);

        slider.onValueChange = [this] { sendChangeMessage(); };
    }

    Component* getComponent() override    { return &slider; }

    int getPreferredHeight() override     { return 40; }
    int getPreferredWidth()  override     { return 500; }

    double getCurrentValue() const        { return slider.getValue(); }

private:
    Slider slider;
};

//==============================================================================
struct ChoiceParameter    : public ParameterBase
{
    ChoiceParameter (const StringArray& options, int initialId, const String& labelName)
        : ParameterBase (labelName)
    {
        parameterBox.addItemList (options, 1);
        parameterBox.onChange = [this] { sendChangeMessage(); };

        parameterBox.setSelectedId (initialId);
    }

    Component* getComponent() override    { return &parameterBox; }

    int getPreferredHeight() override     { return 25; }
    int getPreferredWidth()  override     { return 250; }

    int getCurrentSelectedID() const      { return parameterBox.getSelectedId(); }

private:
    ComboBox parameterBox;
};

class ParametersComponent    : public Component
{
public:
    ParametersComponent (std::vector<ParameterBase*>& paramsToUse)
        : parameters(paramsToUse)
    {
        for (auto demoParameter : parameters)
        {
            addAndMakeVisible (demoParameter->getComponent());

            auto* paramLabel = new Label ({}, demoParameter->name);

            paramLabel->attachToComponent (demoParameter->getComponent(), true);
            paramLabel->setJustificationType (Justification::centredLeft);
            addAndMakeVisible (paramLabel);
            labels.add (paramLabel);
        }
    }

    void resized() override
    {
        auto bounds = getLocalBounds();

        for (auto* p : parameters)
        {
            auto* comp = p->getComponent();

            comp->setSize (jmin (bounds.getWidth(), p->getPreferredWidth()), p->getPreferredHeight());

            auto compBounds = bounds.removeFromTop (p->getPreferredHeight());
            comp->setCentrePosition (compBounds.getCentre());
        }
    }

    int getHeightNeeded()
    {
        std::cout << "size " << parameters.size() << std::endl;
        auto height = 0;

        for (auto* p : parameters)
            height += p->getPreferredHeight();

        return height + 10;
    }
    
    std::vector<ParameterBase*>* getParameters()
    {
        return &parameters;
    }

private:
    std::vector<ParameterBase*>& parameters;
    OwnedArray<Label> labels;
};
