#include "Finder.hh"
#include "Configuration.hh"
#include "Lut.hh"
#include "out.hh"

#include <algorithm>

Finder::Finder(std::vector<HitRecord> & hits) :
    Config(Configuration::getInstance()),  Hits(hits) {}

void Finder::findCoincidences(std::vector<CoincidencePair> & Pairs, const Lut & LUT)
{
    int numSingles    = 0;
    int numBadAngular = 0;

    out("-->Sorting hits");
    std::sort(Hits.begin(), Hits.end());

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

        //nextHit is within the time window

        if (nextHit.iScint == thisHit.iScint)
        {
            // same scint, not ineterested in this hit
            iCurrentHit = findNextHitOutsideTimeWindow(iCurrentHit) - 1; //cycle will auto-increment the index
            continue;
        }

        //check that the nextnext is outside the window, otherwise disreguard all within the window
        int iCheckHit = iNextHit + 1;
        if (iCheckHit >= Hits.size() || Hits[iCheckHit].Time > thisHit.Time + Config.CoincidenceWindow)
        {
            if (!Config.RejectSameHead || LUT.isDifferentHeads(thisHit.iScint, nextHit.iScint))
            {
                //found a good coincidence!
                Pairs.push_back(CoincidencePair(thisHit, nextHit));
            }
            else numBadAngular++;

            iCurrentHit = iNextHit; // will be auto-incremented by the cycle
            continue;
        }

        //pass over all piled-up hits
        iCurrentHit = findNextHitOutsideTimeWindow(iCurrentHit) - 1; //cycle will auto-increment the index
    }

    out("Found", Pairs.size(), "coincidences");
    out("  Num singles:", numSingles);
    if (Config.RejectSameHead) out("  Num rejected based on detector head:", numBadAngular);
}

int Finder::findNextHitOutsideTimeWindow(int iCurrentHit)
{
    int iOtherHit = iCurrentHit;

    while ( Hits[iOtherHit].Time < (Hits[iCurrentHit].Time + Config.CoincidenceWindow) ) // dummy first cycle for safety
    {
        iOtherHit++;
        if (iOtherHit >= Hits.size()) break;
    }

    return iOtherHit;
}

