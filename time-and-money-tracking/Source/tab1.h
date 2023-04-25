#pragma once
#include "global.h"
#include "entryEditor.h"

enum AddActivityButtonTypes {addActivityButtonClicked, editActivityButtonClicked, cancelActivityButtonClicked};
std::function<void(AddActivityButtonTypes buttonType)> callUpdateComboBox;

//==============================================================================
class AddActivityDialog  :  public juce::Component
{
public:
    juce::TextButton okButton{"Ok"}, cancelButton{"Cancel"}, addActivityCallback, editActivityCallback, cancelCallback;
    EntryEditor begDateEditor, endDateEditor, commitmentHDayEditor;
    Activity* underEdit;
    AddActivityDialog()
    {
        setOpaque(true);

        okButton.onClick = [this]
        {
            underEdit->activityName = activityName.getText() == "" ? "no name provided" : activityName.getText();
            underEdit->goalDescription = goalDescription.getText();
            underEdit->isGoal = isGoalButton.getToggleState();
            underEdit->begDate = begDateEditor.getTime();
            underEdit->endDate = endDateEditor.getTime();
            underEdit->commitmentHDay = commitmentHDayEditor.getRelativeTime();
            underEdit->totalTarget = juce::RelativeTime::hours(totalTarget.getText().getIntValue());

            if(currentShowState == addState)
                callUpdateComboBox(addActivityButtonClicked);
            else
                callUpdateComboBox(editActivityButtonClicked);
            currentShowState = showState;
            updateLabelAppearance();
        };

        cancelButton.onClick = [this]
        {
            currentShowState = showState;
            updateLabelAppearance();
            activities.pop_back();
            okButton.setVisible(false);
            cancelButton.setVisible(false);
            dialogTitleLabel.setVisible(false);
            isGoalButton.setVisible(false);
            callUpdateComboBox(cancelActivityButtonClicked);
        };
        
        isGoalButton.onClick = [this]
        {
            showOrHideIsGoalFields();
        };
        
        //begDatePicker.changed.onClick = [this]  { autoFill(); };
        //endDatePicker.changed.onClick = [this]  { autoFill(); };
        //commitmentHDayPicker.changed.onClick = [this]  { autoFill(); };
        totalTarget.onTextChange = [this]  { autoFill(); };
    }
        
    void autoFill()
    {
        return;
/*
        Activity act;
        act.commitmentHDay = commitmentHDayPicker.getRelativeTime();
        act.totalTarget = juce::RelativeTime::hours(totalTarget.getText().getIntValue());
        act.begDate = begDatePicker.getTime();
        if(act.commitmentHDay.inMinutes() == 0 || act.totalTarget.inMinutes() == 0)
            return;
        
        int dayCount = 0;
        juce::RelativeTime countHours(0);
        while(true)
        {
            if(countHours.inHours() < act.totalTarget.inHours())
            {
                countHours += act.commitmentHDay;
                dayCount++;
            }
            else
                break;
        }
        act.endDate = act.begDate + juce::RelativeTime::days(dayCount-1);
        endDatePicker.setDateAndTime(act.endDate);
*/
 }

    void showOrHideIsGoalFields()
    {
        begDateEditor.setVisible(isGoalButton.getToggleState());
        endDateEditor.setVisible(isGoalButton.getToggleState());
        commitmentHDayEditor.setVisible(isGoalButton.getToggleState());
        goalDescription.setVisible(isGoalButton.getToggleState());
        begDateLabel.setVisible(isGoalButton.getToggleState());
        endDateLabel.setVisible(isGoalButton.getToggleState());
        commitmentLabel.setVisible(isGoalButton.getToggleState());
        goalDescriptionLabel.setVisible(isGoalButton.getToggleState());
        targetTotalLabel.setVisible(isGoalButton.getToggleState());
        totalTarget.setVisible(isGoalButton.getToggleState());
    }

    void paint(juce::Graphics& g) override
    {
        g.fillAll(juce::Colour(0xFF171A1A));    //  0xFF105020
    }

    void initialize()
    {
        int tab = 120, betweenItems = 2, itemHeight = 20, editLabelWidth = 200;
        int currentY = 0, timeEditorWidth = 70;

        {
            addAndMakeVisible(activityName);
            addChildComponent(goalDescription);
            addChildComponent(totalTarget);
            addChildComponent(progressSoFar);
            addChildComponent(targetSoFar);
            addChildComponent(aheadOfSchedule);

            addChildComponent(dialogTitleLabel);

            addChildComponent(okButton);
            addChildComponent(cancelButton);
            addChildComponent(isGoalButton);
            addChildComponent(addActivityCallback);
            addChildComponent(editActivityCallback);

            addAndMakeVisible(activityNameLabel);
            addChildComponent(begDateLabel);
            addChildComponent(endDateLabel);
            addChildComponent(commitmentLabel);
            addChildComponent(goalDescriptionLabel);
            addChildComponent(targetTotalLabel);
            
            addChildComponent(begDateEditor);
            begDateEditor.initialize(itemHeight, dateFormat);
            begDateEditor.setEditable(0);
            begDateEditor.setColours(false, ColoursForEntries::transparent, ColoursForEntries::black, ColoursForEntries::black, ColoursForEntries::black, ColoursForEntries::black);
            addChildComponent(endDateEditor);
            endDateEditor.initialize(itemHeight, dateFormat);
            endDateEditor.setEditable(0);
            endDateEditor.setColours(false, ColoursForEntries::transparent, ColoursForEntries::black, ColoursForEntries::black, ColoursForEntries::black, ColoursForEntries::black);
            addChildComponent(commitmentHDayEditor);
            commitmentHDayEditor.initialize(itemHeight, timeFormat);
        }       // add child components
        {
            dialogTitleLabel.setJustificationType(juce::Justification::centred);
            dialogTitleLabel.setFont(juce::Font(20, juce::Font::FontStyleFlags::bold));
            dialogTitleLabel.setBounds(getWidth()/2-editLabelWidth/2, currentY, editLabelWidth, itemHeight);
            currentY += betweenItems + itemHeight;
            activityName.setBounds(tab, currentY, editLabelWidth, itemHeight);
            currentY += betweenItems + itemHeight;
            isGoalButton.setBounds(tab, currentY, editLabelWidth, itemHeight);
            currentY += betweenItems + itemHeight;
            goalDescription.setBounds(tab, currentY, editLabelWidth, itemHeight);

            currentY += betweenItems + itemHeight;
            totalTarget.setBounds(tab, currentY, timeEditorWidth, itemHeight);

            currentY += betweenItems + itemHeight;
            begDateEditor.setBounds(tab, currentY, begDateEditor.getTotalWidth(), itemHeight);
            
            currentY += betweenItems + itemHeight;
            endDateEditor.setBounds(tab, currentY, endDateEditor.getTotalWidth(), itemHeight);
            currentY += betweenItems + itemHeight;
            commitmentHDayEditor.setBounds(tab, currentY, commitmentHDayEditor.getTotalWidth(), itemHeight);

            currentY += betweenItems + itemHeight;
            okButton.setBounds(tab, currentY, timeEditorWidth*2, itemHeight);
            cancelButton.setBounds(tab+timeEditorWidth*2+betweenItems, currentY, timeEditorWidth*2, itemHeight);
            
            progressSoFar.setBounds(0, currentY ,getWidth(), itemHeight);
            currentY += betweenItems + itemHeight;
            targetSoFar.setBounds(0, currentY ,getWidth(), itemHeight);
            currentY += betweenItems + itemHeight;
            aheadOfSchedule.setBounds(0, currentY ,getWidth(), itemHeight);

            activityNameLabel.attachToComponent(&activityName, true);
            goalDescriptionLabel.attachToComponent(&goalDescription, true);
            begDateLabel.attachToComponent(&begDateEditor, true);
            endDateLabel.attachToComponent(&endDateEditor, true);
            commitmentLabel.attachToComponent(&commitmentHDayEditor, true);
            targetTotalLabel.attachToComponent(&totalTarget, true);
        }       // set bounds
    }

