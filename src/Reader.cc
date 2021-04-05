#include "Reader.hh"
#include "out.hh"

#include <fstream>
#include <sstream>

Reader::Reader(const std::string & FileName, double MinEnergy, double MaxEnergy) :
    MinEnergy(MinEnergy), MaxEnergy(MaxEnergy)
{
    if (bDebug)
    {
        out("Input file:", FileName);
        out("->Opening stream...");
    }

    inStream = new std::ifstream(FileName);

    if (!inStream->is_open())
    {
        if (bDebug) out("<-Failed!");
        delete inStream; inStream = nullptr;
    }
    else
    {
        if (bDebug) out("<-Open");
    }
}

std::string Reader::read(std::vector<HitRecord> & Hits, std::vector<ScintPosition> & ScintPositions)
{
    if (!inStream)                            return "Cannot open input file";

    if (bDebug) out("->Reading input file...");

    std::string line;

    int iScint;
    char dummy;
    double x, y, z, time, depo;

    while (!inStream->eof())
    {
        getline(*inStream, line);
        if (bDebug) out(line);
        if (line.empty()) break;

        std::stringstream ss(line);

        if (line[0] == '#')
        {
            //new scintillator
            ss >> dummy >> iScint >> x >> y >> z;
            if (iScint < 0 || iScint >= ScintPositions.size()) return "Bad scintillator index";
            ScintPositions[iScint] = ScintPosition(x, y, z);
            if (bDebug) out("Scint #", iScint, x, y, z);
        }
        else
        {
            //another node
            ss >> time >> depo;
            if (bDebug) out("Extracted values:", time, depo);

            if (depo < MinEnergy || depo > MaxEnergy) continue;
            Hits.push_back( HitRecord(iScint, time) );
            if (bDebug) out("  Added to HitRecords");
        }
    }

    inStream->close();
    if (bDebug) out("\n<-Read completed\n");
    return "";
}
