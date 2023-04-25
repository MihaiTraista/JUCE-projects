#pragma once
#include "global.h"

enum EntryEditorFormat {bigEntryFormat, smallEntryFormat, dateFormat, timeFormat};
enum EditableFlags {flagEditableDate = 8, flagEditableStart = 4, flagEditableFinish = 2, flagEditableBreak = 1};
// bit3 date, bit2 start, bit1 finish, bit0 break
std::function<void()> callRefreshEntryTree;
std::function<void()> setEntryIsBeingEditedFalse;
struct EntryEditorGlobalVariables
{
    float ddWidth, height, dotWidth, hyphenWidth, fontHeight, totalWidth;
    juce::Font::FontStyleFlags fontStyleFlag;
    juce::Colour bgColour = juce::Colour(0xFF303030);
    std::function<void(bool, bool)> callParent;
    EntryEditorFormat format;
};

struct ColoursForEntries {
    static juce::Colour grey;
    static juce::Colour transparent;
    static juce::Colour white;
    static juce::Colour whiteAlpha;
    static juce::Colour black;
};
juce::Colour ColoursForEntries::grey =          juce::Colour(0xFF303030);
juce::Colour ColoursForEntries::transparent =   juce::Colour(0x00000000);
juce::Colour ColoursForEntries::white =         juce::Colour(0xFFFFFFFF);
juce::Colour ColoursForEntries::black =         juce::Colour(0xFF000000);
juce::Colour ColoursForEntries::whiteAlpha =    juce::Colour(0xFFFFFFFF).withAlpha(0.2f);

class Punctuation :   public juce::Component
{
public:
    int type;   //  0=colon, 1=hyphen, 2=paranthesis
    juce::Colour textColour;
    Punctuation()
    {
        setInterceptsMouseClicks(false, false);
        setWantsKeyboardFocus(false);
    }
    void paint(juce::Graphics& g) override
    {
        float unit = glob->height / 30.0f;
        g.setColour( textColour );
        if(type == 0){
            g.fillEllipse(0, unit*8, glob->dotWidth-unit, unit*4);
            g.fillEllipse(0, glob->height-unit*12, glob->dotWidth-unit, unit*4);
        }
        else if(type == 1)
            g.drawLine(unit*3, glob->height / 2, getWidth()-unit*6, glob->height / 2, unit*2);
        else
        {
            g.setFont( juce::Font(glob->fontHeight, glob->fontStyleFlag) );
            g.drawText( "(-     ')", 0, 0, getWidth(), getHeight(), juce::Justification::centredLeft, true );
        }
    }
    
    void init(int typeArg, EntryEditorGlobalVariables* globArg)
    {
        type = typeArg;
        glob = globArg;
    }
    
    void setColour(juce::Colour& textColourArg){ textColour = textColourArg; repaint();}
    
private:
    EntryEditorGlobalVariables* glob;
};

class MonthEditor   :   public juce::Component
{
public:
    EntryEditorGlobalVariables* glob;
    bool shiftIsDown = false, editable = false;
    int value = 0, min=0, max=11;
    std::vector<juce::String> monthNames {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
    juce::Colour textColour;

    MonthEditor(){}
    
    void initialize( EntryEditorGlobalVariables* globArg)
    {
        setWantsKeyboardFocus(true);
        glob = globArg;
    }

    void paint(juce::Graphics& g) override
    {
        if(editable){
            if(hasKeyboardFocus(true))
                g.setColour( juce::Colour(0xFF0020FF) );
            else
                g.setColour( juce::Colour(0x30000000) );
            g.fillRoundedRectangle(0, 0, getWidth(), getHeight(), glob->height / 5);
        }
        g.setColour( textColour );
        g.setFont( juce::Font( glob->fontHeight, glob->fontStyleFlag) );
        g.drawText( monthNames[value], 0, 0, getWidth(), getHeight(), juce::Justification::centred, false );
    }
    
    void mouseDown(const juce::MouseEvent& e)   override{ repaint(); }

    bool keyPressed(const juce::KeyPress& k) override
    {
//        std::cout << k.getKeyCode() << std::endl;
        if(k.getKeyCode() == 63232) //  arrow up
            value++;
        else if(k.getKeyCode() == 63233) //  arrow down
            value--;
        if(value > max)    value = max;
        else if(value < min)    value = min;
        repaint();
        return 1;
    }

    void modifierKeysChanged(const juce::ModifierKeys& m)   override{ shiftIsDown = m.isShiftDown(); }
    void focusLost(FocusChangeType cause)   override        { repaint(); }
    void focusGained(FocusChangeType cause)   override      { repaint(); }
    void setValue(int val) { value = val;   repaint(); }
    void setColour(juce::Colour& textColourArg){ textColour = textColourArg; repaint();}
    void setEditable(bool editableArg) {
        editable = editableArg;
        setInterceptsMouseClicks(editable, editable);
        repaint();
    }
};

class DoubleDigitEditor   :   public juce::Component, private juce::Timer
{
public:
    EntryEditorGlobalVariables* glob;
    bool underEdit = false, shiftIsDown = false, editable = false;
    int value = 0, min, max;
    juce::Colour textColour;