    void updateLabelAppearance()
    {
        bool editable = ( currentShowState == editState || currentShowState == addState);
        bool isGoal = activities[selectedActivityIndex].isGoal;
        bool showGoalFields = ( currentShowState == showState || currentShowState == editState) && isGoal;
        bool showProgressFields = currentShowState == showState && isGoal;

        activityName.setVisible(true);
        isGoalButton.setVisible(editable);
        isGoalButton.setToggleState(currentShowState == editState && isGoal, juce::dontSendNotification);

        {
            goalDescription.setVisible(showGoalFields);
            totalTarget.setVisible(showGoalFields);
            begDateEditor.setVisible(showGoalFields);
            endDateEditor.setVisible(showGoalFields);
            commitmentHDayEditor.setVisible(showGoalFields);
        }   //  show or hide goal fields
        {
            progressSoFar.setVisible(showProgressFields);
            targetSoFar.setVisible(showProgressFields);
            aheadOfSchedule.setVisible(showProgressFields);
        }   //  show or hide progress fields
        {
            activityName.setEditable(editable);
            goalDescription.setEditable(editable);
            totalTarget.setEditable(editable);
            if(editable){
                begDateEditor.setEditable(flagEditableDate);
                endDateEditor.setEditable(flagEditableDate);
            }
            else{
                begDateEditor.setEditable(0);
                endDateEditor.setEditable(0);
            }
            commitmentHDayEditor.setEditable(editable);
        }   //  set editable
        {
            juce::Colour colour;
            if(editable)    colour = juce::Colours::black;
            else            colour = juce::Colour(0x20000000);

            activityName.setColour(juce::Label::backgroundColourId, colour);
            goalDescription.setColour(juce::Label::backgroundColourId, colour);
            totalTarget.setColour(juce::Label::backgroundColourId, colour);
            //begDatePicker.setBackgroundColour(colour);
            //endDatePicker.setBackgroundColour(colour);
            //commitmentHDayPicker.setBackgroundColour(colour);
        }   //  set background colour
        {
            if(currentShowState == addState)
                activityName.setText("", juce::dontSendNotification);
            if(currentShowState == addState || (currentShowState == addState && !isGoal))
            {
                goalDescription.setText("", juce::dontSendNotification);
                totalTarget.setText("", juce::dontSendNotification);
                begDateEditor.setDateAndTime(juce::Time::getCurrentTime());
                endDateEditor.setDateAndTime(juce::Time::getCurrentTime());
                commitmentHDayEditor.setDateAndTime(juce::Time(1970, 1, 1, 0, 0));
            }
        }   //  empty label content
        {
            okButton.setVisible(editable);
            cancelButton.setVisible(editable);
            dialogTitleLabel.setVisible(editable);
            if(currentShowState == addState)
                dialogTitleLabel.setText("Add Activity", juce::dontSendNotification);
            else if (currentShowState == editState)
                dialogTitleLabel.setText("Edit Activity", juce::dontSendNotification);
        }   //  ok, cancel and title visibility and text
        if(currentShowState == showState)
        {
            activityName.setText(activities[selectedActivityIndex].activityName, juce::dontSendNotification);
            if(isGoal)
            {
                goalDescription.setText(juce::String(activities[selectedActivityIndex].goalDescription), juce::dontSendNotification);

                totalTarget.setText(juce::String(activities[selectedActivityIndex].totalTarget.inHours()) + "hours", juce::dontSendNotification);

                begDateEditor.setDateAndTime(activities[selectedActivityIndex].begDate);
                endDateEditor.setDateAndTime(activities[selectedActivityIndex].endDate);
                float h = activities[selectedActivityIndex].commitmentHDay.inHours();
                float m = activities[selectedActivityIndex].commitmentHDay.inMinutes();
                m = m - ((int)h * 60);
                juce::Time comm(1971, 1, 1, (int)h, (int)m);
                commitmentHDayEditor.setDateAndTime(comm);

                progressSoFar.setText("Progress so far: " + juce::String((int)activities[selectedActivityIndex].progressSoFar.inHours()), juce::dontSendNotification);

                juce::RelativeTime daysPassedSinceBegOfActivity = juce::Time::getCurrentTime() - activities[selectedActivityIndex].begDate;
                int daysPassed = daysPassedSinceBegOfActivity.inDays();
                
                juce::RelativeTime targetSoFarRT(0);
                for(int d=0; d<daysPassed; d++)
                    targetSoFarRT += activities[selectedActivityIndex].commitmentHDay;

                targetSoFar.setText("Target so far: " + juce::String((int)targetSoFarRT.inHours()) + " hrs", juce::dontSendNotification);

                aheadOfSchedule.setText("Ahead of schedule: " + juce::String((int)(activities[selectedActivityIndex].progressSoFar - targetSoFarRT).inHours()) + " hrs", juce::dontSendNotification);
            }
        }   //  fill context from (*activity)[selectedActivityIndex]
    }
    
    void changeActivityIndex(int index)
    {
        selectedActivityIndex = index;
        currentShowState = showState;
        updateLabelAppearance();
    }
    
    void addActivity()
    {
        activities.push_back(Activity());
        underEdit = &activities.back();
        currentShowState = addState;
        updateLabelAppearance();
    }

