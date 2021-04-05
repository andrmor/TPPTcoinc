#include "DepositionNodeRecord.hh"
#include "Reader.hh"
#include "Finder.hh"
#include "Writer.hh"
#include "out.hh"

#include <string>
#include <vector>

// Input file format:
//
// # iScint x y z                   iScint is the scintillator's index and xyz is its center position
// t1 e1
// t2 e2
// ...
// tn en                            tn is the time of the n-th hit [ns], en is the n-th hit energy [MeV]
// # ...                            next scintillator index

//Output file format
//
// Contains consecutive pairs of events (two lines per pair)
// x1 y1 z1 T1                      first gamma:  xyz is the position of the scintullator's center, T is the event time
// x2 y2 z2 T2                      second gamma
// ...                              next pair of gammas

int main(int argc, char** argv)
{
    // --- Start of user inits ---

    std::string inputFileName  = "/home/andr/WORK/TPPT/TPPToutput-hits.txt";
    std::string outputFileName = "/home/andr/WORK/TPPT/TPPToutput-coincidences.txt";

    double TimeWindow    = 4.0;
    double EnergyWinFrom = 0.461;
    double EnergyWinTo   = 0.561;

    bool bDebug = true;

    // --- End of user inits

    const int numScint = 6144;

    std::vector<HitRecord> Hits;
    std::vector<ScintPosition> ScintPositions;
    ScintPositions.resize(numScint);

    Reader reader(inputFileName, EnergyWinFrom, EnergyWinTo);
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

    Writer writer(outputFileName);
    writer.bDebug = bDebug;
    error = writer.write(Pairs, ScintPositions);
    if (!error.empty())
    {
        out(error);
        exit(3);
    }
}
