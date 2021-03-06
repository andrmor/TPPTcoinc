#include "Finder1.hh"
#include "Configuration.hh"
#include "Lut.hh"
#include "out.hh"

Finder1::Finder1(std::vector<EventRecord> & hits, const Lut & lut) :
    Config(Configuration::getInstance()),  Hits(hits), LUT(lut) {}

void Finder1::findCoincidences(std::vector<CoincidencePair> & Pairs)
{
    int numSingles    = 0;
    int numBadAngular = 0;

    for (int iCurrentHit = 0; iCurrentHit < Hits.size() - 1; iCurrentHit++)
    {
        const EventRecord & thisHit = Hits[iCurrentHit];
        //out(iCurrentHit, "-->", thisHit.iScint, thisHit.Time);

        int iNextHit = iCurrentHit + 1;
        const EventRecord & nextHit = Hits[iNextHit];

        if (nextHit.Time > thisHit.Time + Config.CoincidenceWindow)
        {
            //large time gap, not interested in this hit
            numSingles++;
            continue;
        }

        //nextHit is within the time window

        if (nextHit.iScint == thisHit.iScint) // paranoic, cannot happen with currently used long integration / dead times
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

int Finder1::findNextHitOutsideTimeWindow(int iCurrentHit)
{
    int iOtherHit = iCurrentHit;

    while ( Hits[iOtherHit].Time < (Hits[iCurrentHit].Time + Config.CoincidenceWindow) ) // dummy first cycle for safety
    {
        iOtherHit++;
        if (iOtherHit >= Hits.size()) break;
    }

    return iOtherHit;
}