    void editActivity()
    {
        underEdit = &activities[selectedActivityIndex];
        currentShowState = editState;
        updateLabelAppearance();
    }
    
    void hideEverything()
    {
        activityName.setVisible(false);
        goalDescription.setVisible(false);
        totalTarget.setVisible(false);
        progressSoFar.setVisible(false);
        aheadOfSchedule.setVisible(false);
        targetSoFar.setVisible(false);
        begDateEditor.setVisible(false);
        endDateEditor.setVisible(false);
        commitmentHDayEditor.setVisible(false);
    }
    
private:
    juce::Label activityNameLabel{{}, {"Activity Name:"}};
    juce::Label begDateLabel{{}, {"Start Date:"}};
    juce::Label endDateLabel{{}, {"End Date:"}};
    juce::Label commitmentLabel{{}, {"Commitment h/day:"}};
    juce::Label goalDescriptionLabel{{}, {"Goal Description:"}};
    juce::Label targetTotalLabel{{}, {"Total Target:"}};
    juce::Label dialogTitleLabel{{}, {"Add Activity"}};
    juce::Label activityName, goalDescription, totalTarget, progressSoFar, aheadOfSchedule, targetSoFar;
    juce::ToggleButton isGoalButton {"This Activity Has A Goal"};
    ShowStates currentShowState;
};

//===================================================================================================
class EntryListBox   :  public juce::Component
{
public:
    EntryListBox(){
        setEntryIsBeingEditedFalse = [this]{
            entryIsBeingEdited = false;
            floatingButtons.entryEditorPtr->setEditable(false);
            floatingButtons.entryEditorPtr->setInterceptsMouseClicks(false, false);
            updateListBox(false);
        };
    }

    void changeExpandDayValue()
    {
        std::cout << "clickedY " << clickedY << ", entry: " << clickedY / lineHeight << ", scrollZeroToOne " << scrollZeroToOne << std::endl;
        int alreadyExpandedDay = expandDay;
        if(clickedY / lineHeight + scrollZeroToOne > pointers.size()-1)
            expandDay = -1;
        else{
            expandDay = pointers[clickedY / lineHeight + scrollZeroToOne]->dayIndex;
            if(activities[selectedActivityIndex].days[expandDay].entries.size() == 1)
                expandDay = -1;
        }
        if(expandDay == alreadyExpandedDay)
            expandDay = -1;
        updateListBox(false);
    }

    void initialize (const int maxEntries, const int listBoxLineHeight)
    {
        lineHeight = listBoxLineHeight;
        vTab1 = (float)((float)lineHeight * 2.5f);
        vTab2 = vTab1 + (float)((float)lineHeight * 2.0f);
        vTab3 = vTab2 + (float)((float)lineHeight * 2.7f);
        vTab4 = vTab3 + (float)((float)lineHeight * 13.0f);
        floatingButtons.initialize(&entryIsBeingEdited, &lineHeight);
        dateVisualiser.initialize(lineHeight, vTab1, vTab2, vTab3, vTab4, &pointers, &scrollZeroToOne);
        maxEntryCount = maxEntries;
        expandDay = -1;
        scrollInt = 0;
        for(int i=0; i<maxEntryCount; i++)
        {
            dateString.add(new juce::String);
            entryString.add(new juce::String);
            dateLabel.add(new juce::Label{{}, ""});
            daySumLabel.add(new juce::Label{{}, ""});
            entry.add(new EntryEditor());
        }
        for(int i=0; i<maxEntryCount; i++)
        {
            addAndMakeVisible(*dateLabel[i]);
            dateLabel[i]->setColour(juce::Label::textColourId, juce::Colours::black);
            dateLabel[i]->setFont(juce::Font(lineHeight, juce::Font::plain));
            dateLabel[i]->setJustificationType(juce::Justification::centredLeft);
            dateLabel[i]->setInterceptsMouseClicks(false, false);

            addAndMakeVisible(entry[i]);
            entry[i]->initialize(lineHeight, smallEntryFormat);
            entry[i]->setEditable(false);
            entry[i]->setColours(false, ColoursForEntries::transparent, ColoursForEntries::black, ColoursForEntries::black, ColoursForEntries::black, ColoursForEntries::black);
            addAndMakeVisible(*daySumLabel[i]);
            daySumLabel[i]->setColour(juce::Label::textColourId, juce::Colours::black);
            daySumLabel[i]->setFont(juce::Font(lineHeight, juce::Font::plain));
            daySumLabel[i]->setInterceptsMouseClicks(false, false);
        }
        addAndMakeVisible(scrollHandleStrip);
        scrollHandleStrip.setBounds(getWidth()-10, 0, 10, getHeight());
        addAndMakeVisible(scrollHandle);
        addAndMakeVisible(dateVisualiser);
        addChildComponent(floatingButtons);
        floatingButtons.setBounds(0, 0, 40, lineHeight);
        dateVisualiser.setBounds(0, 0, getWidth(), getHeight());
    }
    
