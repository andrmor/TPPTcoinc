#ifndef finder2_h
#define finder2_h

#include "DataStructures.hh"

#include <vector>

class Configuration;
class Lut;
class Hist1D;

class Finder2
{
public:
    Finder2(std::vector<HitRecord> & hits, const Lut & lut);
    ~Finder2();

    void findCoincidences(std::vector<CoincidencePair> & Pairs);

private:
    const Configuration          & Config;
    const std::vector<HitRecord> & Hits;
    const Lut                    & LUT;

    Hist1D * histScintMulti = nullptr;
    Hist1D * histAssemblyMulti = nullptr;

    void groupEventsByAssembly(std::vector<HitRecord> & HitsWithin);
    bool isOutsideEnergyWindow(double energy) const;
    void mergeFirstToSecondRecord(const HitRecord & fromRecord, HitRecord & toRecord);

};

#endif // finder2_h
