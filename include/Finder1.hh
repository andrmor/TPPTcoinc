#ifndef finder1_h
#define finder1_h

#include "DataStructures.hh"

#include <vector>

class Configuration;
class Lut;

class Finder1
{
public:
    Finder1(std::vector<HitRecord> & hits, const Lut & lut);

    void findCoincidences(std::vector<CoincidencePair> & Pairs);

private:
    const Configuration    & Config;
    std::vector<HitRecord> & Hits;
    const Lut              & LUT;

    bool bDebug = false;

    int findNextHitOutsideTimeWindow(int iCurrentHit);
};

#endif // finder1_h