    DoubleDigitEditor(){}
    
    void initialize( EntryEditorGlobalVariables* globArg, int minArg, int maxArg )
    {
        min = minArg;
        max = maxArg;
        setWantsKeyboardFocus(true);
        glob = globArg;
    }

    void paint(juce::Graphics& g) override
    {
        if(editable){
            if(hasKeyboardFocus(true))
                g.setColour( juce::Colour(0xFF0020FF) );
            else
                g.setColour( juce::Colour(0x30000000) );
            g.fillRoundedRectangle(0, 0, getWidth(), getHeight(), glob->height / 5);
        }
        g.setColour( textColour );
        g.setFont( juce::Font( glob->fontHeight, glob->fontStyleFlag) );
        if(value == 0 && max < 100)
            g.drawText( "00", 0, 0, getWidth(), getHeight(), juce::Justification::centred, false );
        else if(value == 0 && max > 100)    //  this condition will only be met for the yearEditor
            g.drawText( "0000", 0, 0, getWidth(), getHeight(), juce::Justification::centred, false );
        else
            g.drawText( value >= 10 ? juce::String(value) : "0" + juce::String(value), 0, 0, getWidth(), getHeight(), juce::Justification::centred, false );
    }
    
    void mouseDown(const juce::MouseEvent& e)   override{ repaint(); }

    bool keyPressed(const juce::KeyPress& k) override
    {
        //std::cout << value << ", " << underEdit << std::endl;
        if(k.getKeyCode() == 13){    //  return key
            glob->callParent(false, true);
            return 1;
        }
        if(k.getKeyCode() == 63234 || k.getKeyCode() == 63235){ //left and right arrow
            glob->callParent(k.getKeyCode() == 63235, false);
            return 1;
        }
        else if((k.getKeyCode() == 58 && shiftIsDown) || k.getKeyCode() == 9){
            glob->callParent(true, false);
            return 1;
        }

        if(k.getKeyCode() >= 48 && k.getKeyCode() <= 57){
            if(underEdit){
                value = value * 10 + (k.getKeyCode() - 48);
                if( value > max )   value = max;
            }
            else
                value = k.getKeyCode() - 48;

            stopTimer();
            startTimer(1000);
            underEdit = true;
        }
        repaint();
        return 1;
    }

    void modifierKeysChanged(const juce::ModifierKeys& m)   override{ shiftIsDown = m.isShiftDown(); }
    void focusLost(FocusChangeType cause)   override        {
        underEdit = false;
        stopTimer();
        repaint();
    }
    void focusGained(FocusChangeType cause)   override      { repaint(); }
    void setValue(int val) { value = val;   repaint(); }
    void setColour(juce::Colour& textColourArg){ textColour = textColourArg; repaint();}
    void setEditable(bool editableArg) {
        editable = editableArg;
        setInterceptsMouseClicks(editable, editable);
        repaint();
    }

private:
    void timerCallback()    override
    {
        underEdit = false;
        repaint();
        stopTimer();
    }
};

class EntryEditor   : public juce::Component
{
public:
    DoubleDigitEditor yearEditor, dayEditor, sHourEditor, sMinuteEditor, fHourEditor, fMinuteEditor, breakEditor;
    Punctuation sColon, fColon, hyphen, breakParanthesis;
    MonthEditor monthEditor;
    EntryEditorGlobalVariables glob;
    juce::TextButton startFinishButton{ "Start" }, editButton{ "Edit" };
    juce::Time started;
    Entry* ptrToEntry;

