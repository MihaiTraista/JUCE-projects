#pragma once

enum timeToStringFormats {justHours, hoursAndMinutes, date};
enum ShowStates {showState, addState, editState};
enum TriangleTypes {rightFullTri, rightEmptyTri, downTri, noTri};

juce::RelativeTime subtractTwoDates(const juce::Time& start, const juce::Time& finish)
{
    int startDay = start.getDayOfYear();
    int startHour = start.getHours();
    int startMinute = start.getMinutes();
    int finishDay = finish.getDayOfYear();
    int finishHour = finish.getHours();
    int finishMinute = finish.getMinutes();
    juce::RelativeTime difference;
    difference = juce::RelativeTime::days(finishDay - startDay);
    difference += juce::RelativeTime::hours(finishHour - startHour);
    difference += juce::RelativeTime::minutes(finishMinute - startMinute);
    
    return difference;
}

//==============================================================================
struct Entry
{
    juce::Time tStart, tFinish;
    juce::RelativeTime breakTime;
    int dayIndex;
    TriangleTypes triangleType;
    Entry(){}    
};

//==============================================================================
struct Day
{
    juce::Time date = juce::Time(1);
    std::vector<Entry> entries;
    juce::RelativeTime timeSpentThisDay = juce::RelativeTime(1);
    Day(){}
};

bool e1_SmallerThan_e2(const Entry& e1, const Entry& e2)
{
    juce::Time t1 = e1.tStart;
    juce::Time t2 = e2.tStart;

    if (t1.getHours() < t2.getHours())
        return true;
    else if(t1.getHours() > t2.getHours())
        return false;

    else if (t1.getMinutes() < t2.getMinutes())
        return true;
    else if(t1.getMinutes() > t2.getMinutes())
        return false;
    else
        return false;   //  get rid of warning
}

bool e1_BiggerThan_e2(const Entry& e1, const Entry& e2)
{
    juce::Time t1 = e1.tStart;
    juce::Time t2 = e2.tStart;

    if (t1.getHours() > t2.getHours())
        return true;
    else if(t1.getHours() < t2.getHours())
        return false;

    else if (t1.getMinutes() > t2.getMinutes())
        return true;
    else if(t1.getMinutes() < t2.getMinutes())
        return false;
    else
        return false;   //  get rid of warning
}

bool d1_SmallerThan_d2(const Day& d1, const Day& d2)
{
    juce::Time t1 = d1.date;
    juce::Time t2 = d2.date;
    if (t1.getYear() < t2.getYear())
        return true;
    else if(t1.getYear() > t2.getYear())
        return false;

    else if (t1.getMonth() < t2.getMonth())
        return true;
    else if(t1.getMonth() > t2.getMonth())
        return false;

    else if (t1.getDayOfYear() < t2.getDayOfYear())
        return true;
    else if(t1.getDayOfYear() > t2.getDayOfYear())
        return false;
    else
        return false;   //  get rid of warning
}

bool d1_BiggerThan_d2(const Day& d1, const Day& d2)
{
    juce::Time t1 = d1.date;
    juce::Time t2 = d2.date;
    if (t1.getYear() > t2.getYear())
        return true;
    else if(t1.getYear() < t2.getYear())
        return false;

    else if (t1.getMonth() > t2.getMonth())
        return true;
    else if(t1.getMonth() < t2.getMonth())
        return false;

    else if (t1.getDayOfYear() > t2.getDayOfYear())
        return true;
    else if(t1.getDayOfYear() < t2.getDayOfYear())
        return false;
    else
        return false;   //  get rid of warning
}

bool t1_BiggerThan_t2(const juce::Time& t1, const juce::Time& t2)
{
    if (t1.getYear() > t2.getYear())
        return true;
    else if(t1.getYear() < t2.getYear())
        return false;
    //  if none of the above conditions are met, it means the dates have the same year, so we need to compare the months
    else if (t1.getMonth() > t2.getMonth())
        return true;
    else if(t1.getMonth() < t2.getMonth())
        return false;

    else if (t1.getDayOfYear() > t2.getDayOfYear())
        return true;
    else if(t1.getDayOfYear() < t2.getDayOfYear())
        return false;

    else if (t1.getHours() > t2.getHours())
        return true;
    else if(t1.getHours() < t2.getHours())
        return false;

    else if (t1.getMinutes() > t2.getMinutes())
        return true;
    else if(t1.getMinutes() < t2.getMinutes())
        return false;
    else
        return false;
}

bool t1_SameDay_t2(const juce::Time& t1, const juce::Time& t2)
{
    if (t1.getYear() == t2.getYear() &&
        t1.getMonth() == t2.getMonth() &&
        t1.getDayOfYear() == t2.getDayOfYear())
        return true;
    else
        return false;
}


//==============================================================================
struct Activity
{
    std::vector<Day> days;
    juce::String activityName = "";
    juce::String goalDescription = "";
    bool isGoal;
    juce::RelativeTime commitmentHDay, totalTarget, progressSoFar;
    juce::RelativeTime breakTimeForOpenEntry = juce::RelativeTime(0);
    juce::Time startedTime, begDate, endDate;

