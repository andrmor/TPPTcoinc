#ifndef clusterer_h
#define clusterer_h

#include "DepositionNodeRecord.hh"

#include <vector>

class Finder
{
public:
    Finder(std::vector<HitRecord> & hits, double TimeWindow);

    void findCoincidences(std::vector<CoincidencePair> & Pairs);

    bool bDebug = true;

private:
    std::vector<HitRecord> & Hits;
    double TimeWindow     = 1e20;

    int findNextHitOutsideTimeWindow(int iCurrentHit);
};

#endif // clusterer_h