    EntryEditor()
    {
        glob.callParent = [this](bool arrow, bool returnKeyPressed)
        {
            if(returnKeyPressed)
                startFinishButton.triggerClick();

        };

            /*
                if( !breakEditor.hasKeyboardFocus(true) )
                {
                    if(sHourEditor.hasKeyboardFocus(true) || sMinuteEditor.hasKeyboardFocus(true))
                        fHourEditor.grabKeyboardFocus();
                    else if(fHourEditor.hasKeyboardFocus(true) || fMinuteEditor.hasKeyboardFocus(true))
                        sHourEditor.grabKeyboardFocus();
                }
            }
            else if(sHourEditor.hasKeyboardFocus(true)){
                if(arrow)
                    sMinuteEditor.grabKeyboardFocus();
            }
            else if(sMinuteEditor.hasKeyboardFocus(true)){
                if(!arrow)
                    sHourEditor.grabKeyboardFocus();
            }
            else if(fHourEditor.hasKeyboardFocus(true)){
                if(arrow)
                    fMinuteEditor.grabKeyboardFocus();
            }
            else if(fMinuteEditor.hasKeyboardFocus(true)){
                if(arrow)
                    breakEditor.grabKeyboardFocus();
                else
                    fHourEditor.grabKeyboardFocus();
            }
            else if(breakEditor.hasKeyboardFocus(true) && !arrow)
                fMinuteEditor.grabKeyboardFocus();
            */

        startFinishButton.onClick = [this]
        {
            juce::Time toAdd;
            if(startFinishButton.getButtonText() == "Start"){
                toAdd = juce::Time ( yearEditor.value, monthEditor.value, dayEditor.value, sHourEditor.value, sMinuteEditor.value);
                started = toAdd;
            }
            else{
                toAdd = juce::Time ( yearEditor.value, monthEditor.value, dayEditor.value, fHourEditor.value, fMinuteEditor.value);
            }

            std::cout << "started " << started.toString(true, true) << std::endl;
            std::cout << "toAdd " << toAdd.toString(true, true) << std::endl;

            if(glob.format != smallEntryFormat)
            {
                bool doesOverlapAPair = false;
                for(Day& d : activities[selectedActivityIndex].days)
                {
                    for(Entry& e : d.entries)
                    {
                        if(t1_BiggerThan_t2(toAdd, e.tStart) && t1_BiggerThan_t2(e.tFinish, toAdd)){
                            doesOverlapAPair = true;
                            break;
                        }
                    }
                    if(doesOverlapAPair)
                        break;
                }
                if(doesOverlapAPair){
                    std::cout << "You can't add an entry that overlaps an existing pair" << std::endl;
                    return;
                }
            }   // check if the new entry overlaps an existing pair
            {
                if( !t1_BiggerThan_t2(juce::Time::getCurrentTime()+juce::RelativeTime::minutes(5), toAdd) )
                {
                    std::cout << "You can't add an entry in the future." << std::endl;
                    return;
                }
            }   // check if the new entry is in the future
            {
                if( startFinishButton.getButtonText() == "Finish" && t1_BiggerThan_t2(started, toAdd) ){
                    std::cout << "finishing time can not be earlier than starting time." << std::endl;
                    return;
                }
            }   // check if the fT < sT

            if(glob.format == bigEntryFormat){
                if(sHourEditor.hasKeyboardFocus(true) || sMinuteEditor.hasKeyboardFocus(true))
                    activities[selectedActivityIndex].addEntry(toAdd, false);
                else if (fHourEditor.hasKeyboardFocus(true) || fMinuteEditor.hasKeyboardFocus(true))
                    activities[selectedActivityIndex].addEntry(toAdd, true);
                else
                    activities[selectedActivityIndex].addBreak(juce::RelativeTime(60 * breakEditor.value));
            }
            else{
                if(startFinishButton.getButtonText() == "Finish"){
                    ptrToEntry->tStart = started;
                    ptrToEntry->tFinish = toAdd;
                    ptrToEntry->breakTime = juce::RelativeTime(60 * breakEditor.value);
                    setEntryIsBeingEditedFalse();
                    activities[selectedActivityIndex].sort();
                }
            }

            if(glob.format != smallEntryFormat)
                callRefreshEntryTree();

// flagEditableDate, flagEditableStart, flagEditableFinish, flagEditableBreak
            if(startFinishButton.getButtonText() == "Start"){
                started = toAdd;
                setEditable(flagEditableDate | flagEditableFinish | flagEditableBreak);
                setColours(true, ColoursForEntries::grey, ColoursForEntries::white, ColoursForEntries::whiteAlpha, ColoursForEntries::white, ColoursForEntries::white);
                startFinishButton.setButtonText( "Finish" );
            }
            else{
                if(glob.format != smallEntryFormat){
                    sHourEditor.setValue(0);
                    sMinuteEditor.setValue(0);
                    fHourEditor.setValue(0);
                    fMinuteEditor.setValue(0);
                    breakEditor.setValue(0);
                }
                if(glob.format == bigEntryFormat){
                    setEditable(flagEditableDate | flagEditableStart);
                    setColours(true, ColoursForEntries::grey, ColoursForEntries::white, ColoursForEntries::white, ColoursForEntries::whiteAlpha, ColoursForEntries::whiteAlpha);
                }
                startFinishButton.setButtonText( "Start" );
            }
        };
    }
    
