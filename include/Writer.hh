#ifndef writer_h
#define writer_h

#include "DataStructures.hh"

#include <string>
#include <vector>

class Configuration;

class Writer
{
public:
    Writer();

    std::string write(std::vector<CoincidencePair> & CoincPairs); // returns error string, empty if success

    bool bDebug = true;

private:
    const Configuration & Config;

    std::ofstream * outStream = nullptr;
};

#endif // writer_h
