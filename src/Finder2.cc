#include "Finder2.hh"
#include "Configuration.hh"
#include "Lut.hh"
#include "out.hh"
#include "Hist1D.hh"

#include <algorithm>

Finder2::Finder2(std::vector<EventRecord> & hits, const Lut & lut) :
    Config(Configuration::getInstance()),  Events(hits), LUT(lut)
{
    histScintMulti    = new Hist1D(14, 1, 15);
    histAssemblyMulti = new Hist1D(14, 1, 15);
}

Finder2::~Finder2()
{
    delete histScintMulti;
    delete histAssemblyMulti;
}

void Finder2::findCoincidences(std::vector<CoincidencePair> & Pairs)
{
    int numSingles = 0;
    int numMulti = 0;
    int num3plus = 0;
    int numBadEnergy = 0;
    int numSameHead = 0;

    std::vector<EventRecord> EventsWithin;

    for (int iCurrentEvent = 0; iCurrentEvent < (int)Events.size() - 1; iCurrentEvent++)
    {
        const EventRecord & thisEvent = Events[iCurrentEvent];

        int iNextEvent = iCurrentEvent + 1;
        const EventRecord & nextEvent = Events[iNextEvent];

        if (nextEvent.Time > thisEvent.Time + Config.CoincidenceWindow)
        {
            numSingles++;
            histScintMulti->fill(1.0);
            continue;
        }

        EventsWithin.clear();
        EventsWithin.push_back(Events[iCurrentEvent]);
        do
        {
            EventsWithin.push_back(Events[iNextEvent]);
            iNextEvent++;
        }
        while (iNextEvent < Events.size() && Events[iNextEvent].Time < thisEvent.Time + Config.CoincidenceWindow);
        histScintMulti->fill(EventsWithin.size());

        if (Config.GroupByAssembly)
        {
            if (EventsWithin.size() > 1) groupEventsByAssembly(EventsWithin);
            histAssemblyMulti->fill(EventsWithin.size());
        }

        if (EventsWithin.size() == 1)
        {
            numSingles++;
            iCurrentEvent = iNextEvent;
            continue;
        }
        if (EventsWithin.size() > 2 && Config.RejectMultiples)
        {
            numMulti++;
            iCurrentEvent = iNextEvent;
            continue;
        }

        killOutsideEnergyWinow(EventsWithin);
        if (EventsWithin.size() == 1)
        {
            numBadEnergy++;
            iCurrentEvent = iNextEvent;
            continue;
        }
        if (EventsWithin.size() > 2)
        {
            num3plus++;
            iCurrentEvent = iNextEvent;
            continue;
        }

        // two events of 511 keV in the vector

        if (Config.RejectSameHead && !LUT.isDifferentHeads(EventsWithin.front().iScint, EventsWithin.back().iScint))
        {
            numSameHead++;
            iCurrentEvent = iNextEvent;
            continue;
        }

        //found a good coincidence!
        Pairs.push_back(CoincidencePair(thisEvent, nextEvent));
        iCurrentEvent = iNextEvent;
    }

    if (Config.FinderMethod == 2)
    {
        histScintMulti->report();
        histScintMulti->save(Config.WorkingDirectory + "/CoincScintMultiplicity.txt");

        if (Config.GroupByAssembly) histAssemblyMulti->report();
        histAssemblyMulti->save(Config.WorkingDirectory + "/CoincAssemblyMultiplicity.txt");
    }

    out("Found", Pairs.size(), "coincidences");
    out("  Num singles:", numSingles);
    if (Config.RejectMultiples) out("  Num multiples:", numMulti);
    else                        out("  Num 3+ 511 keV:", num3plus);
    out("  Num bad energy:", numBadEnergy);
    if (Config.RejectSameHead) out("  Num rejected based on detector head:", numSameHead);
}

void Finder2::groupEventsByAssembly(std::vector<EventRecord> & EventsWithin)
{
    for (int iThis = (int)EventsWithin.size()-1; iThis > 0; iThis--)
    {
        const EventRecord & ThisRecord = EventsWithin[iThis];
        const int thisAssembly = LUT.getAssemblyIndex(ThisRecord.iScint);

        bool bMerged = false;
        for (int iTester = 0; iTester < iThis; iTester++)
        {
            EventRecord & TestRecord = EventsWithin[iTester];
            if ( thisAssembly != LUT.getAssemblyIndex(TestRecord.iScint) ) continue;

            mergeFirstToSecondRecord(ThisRecord, TestRecord);
            bMerged = true;
            break;
        }

        if (bMerged) EventsWithin.erase(EventsWithin.begin() + iThis);
    }
}

void Finder2::killOutsideEnergyWinow(std::vector<EventRecord> & EventsWithin)
{
    for (int iEvent = (int)EventsWithin.size()-1; iEvent >= 0; iEvent--)
    {
        if ( isOutsideEnergyWindow(EventsWithin[iEvent].Energy) )
            EventsWithin.erase(EventsWithin.begin() + iEvent);
    }
}

bool Finder2::isOutsideEnergyWindow(double energy) const
{
    return (energy < Config.EnergyFrom || energy > Config.EnergyTo);
}

void Finder2::mergeFirstToSecondRecord(const EventRecord & fromRecord, EventRecord & toRecord)
{
    toRecord.Time = std::min(fromRecord.Time, toRecord.Time);
    toRecord.iScint = (toRecord.Energy < fromRecord.Energy ? toRecord.iScint
                                                           : fromRecord.iScint);
    toRecord.Energy += fromRecord.Energy;
}
