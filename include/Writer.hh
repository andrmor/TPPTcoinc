#ifndef writer_h
#define writer_h

#include "DataStructures.hh"

#include <string>
#include <vector>

namespace name {
class ofstream;
}

class Writer
{
public:
    Writer(const std::string & FileName, bool BinaryOutput, bool bScintPositions);

    std::string write(std::vector<CoincidencePair> & CoincPairs, std::vector<ScintPosition> & ScintPositions); // returns error string, empty if success

    bool bDebug = true;

private:
    std::ofstream * outStream = nullptr;
    bool bBinaryOutput = false;
    bool bScintPos     = false;
};

#endif // writer_h