    void updateListBox(bool calledFromParent)
    {
        clearAllEntryLabels();
        
        if(calledFromParent)
        {
            wheelY = 0;
            expandDay = -1;
            if(activities[selectedActivityIndex].days.size() == 0 || activities.size() == 0)
                return;
        }
        int d, e;
        Activity& a = activities[selectedActivityIndex];
        pointers.clear();

        {
            if(expandDay != -1 &&
               scrollInt >= expandDay &&
               scrollInt < expandDay + activities[selectedActivityIndex].days[expandDay].entries.size())
            {
                d = expandDay;
                e = scrollInt - expandDay;
            }
            else if(expandDay != -1 &&
                    scrollInt >= expandDay + activities[selectedActivityIndex].days[expandDay].entries.size()) // is bigger than expandDay
            {
                d = scrollInt - (int)activities[selectedActivityIndex].days[expandDay].entries.size()+1;
                e = 0;
            }
            else    //  scrollInt is lower than expandDay
            {
                d = scrollInt;
                e = 0;
            }
        }   //  set values for d and e

        //std::cout << "d " << d << ", e " << e << std::endl;
        for(int i=0; i<maxEntryCount; i++)
        {
            if(d > a.days.size()-1)
                break;
            
            if(d == expandDay)
            {
                for(int ee=e; ee<a.days[d].entries.size(); ee++){
                    if(pointers.size()+1 > maxEntryCount)
                        break;
                    pointers.push_back(&a.days[d].entries[ee]);
                    pointers.back()->dayIndex = d;
                    pointers.back()->triangleType = downTri;
                }
            }
            else{
                if(pointers.size()+1 > maxEntryCount)
                    break;
                pointers.push_back(&a.days[d].entries[0]);
                pointers.back()->dayIndex = d;
                if(a.days[d].entries.size() > 1)  pointers.back()->triangleType = rightFullTri;
                if(a.days[d].entries.size() == 1) pointers.back()->triangleType = rightEmptyTri;
            }
            d++;
        }
        for(int i=0; i<maxEntryCount; i++){
            entry[i]->setVisible(false);
            daySumLabel[i]->setVisible(false);
        }
        for(int p=0; p<pointers.size(); p++)
        {
            entry[p]->setVisible(true);
            entry[p]->setValues(pointers[p]->tStart, pointers[p]->tFinish, pointers[p]->breakTime);
            entry[p]->setBounds(vTab3, p*lineHeight-scrollZeroToOne * (float)lineHeight, entry[p]->getTotalWidth(), lineHeight);
            daySumLabel[p]->setText(a.days[pointers[p]->dayIndex].timeSpentThisDay.getDescription(), juce::dontSendNotification);
            if(yHover >= p*lineHeight-scrollZeroToOne * (float)lineHeight
                && yHover <= (p+1)*lineHeight-scrollZeroToOne * (float)lineHeight
                && xHover > vTab4-60 && xHover < vTab4)
            {
                floatingButtons.setVisible(true);
                floatingButtons.setBounds(vTab4-60, p*lineHeight-scrollZeroToOne * (float)lineHeight, 60, lineHeight);
                floatingButtons.entryEditorPtr = entry[p];
                floatingButtons.entryPtr = pointers[p];
            }
        }
        
        for(int p=0; p<pointers.size(); p++)
        {
            int localYVal = p*lineHeight-scrollZeroToOne * (float)lineHeight;
            if(localYVal < 0 && p<pointers.size()-1 && t1_SameDay_t2(pointers[p]->tStart, pointers[p+1]->tStart) )
                localYVal = 0;
            if( p==0 || !t1_SameDay_t2(pointers[p-1]->tStart, pointers[p]->tStart) ){
                daySumLabel[p]->setVisible(true);
                daySumLabel[p]->setBounds(vTab4, localYVal, 200, lineHeight);
            }
        }
        dateVisualiser.repaint();
    }

    void mouseWheelMove(const juce::MouseEvent& e, const juce::MouseWheelDetails& wheel) override
    {
        if(selectedActivityIndex == -1 || entryIsBeingEdited)
            return;
        wheelY = wheelY - (wheel.deltaY * 2.5);

        if(wheelY < 0)
            wheelY = 0;
        
        scrollInt = (int)wheelY;
        scrollZeroToOne = wheelY - scrollInt;
        updateListBox(false);
    }
    
    void paint(juce::Graphics& g)   override
    {
        g.fillAll(juce::Colour(0xFFA0A0A0));
    }

    void mouseDown(const juce::MouseEvent& e) override
    {
        clickedY = e.y;
        changeExpandDayValue();
    }
    
    void mouseMove(const juce::MouseEvent& e) override
    {
        yHover = e.y;
        xHover = e.x;
        if(entryIsBeingEdited || e.x < vTab4-60 || e.x > vTab4){
            floatingButtons.setVisible(false);
            return;
        }
        updateListBox(false);
    }

private:
    void clearAllEntryLabels()
    {
        for(int i=0; i<maxEntryCount; i++)
        {
            dateLabel[i]->setText("", juce::dontSendNotification);
            entry[i]->setVisible(false);
            daySumLabel[i]->setText("", juce::dontSendNotification);
            *dateString[i] = "";
            *entryString[i] = "";
        }
    }

    class ScrollHandle  :   public juce::Component
    {
    public:
        ScrollHandle()
        {
            setInterceptsMouseClicks(false, false);
        }
        void paint(juce::Graphics& g)   override
        {
            g.setColour(juce::Colour(0xFF00F0A0));
            g.fillRoundedRectangle(0, 0, getWidth(), getHeight(), 5);
        }
    };

    class ScrollHandleStrip  :   public juce::Component
    {
    public:
        ScrollHandleStrip()
        {
            setInterceptsMouseClicks(false, false);
        }
        void paint(juce::Graphics& g)   override
        {
            g.fillAll(juce::Colour(0x3000F0A0));
        }
    };

    class FloatingButtons : public juce::Component
    {
    public:
        juce::TextButton editButton{"edit"};
        juce::DrawableButton deleteButton{"dButton", juce::DrawableButton::ButtonStyle::ImageRaw};
        EntryEditor* entryEditorPtr;
        Entry* entryPtr;
        bool* entryIsBeingEdited;
        int* lineHeight;
        FloatingButtons(){}
        
        void resized() override
        {
            editButton.setBounds(0, 0, 40, getHeight());
            deleteButton.setBounds(40, 0, 20, getHeight());
        }
        
        void initialize(bool* ptr, int* lineHeightPtr)
        {
            entryIsBeingEdited = ptr;
            lineHeight = lineHeightPtr;
            addAndMakeVisible(editButton);
            addAndMakeVisible(deleteButton);
//            juce::Image img;
//            juce::DrawableImage dImg;
//            juce::PNGImageFormat png;
//            juce::File myFile;
//            std::unique_ptr<juce::InputStream> fileStream = myFile.createInputStream();;
//            img = juce::PNGImageFormat().decodeImage (*fileStream);
//            dImg.setImage(img);
            juce::DrawablePath normal;

            juce::Path star1;
            //star1.addEllipse(0, 0, *lineHeight, *lineHeight);
            star1.startNewSubPath(4, 4);
            star1.lineTo(*lineHeight-4, *lineHeight-4);
            star1.startNewSubPath(4, *lineHeight-4);
            star1.lineTo(*lineHeight-4, 4);
            normal.setPath (star1);
            normal.setStrokeFill (juce::Colours::black);
            normal.setStrokeThickness(2.0f);
                        
            deleteButton.setImages(&normal);
            
            deleteButton.onClick = [this]
            {
                juce::Time fT = juce::Time( entryEditorPtr->yearEditor.value, entryEditorPtr->monthEditor.value, entryEditorPtr->dayEditor.value, entryEditorPtr->fHourEditor.value, entryEditorPtr->fMinuteEditor.value);
                activities[selectedActivityIndex].deleteEntry(entryEditorPtr->started, fT);
                callRefreshEntryTree();
            };
            
            editButton.onClick = [this]
            {
                entryEditorPtr->thisIsThePointerToEntry(entryPtr);
                *entryIsBeingEdited = true;
                entryEditorPtr->breakEditor.setVisible(true);
                entryEditorPtr->breakParanthesis.setVisible(true);
                entryEditorPtr->setEditable(15);    //  all fields are editable
                entryEditorPtr->setColours(true, ColoursForEntries::grey, ColoursForEntries::white, ColoursForEntries::white, ColoursForEntries::white, ColoursForEntries::white);
                
                entryEditorPtr->sHourEditor.grabKeyboardFocus();
            };
        }
    };
    
