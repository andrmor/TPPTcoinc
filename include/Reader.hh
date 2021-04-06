#ifndef reader_h
#define reader_h

#include "DataStructures.hh"

#include <string>
#include <vector>

namespace name {
class ifstream;
}

class Reader
{
public:
    Reader(const std::string & FileName, double MinEnergy, double MaxEnergy);

    std::string read(std::vector<HitRecord> & Hits, std::vector<ScintPosition> & ScintPositions); // returns error string, empty if success

    bool bDebug = true;

private:
    std::ifstream * inStream = nullptr;

    double MinEnergy = 0;
    double MaxEnergy = 1e10;
};

#endif // reader_h