    void paint(juce::Graphics& g) override
    {
        g.fillAll(glob.bgColour);
    }

    int getTotalWidth()
    {
        return glob.totalWidth;
    }
    
    void thisIsThePointerToEntry(Entry* ptr){ ptrToEntry = ptr; }
    
    void initialize(int heightArg, EntryEditorFormat formatArg)
    {
        glob.height = heightArg;
        glob.format = formatArg;
        glob.ddWidth = glob.height * 1.1666;
        glob.dotWidth = glob.height * 0.1666;
        glob.hyphenWidth = glob.height * 0.6666;
        glob.fontHeight = glob.height * 0.95f;
        glob.fontStyleFlag = juce::Font::FontStyleFlags::bold;

        sHourEditor.initialize(&glob, 0, 23);
        sMinuteEditor.initialize(&glob, 0, 59);
        fHourEditor.initialize(&glob, 0, 23);
        fMinuteEditor.initialize(&glob, 0, 59);
        breakEditor.initialize(&glob, 0, 90);
        yearEditor.initialize(&glob, 2020, 2030);
        monthEditor.initialize(&glob);
        dayEditor.initialize(&glob, 1, 31);
        sColon.init(0, &glob);
        fColon.init(0, &glob);
        hyphen.init(1, &glob);
        breakParanthesis.init(2, &glob);

        if(glob.format == smallEntryFormat)
            setInterceptsMouseClicks(false, false);
        
        addAndMakeVisible(sHourEditor);
        addAndMakeVisible(sMinuteEditor);
        addAndMakeVisible(fHourEditor);
        addAndMakeVisible(fMinuteEditor);
        addAndMakeVisible(breakEditor);
        addAndMakeVisible(sColon);
        addAndMakeVisible(fColon);
        addAndMakeVisible(hyphen);
        addAndMakeVisible(breakParanthesis);
        addChildComponent(startFinishButton);
        addAndMakeVisible(yearEditor);
        addAndMakeVisible(monthEditor);
        addAndMakeVisible(dayEditor);
        
        float tab = 0;
        
        if(glob.format == bigEntryFormat || glob.format == dateFormat)
        {
            yearEditor.setValue(juce::Time::getCurrentTime().getYear());
            monthEditor.setValue(juce::Time::getCurrentTime().getMonth());
            dayEditor.setValue(juce::Time::getCurrentTime().getDayOfMonth());

            dayEditor.setBounds(tab, 0, glob.ddWidth, glob.height);
            monthEditor.setBounds(tab+=glob.ddWidth, 0, glob.dotWidth*10, glob.height);
            yearEditor.setBounds(tab+=glob.dotWidth*10, 0, glob.ddWidth*2, glob.height);
            tab += glob.ddWidth*3;
        }
        if(glob.format != dateFormat)
        {
            sHourEditor.setBounds(tab, 0, glob.ddWidth, glob.height);
            sColon.setBounds(tab+=glob.ddWidth, 0, glob.dotWidth, glob.height);
            sMinuteEditor.setBounds(tab+=glob.dotWidth, 0, glob.ddWidth, glob.height);
            tab += glob.ddWidth;
        }
        if(glob.format == bigEntryFormat || glob.format == smallEntryFormat)
        {
            hyphen.setBounds(tab, 0, glob.dotWidth * 4, glob.height);
            tab += glob.dotWidth * 4;
            fHourEditor.setBounds(tab, 0, glob.ddWidth, glob.height);
            tab += glob.ddWidth;
            fColon.setBounds(tab, 0, glob.dotWidth, glob.height);
            tab += glob.dotWidth;
            fMinuteEditor.setBounds(tab, 0, glob.ddWidth, glob.height);
            tab += glob.ddWidth + glob.dotWidth;
            breakParanthesis.setBounds(tab, 0, glob.dotWidth*16, glob.height);
            breakEditor.setBounds(tab + glob.dotWidth*5, 0, glob.ddWidth, glob.height);
            tab += glob.dotWidth*16;
        }
        if(glob.format == bigEntryFormat){
            startFinishButton.setBounds(tab, 0, glob.dotWidth*16, glob.height);
            tab += glob.dotWidth*16;
        }
        glob.totalWidth = tab;
//        setEditable(editableBitMask);
//        setColours(0x10002233);
    }
    