    class DateVisualiser : public juce::Component
    {
    public:
        int lineHeight, vTab1, vTab2, vTab3, vTab4, lastYPos, dayY, monthY, yearY;
        float* scrollZeroToOne;
        juce::Path path;
        std::vector<Entry*>* pointers;
        std::vector<juce::String> monthNames {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
        DateVisualiser(){ setInterceptsMouseClicks(false, false); }
        void paint(juce::Graphics& g) override
        {
            g.setColour(juce::Colour(0xFF606060));
            g.fillRect(0, 0, vTab1, getHeight());
            g.setColour(juce::Colour(0xFF707070));
            g.fillRect(vTab1, 0, vTab2-vTab1, getHeight());
            g.setColour(juce::Colour(0xFF808080));
            g.fillRect(vTab2, 0, vTab3-vTab2, getHeight());
            g.setFont(juce::Font(lineHeight-2));
            g.setColour(juce::Colour(0x20000000));
            g.drawLine(vTab4, 0, vTab4, getHeight());

            for(int p=0; p<(*pointers).size(); p++)
            {
// draw text and lines _______________________________________________________________________________________________________
                if( p==0 || !t1_SameDay_t2((*pointers)[p-1]->tStart, (*pointers)[p]->tStart) ){
                    g.setColour(juce::Colour(0xFF000000));
                    dayY = p*lineHeight-(*scrollZeroToOne) * (float)lineHeight;
                    if(dayY < 0 && p<(*pointers).size()-1 && t1_SameDay_t2((*pointers)[p]->tStart, (*pointers)[p+1]->tStart) )
                        dayY = 0;
                    g.drawText( juce::String((*pointers)[p]->tStart.getDayOfMonth()), vTab2+1, dayY, vTab3-vTab2, lineHeight, juce::Justification::centredLeft, true );
                    g.setColour(juce::Colour(0x20000000));
                    g.drawLine(vTab2, dayY, getWidth(), dayY);
                }
                if( p==0 || (*pointers)[p-1]->tStart.getMonth() != (*pointers)[p]->tStart.getMonth() ){
                    g.setColour(juce::Colour(0xFF000000));
                    monthY = p*lineHeight-*scrollZeroToOne * (float)lineHeight;
                    if(monthY < 0 && p<(*pointers).size()-1 && (*pointers)[p]->tStart.getMonth() == (*pointers)[p+1]->tStart.getMonth() )
                        monthY = 0;
                    g.drawText( monthNames[(*pointers)[p]->tStart.getMonth()], vTab1, monthY, vTab2-vTab1, lineHeight, juce::Justification::centred, true );
                    g.setColour(juce::Colour(0x20000000));
                    g.drawLine(vTab1, monthY, vTab2, monthY);
                }
                if( p==0 || (*pointers)[p-1]->tStart.getYear() != (*pointers)[p]->tStart.getYear() ){
                    g.setColour(juce::Colour(0xFF000000));
                    yearY = p*lineHeight-*scrollZeroToOne * (float)lineHeight;
                    if(yearY < 0 && p<(*pointers).size()-1 && (*pointers)[p]->tStart.getYear() == (*pointers)[p+1]->tStart.getYear() )
                        yearY = 0;
                    g.drawText( juce::String((*pointers)[p]->tStart.getYear()), 0, yearY, vTab3-vTab2, lineHeight, juce::Justification::centred, true );
                    g.setColour(juce::Colour(0x20000000));
                    g.drawLine(0, yearY, vTab1, yearY);
                }

// draw triangle ______________________________________________________________________________________________________________
                g.setColour(juce::Colour(0xFF303030));
                path.clear();
                if((*pointers)[p]->triangleType == rightFullTri || (*pointers)[p]->triangleType == rightEmptyTri ){
                    path.startNewSubPath(vTab3-15, dayY+3);
                    path.lineTo(vTab3-8, dayY+lineHeight/2);
                    path.lineTo(vTab3-15, dayY+lineHeight-3);
                    path.closeSubPath();
                }else if( (*pointers)[p]->triangleType == downTri ){
                    path.startNewSubPath(vTab3-7, dayY+4);
                    path.lineTo(vTab3-12, dayY+lineHeight-4);
                    path.lineTo(vTab3-17, dayY+4);
                    path.closeSubPath();
                }

                if((*pointers)[p]->triangleType == rightEmptyTri)
                    g.strokePath(path, juce::PathStrokeType(1.0f));
                else
                    g.fillPath(path);
            }
        }

        void initialize(const int& lineHeightArg, const int& vTab1Arg, const int& vTab2Arg, const int& vTab3Arg, const int& vTab4Arg, std::vector<Entry*>* pointersArg, float* scrollZeroToOnePtr){
            lineHeight = lineHeightArg;
            vTab1 = vTab1Arg;
            vTab2 = vTab2Arg;
            vTab3 = vTab3Arg;
            vTab4 = vTab4Arg;
            pointers = pointersArg;
            scrollZeroToOne = scrollZeroToOnePtr;
        }
    };

    juce::OwnedArray<juce::String> dateString, entryString;
    juce::OwnedArray<juce::Label> dateLabel, daySumLabel;
    juce::OwnedArray<EntryEditor> entry;
    int maxEntryCount, scrollInt=0, expandDay=-1, clickedY=0;
    float wheelY = 0, scrollZeroToOne=0, shiftDateLabel=0;
    ScrollHandle scrollHandle;
    ScrollHandleStrip scrollHandleStrip;
    FloatingButtons floatingButtons;
    DateVisualiser dateVisualiser;
    bool entryIsBeingEdited = false;
    int yHover=0, xHover=0, lineHeight, vTab1, vTab2, vTab3, vTab4;
    std::vector<Entry*> pointers;
};

//==============================================================================
class BarChart  :   public juce::Component
{
public:
    BarChart()
    {
        addAndMakeVisible(totalLabel);
        totalLabel.setText("totoal", juce::dontSendNotification);
        addAndMakeVisible(progressLabel);
        progressLabel.setText("progress so far", juce::dontSendNotification);
        addAndMakeVisible(targetLabel);
        targetLabel.setText("target so far", juce::dontSendNotification);
        addAndMakeVisible(totalValue);
        addAndMakeVisible(progressValue);
        addAndMakeVisible(targetValue);
    }
    
