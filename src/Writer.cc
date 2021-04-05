#include "Writer.hh"
#include "out.hh"

#include <fstream>
#include <iostream>
#include <sstream>

Writer::Writer(const std::__cxx11::string &FileName)
{
    if (bDebug)
    {
        out("Output file:", FileName);
        out("->Opening stream...");
    }

    outStream = new std::ofstream(FileName);

    if (!outStream->is_open())
    {
        if (bDebug) out("<-Failed!");
        delete outStream; outStream = nullptr;
    }
    else
    {
        if (bDebug) out("<-Open");
    }
}

std::string Writer::write(std::vector<CoincidencePair> & CoincPairs, std::vector<ScintPosition> & ScintPositions)
{
    if (!outStream) return "Cannot open output file";

    if (bDebug) out("->Writing coincidences to file...");

    for (const CoincidencePair & cp : CoincPairs)
    {
        for (int iP = 0; iP < 2; iP++)
        {
            const int & iScint = cp.Records[iP].iScint;
            const ScintPosition & s = ScintPositions[iScint];
            *outStream << s.pos[0] << " " << s.pos[1] << " " << s.pos[2] << " " << cp.Records[iP].Time << '\n';
        }
    }

    outStream->close();
    if (bDebug) out("\n<-Coincidences are saved\n");
    return "";
}
