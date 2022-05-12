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
    int numSinglesAfterAM = 0; // after assembly merging
    int numMulti = 0;
    int numMultiThree511OrMore = 0;
    int numMultiPassed = 0;             // part of the coincidence count
    int numMultiplesBeforeEnFilter = 0; // coincidence which was multiple before energy filtering, but passed
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
        EventsWithin.push_back(thisEvent);
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

            if (EventsWithin.size() == 1)
            {
                numSinglesAfterAM++;
                iCurrentEvent = iNextEvent;
                continue;
            }
        }

        if (EventsWithin.size() > 2)
        {
            numMulti++;
            if (Config.RejectMultiples == Configuration::All)
            {
                iCurrentEvent = iNextEvent;
                continue;
            }
        }

        bool bWasMultiBeforeEnergyFilter = (EventsWithin.size() > 2);
        //out("->->-->->", EventsWithin.size());
        killOutsideEnergyWinow(EventsWithin);
        if (EventsWithin.size() < 2)
        {
            numBadEnergy++;
            iCurrentEvent = iNextEvent;
            continue;
        }
        if (EventsWithin.size() > 2)
        {
            numMultiThree511OrMore++;
            if (Config.RejectMultiples == Configuration::EnergyWindow)
            {
                iCurrentEvent = iNextEvent;
                continue;
            }
        }

        // could be more than two events, sorting them by energy (largest will be first)
        std::sort(EventsWithin.begin(), EventsWithin.end(),
                  [](const EventRecord & r1, const EventRecord & r2)
                  { return r1.Energy > r2.Energy; });
        //for (auto & r : EventsWithin) r.print();

        if (Config.RejectSameHead)
            if (!LUT.isDifferentHeads(EventsWithin[0].iScint, EventsWithin[1].iScint))
            {
                numSameHead++;
                iCurrentEvent = iNextEvent;
                continue;
            }

        //found a good coincidence!
        Pairs.push_back(CoincidencePair(EventsWithin[0], EventsWithin[1]));
        if (EventsWithin.size() > 2)     numMultiPassed++;
        if (bWasMultiBeforeEnergyFilter) numMultiplesBeforeEnFilter++;
        iCurrentEvent = iNextEvent;
    }


    histScintMulti->report();
    histScintMulti->save(Config.WorkingDirectory + "/CoincScintMultiplicity2.txt");

    if (Config.GroupByAssembly) histAssemblyMulti->report();
    histAssemblyMulti->save(Config.WorkingDirectory + "/CoincAssemblyMultiplicity2.txt");

    out("\n\nFound", Pairs.size(), "coincidences");
    out("  Num singles:", numSingles);
    if (Config.GroupByAssembly) out("  Num singles after assembly merging:", numSinglesAfterAM);
    //if (Config.RejectMultiples == Configuration::All)
    out("  Found multiples:", numMulti, (Config.RejectMultiples == Configuration::None ? " - not restricting" : (Config.RejectMultiples == Configuration::All ? " - killed all" : " - filter dependes on energy") ));
    if (Config.RejectMultiples == Configuration::EnergyWindow)
        out("  Rejected multiples within the energy window:", numMultiThree511OrMore);
    out("  Num bad energy:", numBadEnergy);
    if (Config.RejectSameHead) out("  Num rejected based on detector head:", numSameHead);
    out("  Coincidences which are multiples but passed all filters:", numMultiPassed);
    out("  Coincidences which were multiples before energy filter:",  numMultiplesBeforeEnFilter);
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
    toRecord.Time = std::min(fromRecord.Time, toRecord.Time);                     // !!!*** is it the best possible option?
    toRecord.iScint = (toRecord.Energy > fromRecord.Energy ? toRecord.iScint      // scint with max energy
                                                           : fromRecord.iScint);
    toRecord.Energy += fromRecord.Energy;
}