    void paint(juce::Graphics& g)   override
    {
        if(selectedActivityIndex == -1 || activities.size() == 0)
            return;

        totalLabel.setVisible(true);
        totalLabel.setBounds(getWidth()/3*2+8, getHeight(), 100, 20);
        juce::Point<int> p = getLocalPoint(&totalLabel, juce::Point<int>(0, 0));
        totalLabel.setTransform( juce::AffineTransform::rotation(3.14/2.0f*3, p.x, p.y) );
        progressLabel.setBounds(8, getHeight(), 100, 20);
        p = getLocalPoint(&progressLabel, juce::Point<int>(0, 0));
        progressLabel.setTransform( juce::AffineTransform::rotation(3.14/2.0f*3, p.x, p.y) );
        targetLabel.setBounds(getWidth()/3+8, getHeight(), 100, 20);
        p = getLocalPoint(&targetLabel, juce::Point<int>(0, 0));
        targetLabel.setTransform( juce::AffineTransform::rotation(3.14/2.0f*3, p.x, p.y) );

        juce::Rectangle<int> progress(0, 0, getWidth()/3, getHeight());
        juce::Rectangle<int> target(getWidth()/3, 0, getWidth()/3, getHeight());
        juce::Rectangle<int> total(getWidth()/3*2, 0,  getWidth()/3, getHeight());

        fProgressSoFar = activities[selectedActivityIndex].progressSoFar.inMinutes() / activities[selectedActivityIndex].totalTarget.inMinutes();

        int daysPassed = subtractTwoDates(activities[selectedActivityIndex].begDate, juce::Time::getCurrentTime()).inDays();
        juce::RelativeTime targetSoFar(0);
        for(int d=0; d<daysPassed; d++)
            targetSoFar += activities[selectedActivityIndex].commitmentHDay;
        targetValue.setText( juce::String(targetSoFar.inHours()), juce::dontSendNotification );
        fTargetSoFar = targetSoFar.inMinutes() / activities[selectedActivityIndex].totalTarget.inMinutes();

        float scaleDown = 1;
        if(fProgressSoFar > 1.0f)
            scaleDown = 1.0f / fProgressSoFar;
        if(fTargetSoFar > 1.0f)
            fTargetSoFar = 1;
        fProgressSoFar = (1.0f-fProgressSoFar*scaleDown)*(float)getHeight();
        fTargetSoFar = (1.0f-fTargetSoFar*scaleDown)*(float)getHeight();
        fTotalTarget = (1.0f-scaleDown)*(float)getHeight();

        progress.setY( fProgressSoFar );
        target.setY( fTargetSoFar );
        total.setY( fTotalTarget );
        g.setColour(juce::Colours::red);    //  total Target
        g.fillRect(total);
        g.setColour(juce::Colours::green);  //  progressSoFar
        g.fillRect(progress);
        g.setColour(juce::Colours::blue);   //  targetSoFar
        g.fillRect(target);
        
        totalValue.setBounds(getWidth()/3*2, 0, getWidth()/3, 50);
        targetValue.setBounds(getWidth()/3, 0, getWidth()/3, 50);
        progressValue.setBounds(0, 0, getWidth()/3, 50);
        totalValue.setText( juce::String((int)activities[selectedActivityIndex].totalTarget.inHours()), juce::dontSendNotification );
        progressValue.setText( juce::String((int)activities[selectedActivityIndex].progressSoFar.inHours()), juce::dontSendNotification );
    }
    
private:
    float fProgressSoFar, fTargetSoFar, fTotalTarget;
    juce::Label totalLabel, progressLabel, targetLabel, totalValue, progressValue, targetValue;
};

//==============================================================================
class Tab1  : public juce::Component
{
public:
    Tab1()
    {
        int currentTab, betweenItems = 5, itemHeight = 15, currentY = 70;
        int maxEntryCountInListBox = 12, listBoxLineHeight = 15;

        setSize(600, 600);

        xmlFile = "/Users/mihaitraista/Documents/JUCE/__Mihai_Projects/Time&Money_V1/EntryList.xml";
        if(!xmlFile.exists()){
            std::cout << "EntryList.xml file doesn't exist." << std::endl;
            return;
        }

        juce::XmlDocument xmlDoc(xmlFile);
        entireXml = xmlDoc.getDocumentElement();
        if(entireXml->getNumChildElements())        //  if there is at least one activity in the xml file
            readXmlData();
        
        {
            getLookAndFeel().setColour (juce::TextButton::buttonColourId, juce::Colour(0xff303030));
            getLookAndFeel().setColour (juce::TextButton::buttonOnColourId, juce::Colours::green);
            getLookAndFeel().setColour (juce::TextButton::textColourOffId, juce::Colours::white);
            getLookAndFeel().setColour (juce::TextButton::textColourOnId, juce::Colours::black);
        }   //  lookAndFeel
        
        {
            addChildComponent(newEntryLabel);
            
            addAndMakeVisible(activityComboBox);
            activityComboBox.setBounds(60, 10, 200, 30);
            activityComboBox.setTextWhenNoChoicesAvailable ("No Activity");
            activityComboBox.setTextWhenNothingSelected ("Choose Activity");
            activityComboBox.setEditableText(false);
            activityTextlabel.attachToComponent(&activityComboBox, true);

            addAndMakeVisible(listBoxColumnTitleOne);
            addAndMakeVisible(listBoxColumnTitleTwo);
            addAndMakeVisible(listBoxColumnTitleThree);

            addAndMakeVisible(entryListBox);

            addAndMakeVisible(barChart);

            addAndMakeVisible(addActivityDialog);
            addAndMakeVisible(addActivityButton);
            addAndMakeVisible(editActivityButton);
            addAndMakeVisible(deleteActivityButton);

            listBoxColumnTitleOne.setFont(juce::Font(10, juce::Font::FontStyleFlags::bold));
            listBoxColumnTitleTwo.setFont(juce::Font(10, juce::Font::FontStyleFlags::bold));
            listBoxColumnTitleThree.setFont(juce::Font(10, juce::Font::FontStyleFlags::bold));
            listBoxColumnTitleOne.setColour(juce::Label::backgroundColourId, juce::Colour(0xFF204030));
            listBoxColumnTitleTwo.setColour(juce::Label::backgroundColourId, juce::Colour(0xFF204030));
            listBoxColumnTitleThree.setColour(juce::Label::backgroundColourId, juce::Colour(0xFF204030));
            listBoxColumnTitleOne.setJustificationType(juce::Justification::centred);
            listBoxColumnTitleTwo.setJustificationType(juce::Justification::centred);
            listBoxColumnTitleThree.setJustificationType(juce::Justification::centred);

            newEntryLabel.setText("Add Entry", juce::dontSendNotification);
            newEntryLabel.setJustificationType(juce::Justification::centred);
            newEntryLabel.setFont(juce::Font(20, juce::Font::FontStyleFlags::bold));

            addChildComponent(newEntryEditor);
            newEntryEditor.initialize(26, bigEntryFormat);
            newEntryEditor.setEditable(flagEditableDate | flagEditableStart);

            newEntryEditor.setColours(true, ColoursForEntries::grey, ColoursForEntries::white, ColoursForEntries::white, ColoursForEntries::whiteAlpha, ColoursForEntries::whiteAlpha);

        }   //  Add Child Componentes
            
        {
            addActivityDialog.setBounds(10, 50, 440, 240);
            addActivityButton.setBounds(270, 30, 45, 20);
            editActivityButton.setBounds(320, 30, 45, 20);
            deleteActivityButton.setBounds(370, 30, 45, 20);
            entryListBox.setBounds(10, 360, getWidth()-20, (maxEntryCountInListBox-1)*listBoxLineHeight);
            listBoxColumnTitleOne.setBounds(10, 350, 150, 10);
            listBoxColumnTitleTwo.setBounds(160, 350, 200, 10);
            listBoxColumnTitleThree.setBounds(360, 350, 230, 10);
            barChart.setBounds(460, 10, 130, 270);
            
            currentY = 290;
            newEntryLabel.setBounds(0, currentY, 300, itemHeight);
            currentY += betweenItems + itemHeight;
            currentTab = 100;
            newEntryEditor.setBounds(currentTab, currentY, newEntryEditor.getTotalWidth(), 26);
            //newEntryEditor.setBackgroundColour(juce::Colour(0x20FFFFFF));
            currentTab += newEntryEditor.getTotalWidth() + betweenItems;
        }   //  Set Bounds

        {
            entryListBox.initialize(maxEntryCountInListBox, listBoxLineHeight);
            entryListBox.updateListBox(true);
            addActivityDialog.initialize();
            for(int i=0; i<activities.size(); i++){
                if(activities[i].startedTime.getYear() != 1970)
                    activityComboBox.setSelectedId(i+1);
            }

        }   //  Other Button / Label Settings
        
        {
            activityComboBox.onChange = [this]
            {
                activityChanged();
            };
            
            addActivityButton.onClick = [this]  {
                showHideNewEntryFields(false);
                addActivityDialog.addActivity();
                activityComboBox.setVisible(false);
            };

            editActivityButton.onClick = [this] {
                if(activities.size() == 0)
                    return;
                addActivityDialog.editActivity();
                showHideNewEntryFields(false);
                activityComboBox.setVisible(false);
            };

            deleteActivityButton.onClick = [this]
            {
                if(activities.size() == 0){
                    showHideNewEntryFields(false);
                    return;
                }
                activityComboBox.setVisible(true);

                std::vector<juce::String> comboBoxNames;
                for(int i=0; i<activityComboBox.getNumItems(); i++)
                {
                    if(i != selectedActivityIndex)
                        comboBoxNames.push_back( activityComboBox.getItemText(i) );
                }
                activityComboBox.clear();
                for(int i=0; i<comboBoxNames.size(); i++)
                    activityComboBox.addItem(comboBoxNames[i], i+1);
                activities.erase( activities.begin() + selectedActivityIndex);
                selectedActivityIndex = selectedActivityIndex == 0 ? 0 : selectedActivityIndex-1;
                activityComboBox.setSelectedId(selectedActivityIndex+1);
            };
            
            callUpdateComboBox = [this](AddActivityButtonTypes buttonType) {
                activityComboBox.setVisible(true);
                showHideNewEntryFields(true);

                if(buttonType == addActivityButtonClicked)
                {
                    activityComboBox.addItem(addActivityDialog.underEdit->activityName, activityComboBox.getNumItems()+1);
                    activityComboBox.setSelectedId(activityComboBox.getNumItems());
                }
                else if(buttonType == editActivityButtonClicked)
                {
                    activityComboBox.changeItemText(selectedActivityIndex+1, addActivityDialog.underEdit->activityName);
                    activityComboBox.setSelectedId(selectedActivityIndex+1);
                }
                entryListBox.updateListBox(true);
            };
            
            callRefreshEntryTree = [this]
            {
                barChart.repaint();
                entryListBox.updateListBox(true);
            };

        }   //  Lambda Functions
    }

