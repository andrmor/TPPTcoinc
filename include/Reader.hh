#ifndef reader_h
#define reader_h

#include "DataStructures.hh"

#include <string>
#include <vector>

class Configuration;

class Reader
{
public:
    Reader(bool EnforceEnergyRange);

    std::string read(std::vector<EventRecord> & Hits); // returns error string, empty if success

private:
    const Configuration & Config;

    std::ifstream * inStream = nullptr;

    bool bEnforceEnergyRange = true;
    bool bDebug = false;
};

#endif // reader_h
