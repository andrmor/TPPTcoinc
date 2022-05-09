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
    Finder2(std::vector<EventRecord> & hits, const Lut & lut);
    ~Finder2();

    void findCoincidences(std::vector<CoincidencePair> & Pairs);

private:
    const Configuration            & Config;
    const std::vector<EventRecord> & Events;
    const Lut                      & LUT;

    Hist1D * histScintMulti    = nullptr;
    Hist1D * histAssemblyMulti = nullptr;

    void groupEventsByAssembly(std::vector<EventRecord> & EventsWithin);
    void killOutsideEnergyWinow(std::vector<EventRecord> & EventsWithin);
    bool isOutsideEnergyWindow(double energy) const;
    void mergeFirstToSecondRecord(const EventRecord & fromRecord, EventRecord & toRecord);

};

#endif // finder2_h