    ~Tab1()
    {
        entireXml->deleteAllChildElements();

        auto* activityToAdd = new Activity;
        for(int a=0; a<activities.size(); a++)
        {
            *activityToAdd = activities[a];
            entireXml->createNewChildElement("activity");
            activityElement = entireXml->getChildElement(a);
            activityElement->createNewChildElement("activityName");
            activityElement->getChildElement(activityElement->getNumChildElements()-1)->addTextElement(activityToAdd->activityName);
            activityElement->createNewChildElement("isGoal");
            if(activityToAdd->isGoal)
                activityElement->getChildElement(activityElement->getNumChildElements()-1)->addTextElement("true");
            else
                activityElement->getChildElement(activityElement->getNumChildElements()-1)->addTextElement("false");
            activityElement->createNewChildElement("goalDescription");
            activityElement->getChildElement(activityElement->getNumChildElements()-1)->addTextElement(activityToAdd->goalDescription);

            juce::String ISO8601Format = activityToAdd->begDate.toISO8601(true);
            activityElement->createNewChildElement("begDate");
            activityElement->getChildElement(activityElement->getNumChildElements()-1)->addTextElement(ISO8601Format);
            ISO8601Format = activityToAdd->endDate.toISO8601(true);
            activityElement->createNewChildElement("endDate");
            activityElement->getChildElement(activityElement->getNumChildElements()-1)->addTextElement(ISO8601Format);
            ISO8601Format = activityToAdd->startedTime.toISO8601(true);
            activityElement->createNewChildElement("startedTime");
            activityElement->getChildElement(activityElement->getNumChildElements()-1)->addTextElement(ISO8601Format);
            activityElement->createNewChildElement("breakTimeForOpenEntry");
            activityElement->getChildElement(activityElement->getNumChildElements()-1)->addTextElement(juce::String(newEntryEditor.breakEditor.value * 60));

            //breakTimeForOpenEntry

            double commH = (int)activityToAdd->commitmentHDay.inHours();
            double commM = activityToAdd->commitmentHDay.inMinutes();
            commM = commM - commH*60;
            juce::String relTime = juce::String((int)commH) + ":" + juce::String((int)commM);
            activityElement->createNewChildElement("commitmentHDay");
            activityElement->getChildElement(activityElement->getNumChildElements()-1)->addTextElement(relTime);
            relTime = juce::String((int)activityToAdd->totalTarget.inHours());
            activityElement->createNewChildElement("totalTarget");
            activityElement->getChildElement(activityElement->getNumChildElements()-1)->addTextElement(relTime);
            activityElement->createNewChildElement("days");
            activityElement = activityElement->getChildElement(activityElement->getNumChildElements()-1);

            for(int d=0; d<activities[a].days.size(); d++)
            {
                activityElement->createNewChildElement("day");
                dayElement = activityElement->getChildElement(d);

                for(int e=0; e < activityToAdd->days[d].entries.size(); e++)
                {
                    dayElement->createNewChildElement("start");
                    dayElement->getChildElement(dayElement->getNumChildElements()-1)->addTextElement(activityToAdd->days[d].entries[e].tStart.toISO8601(true));
                    dayElement->createNewChildElement("finish");
                    dayElement->getChildElement(dayElement->getNumChildElements()-1)->addTextElement(activityToAdd->days[d].entries[e].tFinish.toISO8601(true));
                }
            }
        }
 
        entireXml->writeTo(xmlFile, juce::XmlElement::TextFormat());
    }

private:
    void activityChanged()
    {
        if(activities.size() == 0){
            addActivityDialog.hideEverything();
            showHideNewEntryFields(false);
            entryListBox.updateListBox(true);
            return;
        }
        selectedActivityIndex = activityComboBox.getSelectedId() - 1;

        if(activities[selectedActivityIndex].startedTime.getYear() != 1970){
            if(newEntryEditor.startFinishButton.getButtonText() == "Start")
                newEntryEditor.startFinishButton.triggerClick();
            newEntryEditor.setDateAndTime(activities[selectedActivityIndex].startedTime);
            newEntryEditor.breakEditor.value = activities[selectedActivityIndex].breakTimeForOpenEntry.inMinutes();
            newEntryEditor.fHourEditor.setValue(juce::Time::getCurrentTime().getHours());
            newEntryEditor.fMinuteEditor.setValue(juce::Time::getCurrentTime().getMinutes());
        }
        else{
            newEntryEditor.sHourEditor.setValue(juce::Time::getCurrentTime().getHours());
            newEntryEditor.sMinuteEditor.setValue(juce::Time::getCurrentTime().getMinutes());
            if(newEntryEditor.startFinishButton.getButtonText() == "Finish")
                newEntryEditor.startFinishButton.triggerClick();
        }

        entryListBox.updateListBox(true);
        barChart.repaint();
        addActivityDialog.changeActivityIndex(selectedActivityIndex);
        showHideNewEntryFields(true);
    }


