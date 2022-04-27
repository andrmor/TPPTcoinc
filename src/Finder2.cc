#include "Finder2.hh"
#include "Configuration.hh"
#include "Lut.hh"
#include "out.hh"

#include <algorithm>

Finder2::Finder2(std::vector<HitRecord> & hits, const Lut & lut) :
    Config(Configuration::getInstance()),  Hits(hits), LUT(lut) {}

void Finder2::findCoincidences(std::vector<CoincidencePair> & Pairs)
{
    int numSingles    = 0;
    int numMulti = 0;
    int numBadEnergy = 0;
    int numSameHead = 0;

    std::vector<HitRecord> HitsWithin;

    out("-->Finding coincidences");
    for (int iCurrentHit = 0; iCurrentHit < Hits.size() - 1; iCurrentHit++)
    {
        const HitRecord & thisHit = Hits[iCurrentHit];
        //out(iCurrentHit, "-->", thisHit.iScint, thisHit.Time);

        int iNextHit = iCurrentHit + 1;
        const HitRecord & nextHit = Hits[iNextHit];

        if (nextHit.Time > thisHit.Time + Config.CoincidenceWindow)
        {
            //large time gap, not interested in this hit
            numSingles++;
            continue;
        }

        HitsWithin.clear();
        HitsWithin.push_back(Hits[iCurrentHit]);
        do
        {
            HitsWithin.push_back(Hits[iNextHit]);
            iNextHit++;
        }
        while (iNextHit < Hits.size() && Hits[iNextHit].Time < thisHit.Time + Config.CoincidenceWindow);

        if (HitsWithin.size() > 2)
            groupEventsByAssembly(HitsWithin);

        if (HitsWithin.size() == 1)  // paranoic
        {
            numSingles++;
            iCurrentHit = iNextHit;
            continue;
        }
        else if (HitsWithin.size() > 2)
        {
            numMulti++;
            iCurrentHit = iNextHit;
            continue;
        }

        // two hits in the vector

        if (isOutsideEnergyWindow(HitsWithin.front().Energy) || isOutsideEnergyWindow(HitsWithin.back().Energy))
        {
            numBadEnergy++;
            iCurrentHit = iNextHit;
            continue;
        }

        if (Config.RejectSameHead && LUT.isDifferentHeads(HitsWithin.front().iScint, HitsWithin.back().iScint))
        {
            numSameHead++;
            iCurrentHit = iNextHit;
            continue;
        }

        //found a good coincidence!
        Pairs.push_back(CoincidencePair(thisHit, nextHit));
        iCurrentHit = iNextHit;
    }

    out("Found", Pairs.size(), "coincidences");
    out("  Num singles:", numSingles);
    out("  Num multiples:", numMulti);
    out("  Num bad energy:", numBadEnergy);
    if (Config.RejectSameHead) out("  Num rejected based on detector head:", numSameHead);
}

void Finder2::groupEventsByAssembly(std::vector<HitRecord> & HitsWithin)
{
    for (size_t iThis = HitsWithin.size()-1; iThis > 1; iThis--)
    {
        const HitRecord & ThisRecord = HitsWithin[iThis];
        const int thisAssembly = LUT.getAssemblyIndex(ThisRecord.iScint);

        bool bMerged = false;
        for (size_t iTester = 0; iTester < iThis; iTester++)
        {
            HitRecord & TestRecord = HitsWithin[iTester];
            if ( thisAssembly != LUT.getAssemblyIndex(TestRecord.iScint) ) continue;

            mergeFirstToSecondRecord(ThisRecord, TestRecord);
            bMerged = true;
            break;
        }

        if (bMerged) HitsWithin.erase(HitsWithin.begin() + iThis);
    }
}

bool Finder2::isOutsideEnergyWindow(double energy) const
{
    return (energy < Config.EnergyFrom || energy > Config.EnergyTo);
}

void Finder2::mergeFirstToSecondRecord(const HitRecord & fromRecord, HitRecord & toRecord)
{
    toRecord.Time = std::min(fromRecord.Time, toRecord.Time);
    toRecord.iScint = (toRecord.Energy < fromRecord.Energy ? toRecord.iScint
                                                           : fromRecord.iScint);
    toRecord.Energy += fromRecord.Energy;
}
