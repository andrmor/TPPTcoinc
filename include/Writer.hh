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

private:
    const Configuration & Config;

    std::ofstream * outStream = nullptr;

    bool bDebug = false;
};

#endif // writer_h
