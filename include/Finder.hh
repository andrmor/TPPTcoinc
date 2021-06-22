#ifndef clusterer_h
#define clusterer_h

#include "DataStructures.hh"

#include <vector>

class Lut;

class Finder
{
public:
    Finder(std::vector<HitRecord> & hits, double TimeWindow);

    void findCoincidences(std::vector<CoincidencePair> & Pairs, const Lut & LUT, bool bRejectEventsSameHead);

    bool bDebug = false;

private:
    std::vector<HitRecord> & Hits;
    double TimeWindow     = 1e20;

    int findNextHitOutsideTimeWindow(int iCurrentHit);
};

#endif // clusterer_h
