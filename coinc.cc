#include "DataStructures.hh"
#include "Reader.hh"
#include "Finder.hh"
#include "Writer.hh"
#include "out.hh"

#include <string>
#include <vector>

// Input file format:
//
// # iScint x y z               iScint is the scintillator's index and xyz is its center position
// t1 e1
// t2 e2
// ...
// tn en                        tn is the time of the n-th hit [ns], en is the n-th hit energy [MeV]
// # ...                        next scintillator index

//Output file format
//
// x1 y1 z1 x2 y2 z2 dt        xyz is the position of the scintullator's center, d2 is t2 - t1

int main(int argc, char** argv)
{
    // --- Start of user inits ---

    std::string inputFileName  = "/home/andr/WORK/TPPT/BuilderOutput.bin"; bool bBinaryInput  = true;
    //std::string inputFileName  = "/home/andr/WORK/TPPT/BuilderOutput.txt"; bool bBinaryInput = false;
    std::string outputFileName = "/home/andr/WORK/TPPT/CoincPairs.bin";    bool bBinaryOutput = true;
    //std::string outputFileName = "/home/andr/WORK/TPPT/CoincPairs.txt";    bool bBinaryOutput = false;

    double TimeWindow    = 1.12;
    double EnergyWinFrom = 0.511 * 0.95;
    double EnergyWinTo   = 0.511 * 1.05;

    bool bDebug = true;

    // --- End of user inits

    const int numScint = 6144;

    std::vector<HitRecord> Hits;
    std::vector<ScintPosition> ScintPositions;
    ScintPositions.resize(numScint);

    Reader reader(inputFileName, bBinaryInput, EnergyWinFrom, EnergyWinTo);
    reader.bDebug = bDebug;
    std::string error = reader.read(Hits, ScintPositions);
    if (!error.empty())
    {
        out(error);
        exit(2);
    }

    Finder cf(Hits, TimeWindow);
    cf.bDebug = bDebug;
    std::vector<CoincidencePair> Pairs;
    cf.findCoincidences(Pairs);

    Writer writer(outputFileName, bBinaryOutput);
    writer.bDebug = bDebug;
    error = writer.write(Pairs, ScintPositions);
    if (!error.empty())
    {
        out(error);
        exit(3);
    }
}