    void readXmlData()
    {
        for(int a=0; a<entireXml->getNumChildElements(); a++)
        {
            activities.push_back(Activity());
            activityElement = entireXml->getChildElement(a);
            activities[a].activityName = activityElement->getChildByName("activityName")->getAllSubText();
            if(activityElement->getChildByName("isGoal")->getAllSubText() == "true")
                activities[a].isGoal = true;
            else
                activities[a].isGoal = false;

            activities[a].goalDescription = activityElement->getChildByName("goalDescription")->getAllSubText();
            activities[a].begDate = juce::Time::fromISO8601(activityElement->getChildByName("begDate")->getAllSubText());
            activities[a].endDate = juce::Time::fromISO8601(activityElement->getChildByName("endDate")->getAllSubText());
            activities[a].startedTime = juce::Time::fromISO8601(activityElement->getChildByName("startedTime")->getAllSubText());
            activities[a].breakTimeForOpenEntry = juce::RelativeTime(activityElement->getChildByName("breakTimeForOpenEntry")->getAllSubText().getIntValue());
            juce::String cHDay = activityElement->getChildByName("commitmentHDay")->getAllSubText();
            int cHDay_Hours = cHDay.substring(0, 2).getIntValue();
            int cHDay_Minutes = cHDay.substring(3, 5).getIntValue();

            activities[a].commitmentHDay = juce::RelativeTime::hours(cHDay_Hours);
            activities[a].commitmentHDay += juce::RelativeTime::minutes(cHDay_Minutes);
            activities[a].totalTarget = juce::RelativeTime::hours( activityElement->getChildByName("totalTarget")->getAllSubText().getIntValue() );

            activityComboBox.addItem(activities[a].activityName, activityComboBox.getNumItems()+1);
            dayElement = activityElement->getChildByName("days");
            
            for(int d=0; d<dayElement->getNumChildElements(); d++)
            {
                activities[a].days.push_back(Day());
                activities[a].days[d].date = juce::Time::fromISO8601(dayElement->getChildElement(d)->getChildElement(0)->getAllSubText());
                for(int e=0; e<dayElement->getChildElement(d)->getNumChildElements()/2; e++)
                {
                    activities[a].days[d].entries.push_back(Entry());
                    activities[a].days[d].entries[e].tStart = juce::Time::fromISO8601(dayElement->getChildElement(d)->getChildElement(e*2)->getAllSubText());
                    activities[a].days[d].entries[e].tFinish = juce::Time::fromISO8601(dayElement->getChildElement(d)->getChildElement(e*2+1)->getAllSubText());
                }
            }
            activities.back().sort();
        }
        if(activities.size() > 0){
            activityComboBox.setSelectedId(1, juce::dontSendNotification);
            activityChanged();
            showHideNewEntryFields(true);
            entryListBox.updateListBox(true);
        }
    }
    
    void showHideNewEntryFields(bool show)
    {
        if(activities.size() == 0)
            show = false;
        newEntryLabel.setVisible(show);
        newEntryEditor.setVisible(show);
    }

    juce::ComboBox activityComboBox;
    juce::Label activityTextlabel {{}, "Activity"};
    juce::Label listBoxColumnTitleOne {{}, "Day"}, listBoxColumnTitleTwo {{}, "Start - Finish"}, listBoxColumnTitleThree {{}, "Hours"}, newEntryLabel{{}, "New Entry"};
    AddActivityDialog addActivityDialog;
    juce::TextButton addActivityButton {"Add"}, editActivityButton {"Edit"}, deleteActivityButton {"Delete"};
    juce::File xmlFile;
    juce::XmlElement* activityElement;
    juce::XmlElement* dayElement;
    std::unique_ptr<juce::XmlElement> entireXml;
    EntryListBox entryListBox;
    BarChart barChart;
    EntryEditor newEntryEditor;
};
