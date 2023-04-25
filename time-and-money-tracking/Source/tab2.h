#pragma once
#include "global.h"

juce::Time begShowDate;
juce::Time endShowDate;
int numDays = 0;
juce::Colour colourArray[10] = {juce::Colours::red, juce::Colours::orange, juce::Colours::yellow, juce::Colours::green, juce::Colours::blueviolet, juce::Colours::azure, juce::Colours::cyan, juce::Colours::grey, juce::Colours::gold, juce::Colours::indigo};

std::function<void()> updatePickers;

juce::String getDayString(const juce::Time& t)
{
    juce::String monthNames[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
    return (juce::String(t.getDayOfMonth()) + monthNames[t.getMonth()]);
}
class Graph     :   public juce::Component
{
public:
    float yTab=0, xTab=0, xDragFloat=0, yDragFloat=0;
    int shiftDay=0, xDragDelta=0, xOnClick=0, betweenClicksXDelta=0, yDragDelta=0, yOnClick=0, betweenClicksYDelta=0, maxMinsADay=0, addNumDay=0;

    Graph()
    {
        addAndMakeVisible(bars);
        addAndMakeVisible(frame);
    }

    void updateVisibleActivities(std::vector<int> arg)
    {
        if(activities.size() == 0)
            return;
        visibleActivities = arg;
        bars.updateVisibleActivities(arg);
        findMaxMinsADay();
        bars.update(xDragFloat, yDragFloat, maxMinsADay);
        frame.update(xDragFloat, yDragFloat, maxMinsADay);
    }
    
    void mouseDown(const juce::MouseEvent& e) override
    {
        xOnClick = e.x;
        yOnClick = e.y;
    }
    void mouseUp(const juce::MouseEvent& e) override
    {
        betweenClicksXDelta += e.x - xOnClick;
        betweenClicksYDelta += e.y - yOnClick;
    }

    void mouseDrag(const juce::MouseEvent& e) override
    {
        if(activities.size() == 0)
            return;
        xDragDelta = (e.x - xOnClick + betweenClicksXDelta) * -1;
        xDragFloat = (float)xDragDelta / 40.0f;
        shiftDay = xDragFloat>0 ? (int)xDragFloat : (int)xDragFloat-1;
        xDragFloat = shiftDay - xDragFloat;

        yDragDelta = (e.y - yOnClick + betweenClicksYDelta) * -1;
        yDragFloat = (float)yDragDelta / 40.0f;
        addNumDay = yDragFloat>0 ? (int)yDragFloat : (int)yDragFloat-1;
        yDragFloat = addNumDay - yDragFloat;

        if(yDragDelta < 0){
            addNumDay = 0;
            yDragDelta = 0;
            yDragFloat = 0;
            betweenClicksYDelta = 0;
        }
        begShowDate = juce::Time::getCurrentTime() - juce::RelativeTime( 60*60*24*60 ) + juce::RelativeTime( 60*60*24*shiftDay );
        endShowDate = juce::Time::getCurrentTime() + juce::RelativeTime( 60*60*24*(shiftDay+addNumDay) );
        //std::cout << xDragFloat << ", " << shiftDay << ", " << begShowDate.toString(true, false) << std::endl;
        //std::cout << yDragFloat << ", " << addNumDay << ", " << begShowDate.toString(true, false) << std::endl;
        
        juce::RelativeTime diffInDays = endShowDate - begShowDate;
        numDays = diffInDays.inDays();
            //endShowDate.getDayOfYear() - begShowDate.getDayOfYear() + 1;

        findMaxMinsADay();
        bars.update(xDragFloat, yDragFloat, maxMinsADay);
        frame.update(xDragFloat, yDragFloat, maxMinsADay);
        updatePickers();
    }

    void findMaxMinsADay()
    {
        maxMinsADay = 0;
        int addAll;
        for(int d=0; d<numDays+2; d++) // find maxMinsADay
        {
            addAll = 0;
            for(int a=0; a<visibleActivities.size(); a++)
            {
//                for(int dd=0; dd<activities[visibleActivities[a]].days.size(); dd++)
                for( Day dd : activities[visibleActivities[a]].days )
                {
                    if(t1_SameDay_t2( dd.date, begShowDate+juce::RelativeTime(60*60*24*d)) )
                        addAll += dd.timeSpentThisDay.inMinutes();
                }
            }
            //std::cout << "d.date " << d.date.toString(1, 0) << ", begShowDate " << begShowDate.toString(1, 0) << ", endShowDate " << endShowDate.toString(1, 0) << std::endl;
            if(addAll > maxMinsADay)
                maxMinsADay = addAll;
        }
    }
    void paint(juce::Graphics& g) override
    {
        g.fillAll( juce::Colour(0x20000000) );
//        g.setColour(juce::Colours::white);
//        g.drawRect( (int)xTab, 0, (int)yTab, (int)gWidth, 2 );
    }
    
    void resized()   override
    {
        yTab = getHeight() - (getHeight()/40);
        xTab = getWidth()/15;
        bars.setBounds(xTab, 0, getWidth() - xTab, getHeight());
        frame.setBounds(0, 0, getWidth(), yTab);
        bars.setYTab(yTab);
        frame.setXTab(xTab);
    }

private:
    class Bars  :   public juce::Component
    {
    public:
        float xDragFloat=0, yDragFloat=0;
        int maxMinsADay=0, yTab=0;
        std::vector<int> visibleActivities;

        Bars()
        {
            setInterceptsMouseClicks(false, false);
        }

        void updateVisibleActivities(std::vector<int> arg){ visibleActivities = arg; }

        void update(float xDragFloatArg, float yDragFloatArg, int maxMinsADayArg)
        {
            xDragFloat = xDragFloatArg;
            yDragFloat = yDragFloatArg;
            maxMinsADay = maxMinsADayArg;
            repaint();
        }

        void setYTab(int yT){yTab = yT;}
        
        void paint(juce::Graphics& g) override
        {
            if(numDays == 0 || activities.size() == 0)
                return;
            g.fillAll(juce::Colour(0x10000000));
            float w = (float)getWidth() / ((float)numDays - yDragFloat);
            float spacer = (float)getWidth() / ((float)numDays - yDragFloat) / 10.0f;
            juce::Rectangle<int> rect(0, 0, w-spacer, getHeight());
            juce::Time iteratorDate = begShowDate;
            float mins;
            int skip = 1;
            int yShifter;
            if(w<30)            skip = 3;
            else if(w<40)       skip = 2;

            for(int i=0; i<numDays+2; i++)
            {
                yShifter = 0;
                for(int a=0; a<visibleActivities.size(); a++)
                {
                    mins = 0;
                    for(Day d : activities[visibleActivities[a]].days)
                    {
                        if( t1_SameDay_t2(iteratorDate, d.date) )
                            mins = d.timeSpentThisDay.inMinutes();
                    }
                    mins /= maxMinsADay;
                    rect.setX( (float)i * w + xDragFloat*w);
                    rect.setY((1.0 - mins) * yTab - yShifter);
                    yShifter += mins * yTab;
                    rect.setHeight( yTab - ((1.0 - mins) * yTab) );
                    g.setColour(colourArray[visibleActivities[a]]);
                    g.fillRect(rect);
                }

//                std::cout << (int)((float)i * w + xDragFloat*w) % (int)(w*skip) << ", " << w*skip << std::endl;
//                if( (int)((float)i * w + xDragFloat*w) < (int)((float)i * w + xDragFloat*w) % (int)(w*skip) )
                g.setColour(juce::Colours::white);
                if(iteratorDate.getDayOfWeek() == 1)
                    g.drawText( getDayString(iteratorDate), (float)i * w + xDragFloat*w, yTab+3, w*7, 10, juce::Justification::bottomLeft, true );
                iteratorDate += juce::RelativeTime(60*60*24);
            }
        }
    };
    
    class Frame  :   public juce::Component
    {
    public:
        float xDragFloat=0, yDragFloat=0;
        int xTab=0, maxMinsADay=0;

        Frame()
        {
            setInterceptsMouseClicks(false, false);
        }

        void update(float xDragFloatArg, float yDragFloatArg, int maxMinsADayArg)
        {
            xDragFloat = xDragFloatArg;
            yDragFloat = yDragFloatArg;
            maxMinsADay = maxMinsADayArg;
            repaint();
        }
        void setXTab(int xT){  xTab = xT;}

        void paint(juce::Graphics& g) override
        {
            if(numDays == 0 || activities.size() == 0)
                return;

            bool activeRange = true;
            if(maxMinsADay == 0)
                activeRange = false;
            
            float iF;
            if(activeRange)
            {
                for(int i=0; i<=maxMinsADay/60; i++)
                {
                    iF = 1.0 - ((float)i / ((float)maxMinsADay / 60.0f));
                    iF = iF * (float)getHeight();
                    g.setColour(juce::Colour(0x20FFFFFF));
                    g.drawLine( xTab-5, iF, getWidth(), iF, 1 );
                    if(iF <= 0)  iF = 0;
                    else if(iF > getHeight()-10) iF = getHeight() - 10;
                    else iF -= 5;
                    g.setColour(juce::Colour(0x40FFFFFF));
                    g.drawText( juce::String(i) + " hrs", 1, iF, xTab, 10, juce::Justification::left, true );
                }
            }
            else{
                g.setColour(juce::Colour(0x20FFFFFF));
                g.drawLine( xTab-5, 0, getWidth(), 0, 1 );
                g.setColour(juce::Colour(0x40FFFFFF));
                g.drawText("0 hrs", 1, 0, xTab, 10, juce::Justification::centred, true );
            }
        }
    };
    
    Bars bars;
    Frame frame;
    std::vector<int> visibleActivities;
};

class Tab2  :   public juce::Component
{
public:
    Tab2()
    {
        setSize(600, 600);
        begShowDate = juce::Time::getCurrentTime() - juce::RelativeTime(60*60*24*60);
        endShowDate = juce::Time::getCurrentTime();
        numDays = 60;   //endShowDate.getDayOfYear() - begShowDate.getDayOfYear();
        addAndMakeVisible(graph);
        graph.setBounds(10, 50, 580, 520);
        updateToggleActivityButtons();

        updatePickers = [this]{};
    }

    ~Tab2(){}
        
    void updateToggleActivityButtons()
    {
        if(activities.size() == 0){
            toggleActivityButtons.clear();
            return;
        }
        int yTab = getHeight() - 20;
        int bWidth = getWidth() / activities.size();
        toggleActivityButtons.clear();
        for(int i=0; i<activities.size(); i++)
        {
            auto* toAdd = new juce::TextButton{ activities[i].activityName };
            toggleActivityButtons.add( toAdd );
            addAndMakeVisible(toggleActivityButtons.getLast());
            toggleActivityButtons[i]->setBounds(i*bWidth, yTab, bWidth, 20);
            toggleActivityButtons[i]->setClickingTogglesState(true);
            toggleActivityButtons[i]->setColour( juce::TextButton::ColourIds::buttonOnColourId, colourArray[ i%10 ] );
            toggleActivityButtons[i]->onClick = [this] { updateVisibleActivities(); };
            toggleActivityButtons[i]->setToggleState(true, juce::dontSendNotification);
            updateVisibleActivities();
        }
    }

    void updateVisibleActivities()
    {
        std::vector<int> visibleActivities;
        for(int i=0; i<toggleActivityButtons.size(); i++)
        {
            if(toggleActivityButtons[i]->getToggleState() == true)
                visibleActivities.push_back(i);
        }
        graph.updateVisibleActivities(visibleActivities);
    }

private:
    juce::OwnedArray<juce::TextButton> toggleActivityButtons;
    Graph graph;
};
