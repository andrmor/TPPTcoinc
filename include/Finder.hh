#ifndef clusterer_h
#define clusterer_h

#include "DataStructures.hh"

#include <vector>

class Configuration;
class Lut;

class Finder
{
public:
    Finder(std::vector<HitRecord> & hits);

    void findCoincidences(std::vector<CoincidencePair> & Pairs, const Lut & LUT);

private:
    const Configuration    & Config;
    std::vector<HitRecord> & Hits;

    bool bDebug = false;

    int findNextHitOutsideTimeWindow(int iCurrentHit);
};

#endif // clusterer_h