    void setEditable(int bitMask)
    {
        // bit3 date, bit2 start, bit1 finish, bit0 break
        setInterceptsMouseClicks(bitMask, bitMask);

        yearEditor.setEditable(bitMask & flagEditableDate);
        monthEditor.setEditable(bitMask & flagEditableDate);
        dayEditor.setEditable(bitMask & flagEditableDate);
        sHourEditor.setEditable(bitMask & flagEditableStart);
        sMinuteEditor.setEditable(bitMask & flagEditableStart);
        fHourEditor.setEditable(bitMask & flagEditableFinish);
        fMinuteEditor.setEditable(bitMask & flagEditableFinish);
        breakEditor.setEditable(bitMask & flagEditableBreak);
    }
    
    void setColours(bool boldFlag, juce::Colour bgCol, juce::Colour dateCol, juce::Colour sCol, juce::Colour fCol, juce::Colour bCol)  // bold, bg, date, s, f, b
    {

        if(boldFlag)        glob.fontStyleFlag = juce::Font::FontStyleFlags::bold;
        else                glob.fontStyleFlag = juce::Font::FontStyleFlags::plain;

        glob.bgColour = bgCol;
        
        yearEditor.setColour(dateCol);
        monthEditor.setColour(dateCol);
        dayEditor.setColour(dateCol);
        sHourEditor.setColour(sCol);
        sColon.setColour(sCol);
        sMinuteEditor.setColour(sCol);
        fHourEditor.setColour(fCol);
        fMinuteEditor.setColour(fCol);
        fColon.setColour(fCol);
        hyphen.setColour(bCol);
        breakParanthesis.setColour(bCol);
        breakEditor.setColour(bCol);

        repaint();
    }

    void setValues(juce::Time tS, juce::Time tF, juce::RelativeTime tBreak)
    {
        started = tS;
        yearEditor.setValue(tS.getYear());
        monthEditor.setValue(tS.getMonth());
        dayEditor.setValue(tS.getDayOfMonth());
        sHourEditor.setValue(tS.getHours());
        sMinuteEditor.setValue(tS.getMinutes());
        fHourEditor.setValue(tF.getHours());
        fMinuteEditor.setValue(tF.getMinutes());
        breakEditor.setValue(tBreak.inMinutes());
    }

    void setDateAndTime(juce::Time t)
    {
        yearEditor.setValue(t.getYear());
        monthEditor.setValue(t.getMonth());
        dayEditor.setValue(t.getDayOfMonth());
        sHourEditor.setValue(t.getHours());
        sMinuteEditor.setValue(t.getMinutes());
    }
    
    juce::Time getTime(){
        return juce::Time(yearEditor.value, monthEditor.value, dayEditor.value, sHourEditor.value, sMinuteEditor.value);
    }

    juce::RelativeTime getRelativeTime(){
        return juce::RelativeTime( 60 * sMinuteEditor.value + 60*60*sHourEditor.value);
    }
};
