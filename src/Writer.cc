#include "Writer.hh"
#include "out.hh"

#include <fstream>
#include <iostream>
#include <sstream>

Writer::Writer(const std::string & FileName, bool BinaryOutput) : bBinaryOutput(BinaryOutput)
{
    if (bDebug)
    {
        out("Output file:", FileName);
        out("->Opening stream...");
    }

    outStream = new std::ofstream;
    if (bBinaryOutput)
        outStream->open(FileName, std::ios::out | std::ios::binary);
    else
        outStream->open(FileName);

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

    if (bDebug) out("->Writing coincidences", "(", CoincPairs.size(), ")", "to file...");

    for (const CoincidencePair & cp : CoincPairs)
    {
        const int & iScint0 = cp.Records[0].iScint;
        const int & iScint1 = cp.Records[1].iScint;
        const ScintPosition & p0 = ScintPositions[iScint0];
        const ScintPosition & p1 = ScintPositions[iScint1];
        double dt = cp.Records[1].Time - cp.Records[0].Time;

        if (bBinaryOutput)
        {
            outStream->write((char*)p0.pos, 3 * sizeof(double));
            outStream->write((char*)p1.pos, 3 * sizeof(double));
            outStream->write((char*)&dt,        sizeof(double));
        }
        else
        {
            *outStream << p0.pos[0] << " " << p0.pos[1] << " " << p0.pos[2] << " "
                       << p1.pos[0] << " " << p1.pos[1] << " " << p1.pos[2] << " "
                       << dt << '\n';
        }
    }

    outStream->close();
    if (bDebug) out("\n<-Coincidences are saved\n");
    return "";
}
