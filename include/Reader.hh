#ifndef reader_h
#define reader_h

#include "DataStructures.hh"

#include <string>
#include <vector>

class Configuration;

class Reader
{
public:
    Reader();

    std::string read(std::vector<HitRecord> & Hits); // returns error string, empty if success

private:
    const Configuration & Config;

    std::ifstream * inStream = nullptr;

    bool bDebug = false;
};

#endif // reader_h
