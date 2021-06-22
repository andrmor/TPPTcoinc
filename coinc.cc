#include "DataStructures.hh"
#include "Lut.hh"
#include "Reader.hh"
#include "Finder.hh"
#include "Writer.hh"
#include "out.hh"

#include <string>
#include <vector>

// Input file format:
//
// # iScint                     iScint is the scintillator's index
// t1 e1                        t is the time of the hit [ns], e is the event energy [MeV]
// t2 e2
// ...
// tn en
// # ...                        next scintillator index

//Output file format
//
// i1 i2 dt                     i1 and i2 are the scintullator's indexes, dt is delta time (t2 - t1)

int main(int argc, char** argv)
{
    // --- Start of user inits ---

    std::string dir            = "/home/andr/WORK/TPPT";

    std::string inputFileName  = "/BuilderOutput.bin"; bool bBinaryInput  = true;
    //std::string inputFileName  = "BuilderOutput.txt";  bool bBinaryInput = false;
    //std::string outputFileName = "CoincPairs.bin";     bool bBinaryOutput = true;
    std::string outputFileName = "CoincPairs.txt";     bool bBinaryOutput = false;

    std::string lutFileName    = "LUT.txt";

    bool   bRejectEventsSameHead = true;

    double CoincidenceWindow = 4.0;      // [ns]

    double TimeFrom = 4.0 * 1e6;         // 4 ms
    double TimeTo   = 5.0 * 60.0 * 1e9;  // 5 min

    double EnergyWinFrom = 0.511 * 0.95; // [MeV]
    double EnergyWinTo   = 0.511 * 1.05;

    bool bDebug = false;

    // --- End of user inits

    inputFileName  = dir + '/' + inputFileName;
    outputFileName = dir + '/' + outputFileName;
    lutFileName    = dir + '/' + lutFileName;

    Lut LUT(lutFileName);

    std::vector<HitRecord> Hits;

    Reader reader(inputFileName, bBinaryInput, EnergyWinFrom, EnergyWinTo, TimeFrom, TimeTo);
    reader.bDebug = bDebug;
    std::string error = reader.read(Hits);
    if (!error.empty())
    {
        out(error);
        exit(2);
    }

    Finder cf(Hits, CoincidenceWindow);
    cf.bDebug = bDebug;
    std::vector<CoincidencePair> Pairs;
    cf.findCoincidences(Pairs, LUT, bRejectEventsSameHead);

    Writer writer(outputFileName, bBinaryOutput);
    writer.bDebug = bDebug;
    error = writer.write(Pairs);
    if (!error.empty())
    {
        out(error);
        exit(3);
    }
}
