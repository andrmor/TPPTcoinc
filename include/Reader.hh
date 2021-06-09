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
    Reader(const std::string & FileName, bool BinaryInput, double MinEnergy, double MaxEnergy);

    std::string read(std::vector<HitRecord> & Hits); // returns error string, empty if success

    bool bDebug = true;

private:
    std::ifstream * inStream = nullptr;
    bool   bBinary = false;

    double MinEnergy = 0;
    double MaxEnergy = 1e10;
};

#endif // reader_h
