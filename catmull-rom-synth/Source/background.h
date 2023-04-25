#pragma once
using namespace juce;

//==============================================================================
class PlayHead : public Component,
                 private Timer
{
public:
    PlayHead ()
    {
        setInterceptsMouseClicks(false, false);
        timerLabel.setBounds(250, 10, 100, 50);
        addAndMakeVisible(timerLabel);
    }

    void startUpdating()
    {
        showPlayHead = 1;
        initialMs = Time::getMillisecondCounter();
        startTimer (40);
    }

    void stopUpdating()
    {
        showPlayHead = 0;
        timerLabel.setText("0.00000", dontSendNotification);
        stopTimer();
    }

    void paint (Graphics& g) override
    {
        if(showPlayHead)
        {
            g.setColour(Colours::orange);
            float x = Time::getMillisecondCounter() - initialMs;
            x /= (float)projectLength;
            x = x * (float)editArea.getWidth() + (float)editArea.getX();
            Line<float> timebar(x, editArea.getY(), x, getHeight());
            g.drawLine(timebar, 2.0);
        }
    }
    void setEditZoneBounds(Rectangle<int>& rect)
    {
        editArea = rect;
    }

private:
    void timerCallback() override
    {
        int msPassed = Time::getMillisecondCounter() - initialMs;
        std::string msPassedString = std::to_string((float)msPassed/1000.0);
        timerLabel.setText(msPassedString, dontSendNotification);

        repaint();
    }

    Label timerLabel = {"0"};
    bool showPlayHead = 0;
    int initialMs;
    Rectangle<int> editArea;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PlayHead)
};

//==============================================================================
class RulerLines : public Component
{
public:
    RulerLines()
    {
        setInterceptsMouseClicks(false, false);
    }

    void drawRulerLines(Rectangle<int>& arg)
    {
        editArea = arg;
        repaint();
    }
    
    void paint(Graphics& g)   override
    {
        g.setFont(Font(20, 0));
        g.setColour(Colour(0x90FFFFFF));
        Line<float> tick;
        int x;
        for(int i=0; i<5; i++)
        {
            x = i * (editArea.getWidth() / 5) + editArea.getX();
            tick = Line<float> (x, editArea.getY() - 30, x, getHeight());
            g.drawLine(tick);
            g.drawText(String(i), x+4, editArea.getY() - 30, 30, 30, 1, 0);
        }
    }
private:
    Rectangle<int> editArea;
};

//==============================================================================
class Background : public Component
{
public:
    Background()
    {
        setInterceptsMouseClicks(false, false);
    }

    void defineEditArea()
    {
        toolBarPanel.setBounds(editArea.removeFromTop(50));
        toolBarPanel.setColour(Label::backgroundColourId, Colour(0xff602020));
        addAndMakeVisible(toolBarPanel);

        leftPanel.setBounds(editArea.removeFromLeft(50));
        leftPanel.setColour(Label::backgroundColourId, Colour(0x30202030));
        addAndMakeVisible(leftPanel);
        
        rulerLabel.setBounds(editArea.removeFromTop(50));
//        rulerLabel.setColour(Label::backgroundColourId, Colour(0xFF002030));
        addAndMakeVisible(rulerLabel);
        addAndMakeVisible(rulerLines);
        rulerLines.setBounds(getLocalBounds());
        rulerLines.drawRulerLines(editArea);
    }
    
    void update(bool pitchOrWFArg)
    {
        pitchOrWF = pitchOrWFArg;
        repaint();
    }
    
    void paint (Graphics& g) override
    {
        if(pitchOrWF)
        {
            drawKeyboardNotes(g);
            drawNoteNames(g);
            rulerLabel.setVisible(true);
            rulerLines.setVisible(true);
        }
        else
        {
            g.setColour(Colour(0xFF202020));
            g.fillRect(editArea);
            g.setColour(Colour(0xFF0077FF));
            g.drawRect(editArea, 2.0f);
            rulerLabel.setVisible(false);
            rulerLines.setVisible(false);
        }
    }

