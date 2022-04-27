#ifndef finder2_h
#define finder2_h

#include "DataStructures.hh"

#include <vector>

class Configuration;
class Lut;

class Finder2
{
public:
    Finder2(std::vector<HitRecord> & hits, const Lut & lut);

    void findCoincidences(std::vector<CoincidencePair> & Pairs);

private:
    const Configuration          & Config;
    const std::vector<HitRecord> & Hits;
    const Lut                    & LUT;

    void groupEventsByAssembly(std::vector<HitRecord> & HitsWithin);
    bool isOutsideEnergyWindow(double energy) const;
    void mergeFirstToSecondRecord(const HitRecord & fromRecord, HitRecord & toRecord);

};

#endif // finder2_h