    Activity()
        : commitmentHDay(juce::RelativeTime(0)), totalTarget(juce::RelativeTime(0)), progressSoFar(juce::RelativeTime(0)),
        startedTime(juce::Time(0)), begDate(juce::Time(0)), endDate(juce::Time(0))
    {
        startedTime = juce::Time(0);
    }

    void addEntry(const juce::Time& time, const bool& startOrFinish)
    {
        if(startOrFinish == false){
            startedTime = time;
            return;
        }

        if (startOrFinish == true)
        {
            bool dayExists = false;
            for(int d=0; d<days.size(); d++)        //  if this day already exists, add the new entry in it
            {
                if(t1_SameDay_t2(startedTime, days[d].date)){
                    days[d].entries.push_back( Entry() );
                    days[d].entries.back().tStart = startedTime;
                    if( t1_SameDay_t2(time, startedTime) )
                        days[d].entries.back().tFinish = time;
                    else
                        days[d].entries.back().tFinish = juce::Time(startedTime.getYear(), startedTime.getMonth(), startedTime.getDayOfMonth(), 23, 59);
                    days[d].entries.back().breakTime = breakTimeForOpenEntry;
                    dayExists = true;
                    break;
                }
            }
            if(!dayExists){
                days.push_back( Day() );
                days.back().date = startedTime;
                days.back().entries.push_back( Entry() );
                days.back().entries.back().tStart = startedTime;
                if( t1_SameDay_t2(time, startedTime) )
                    days.back().entries.back().tFinish = time;
                else
                    days.back().entries.back().tFinish = juce::Time(startedTime.getYear(), startedTime.getMonth(), startedTime.getDayOfMonth(), 23, 59);
                days.back().entries.back().breakTime = breakTimeForOpenEntry;
            }
        }
        
        if( !t1_SameDay_t2(time, startedTime) )
        {
            std::cout << time.toString(true, true) << ", " << startedTime.toString(true, true) << std::endl;
            bool dayExists = false;
            for(int d=0; d<days.size(); d++)        //  if this day already exists, add the new entry in it
            {
                if(t1_SameDay_t2(time, days[d].date)){
                    days[d].entries.push_back( Entry() );
                    days[d].entries.back().tStart = juce::Time(time.getYear(), time.getMonth(), time.getDayOfMonth(), 0, 0);
                    days[d].entries.back().tFinish = time;
                    days[d].entries.back().breakTime = breakTimeForOpenEntry;
                    dayExists = true;
                    break;
                }
            }
            if(!dayExists){
                days.push_back( Day() );
                days.back().date = time;
                days.back().entries.push_back( Entry() );
                days.back().entries.back().tStart = juce::Time(time.getYear(), time.getMonth(), time.getDayOfMonth(), 0, 0);
                days.back().entries.back().tFinish = time;
                days.back().entries.back().breakTime = breakTimeForOpenEntry;
            }
        }

        startedTime = juce::Time(0);
        sort();
    }

    void addBreak(const juce::RelativeTime& rT){    breakTimeForOpenEntry = rT;    }
    
    void deleteEntry(const juce::Time sT, const juce::Time fT)
    {
        std::cout << "sT " << sT.toString(true, true) << ", " << "fT " << fT.toString(true, true) << std::endl;
        for(int d=0; d<days.size(); d++)
        {
            if(t1_SameDay_t2(sT, days[d].date))
            {
                for(int e=0; e<days[d].entries.size(); e++)
                {
                    std::cout << "tStart " << days[d].entries[e].tStart.toString(true, true) << ", " << "tFinish " << days[d].entries[e].tFinish.toString(true, true) << std::endl;
                    if(days[d].entries[e].tStart.getHours() == sT.getHours()
                       && days[d].entries[e].tStart.getMinutes() == sT.getMinutes()
                       && days[d].entries[e].tFinish.getHours() == fT.getHours()
                       && days[d].entries[e].tFinish.getMinutes() == fT.getMinutes() )
                    {
                        days[d].entries.erase(days[d].entries.begin() + e);
                        if(days[d].entries.size() == 0){
                            days.erase(days.begin() + d);
                            break;
                        }
                    }
                }
            }
        }
    }

    void sort()
    {
        std::sort(days.begin(), days.end(), d1_BiggerThan_d2);
        for(Day& d : days)
            std::sort(d.entries.begin(), d.entries.end(), e1_BiggerThan_e2);

        progressSoFar = juce::RelativeTime(0);
        for(int d=0; d<days.size(); d++)
        {
            days[d].timeSpentThisDay = juce::RelativeTime(0);
            for(int e=0; e<days[d].entries.size(); e++)
                days[d].timeSpentThisDay += (subtractTwoDates(days[d].entries[e].tStart, days[d].entries[e].tFinish)) - days[d].entries[e].breakTime;
            progressSoFar += days[d].timeSpentThisDay;
        }
    }
};

int selectedActivityIndex = 0;
std::vector<Activity> activities;