    void drawKeyboardNotes(Graphics& g)
    {
        Rectangle<int> rectMidiNote (0, editArea.getY(), getWidth(), editArea.getHeight()/12);
        g.setFont(Font(20, 1));
        for(int i=0; i<12; i++)
        {
            int currentY = i * (editArea.getHeight()/12) + editArea.getY();

            if(i==10 || i==8 || i==5 || i==3 || i==1)
                g.setColour(Colour(0xFF202623));
            else
                g.setColour(Colour(0xFF303632));
            rectMidiNote.setY(currentY);
            g.fillRect(rectMidiNote);
            g.setColour(Colour(0xFF202623));
            g.drawRect(rectMidiNote, 1);
        }
        g.setColour(Colour(0xA0FFFFFF));
        Line<float> verticalLineRightOfNoteNames (editArea.getX(), editArea.getY(), editArea.getX(), getHeight());
        g.drawLine(verticalLineRightOfNoteNames, 2);
    }
    
    void drawNoteNames(Graphics& g)
    {
       Rectangle<int> rectMidiNote (0, editArea.getY(), getWidth(), editArea.getHeight()/12);
       g.setFont(Font(20, 1));
       for(int i=0; i<12; i++)
       {
           int currentY = i * (editArea.getHeight()/12) + editArea.getY();

           g.setColour(Colour(0xFFFFFFFF));
           if(i == 11)             g.drawText("C", 10, currentY - 6, 100, 50, 1, 0);
           if(i == 10)             g.drawText("C#", 10, currentY - 6, 100, 50, 1, 0);
           if(i == 9)              g.drawText("D", 10, currentY - 6, 100, 50, 1, 0);
           if(i == 8)              g.drawText("D#", 10, currentY - 6, 100, 50, 1, 0);
           if(i == 7)              g.drawText("E", 10, currentY - 6, 100, 50, 1, 0);
           if(i == 6)              g.drawText("F", 10, currentY - 6, 100, 50, 1, 0);
           if(i == 5)              g.drawText("F#", 10, currentY - 6, 100, 50, 1, 0);
           if(i == 4)              g.drawText("G", 10, currentY - 6, 100, 50, 1, 0);
           if(i == 3)              g.drawText("G#", 10, currentY - 6, 100, 50, 1, 0);
           if(i == 2)              g.drawText("A", 10, currentY - 6, 100, 50, 1, 0);
           if(i == 1)              g.drawText("A#", 10, currentY - 6, 100, 50, 1, 0);
           if(i == 0)              g.drawText("B", 10, currentY - 6, 100, 50, 1, 0);
       }
    }
        

private:
    Label toolBarPanel;
    Label rulerLabel;
    Label leftPanel;
    bool pitchOrWF = true;
    int myA;
    RulerLines rulerLines;
};

//==============================================================================
class Thumbnail : public Component
{
public:
    Thumbnail()
    {
        setWantsKeyboardFocus(false);
        setInterceptsMouseClicks(false, false);
//        AudioVisualiserComponent c;
    }
    
    void update()
    {
        convertBufferToPath();
        repaint();
    }
    
    float getAverageY(int currentLocInBuffer, int numSamplesPerPathPoint)
    {
        float accum = 0;
        for(int j=0; j<numSamplesPerPathPoint; j++)
        {
            accum += abs(renderedBuffer.getSample(0, j+currentLocInBuffer));
        }
        accum /= numSamplesPerPathPoint;
        return accum;
    }
    
    void convertBufferToPath()
    {
        int numSamples = renderedBuffer.getNumSamples();
        int numOfPathPoints = getWidth();
        float averageY;
        int numSamplesPerPathPoint = numSamples / numOfPathPoints;
        int currentLocInBuffer;
        path.clear();
        path.preallocateSpace (numOfPathPoints);
        path.startNewSubPath(0, getHeight()/2);
        for(int i=0; i<numOfPathPoints; i++)
        {
            currentLocInBuffer = i*numSamplesPerPathPoint;
            averageY = getAverageY(currentLocInBuffer, numSamplesPerPathPoint);
//            averageY *= 10.0;    //  boost level to see better
            if(i%2 == 0)
                averageY *= -1;
            if(averageY > 1.0f) averageY = 1.0f;
            if(averageY < -1.0f) averageY = -1.0f;
            averageY = averageY * 0.5 + 0.5;
            averageY *= (float)getHeight();
//            std::cout << i<< ", " << averageY << std::endl;
            path.lineTo(i, averageY);
        }
    }
    
    void paint(Graphics& g)    override
    {
        g.drawLine(0, 1, getWidth(), 1);
        g.setColour(thumbnailColour);
        g.strokePath(path, PathStrokeType (1.5f));
    }
    
    void setThumbnailColour(Colour col)
    {
        thumbnailColour = col;
        update();
    }

private:
    Path path;
    Colour thumbnailColour;
};

//==============================================================================
