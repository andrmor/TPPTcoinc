#include "Finder3.hh"
#include "Configuration.hh"
#include "Lut.hh"
#include "out.hh"
#include "Hist1D.hh"

#include <algorithm>

Finder3::Finder3(std::vector<EventRecord> & hits, const Lut & lut) :
    Config(Configuration::getInstance()),  Events(hits), LUT(lut)
{
    histScintMulti    = new Hist1D(14, 1, 15);
    histAssemblyMulti = new Hist1D(14, 1, 15);
}

Finder3::~Finder3()
{
    delete histScintMulti;
    delete histAssemblyMulti;
}

void Finder3::findCoincidences(std::vector<CoincidencePair> & Pairs)
{
    if (Events.size() < 2) return;

    int numSingles = 0;
    int numMulti = 0;
    int num3plus = 0;
    int numBadEnergy = 0;
    int numSameHead = 0;

    std::vector<EventRecord> EventsWithin;

    double TimeGroupStop = Events[0].Time + Config.GroupingTime;

    size_t iEvent = 0;
    do
    {
        EventsWithin.clear();
        for (; iEvent < Events.size(); iEvent++)
        {
            if (Events[iEvent].Time > TimeGroupStop) break;
            EventsWithin.push_back(Events[iEvent]);
        }

        TimeGroupStop += Config.GroupingTime;

        // ---

        histScintMulti->fill(EventsWithin.size());
        if (Config.GroupByAssembly)
        {
            if (EventsWithin.size() > 1) groupEventsByAssembly(EventsWithin);
            histAssemblyMulti->fill(EventsWithin.size());
        }


        if (EventsWithin.empty()) continue;
        if (EventsWithin.size() == 1)
        {
            numSingles++;
            continue;
        }

        killOutsideEnergyWinow(EventsWithin);
        if (EventsWithin.size() == 1)
        {
            numBadEnergy++;
            continue;
        }
        if (EventsWithin.size() > 2)
        {
            num3plus++;
            continue;
        }

        // two events of 511 keV in the vector

        if (Config.RejectSameHead && !LUT.isDifferentHeads(EventsWithin.front().iScint, EventsWithin.back().iScint))
        {
            numSameHead++;
            continue;
        }

        //found a good coincidence!
        Pairs.push_back(CoincidencePair(EventsWithin.front(), EventsWithin.back()));

        // ---
    }
    while (iEvent < Events.size());

    histScintMulti->report();
    histScintMulti->save(Config.WorkingDirectory + "/CoincScintMultiplicity3.txt");

    if (Config.GroupByAssembly) histAssemblyMulti->report();
    histAssemblyMulti->save(Config.WorkingDirectory + "/CoincAssemblyMultiplicity3.txt");

    out("Found", Pairs.size(), "coincidences");
    out("  Num singles:", numSingles);
    if (Config.RejectMultiples) out("  Num multiples:", numMulti);
    else                        out("  Num 3+ 511 keV:", num3plus);
    out("  Num bad energy:", numBadEnergy);
    if (Config.RejectSameHead) out("  Num rejected based on detector head:", numSameHead);
}

void Finder3::groupEventsByAssembly(std::vector<EventRecord> & EventsWithin)
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

void Finder3::killOutsideEnergyWinow(std::vector<EventRecord> & EventsWithin)
{
    for (int iEvent = (int)EventsWithin.size()-1; iEvent >= 0; iEvent--)
    {
        if ( isOutsideEnergyWindow(EventsWithin[iEvent].Energy) )
            EventsWithin.erase(EventsWithin.begin() + iEvent);
    }
}

bool Finder3::isOutsideEnergyWindow(double energy) const
{
    return (energy < Config.EnergyFrom || energy > Config.EnergyTo);
}

void Finder3::mergeFirstToSecondRecord(const EventRecord & fromRecord, EventRecord & toRecord)
{
    toRecord.Time = std::min(fromRecord.Time, toRecord.Time);
    toRecord.iScint = (toRecord.Energy > fromRecord.Energy ? toRecord.iScint
                                                           : fromRecord.iScint);
    toRecord.Energy += fromRecord.Energy;
}
