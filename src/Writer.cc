#include "Writer.hh"
#include "Configuration.hh"
#include "out.hh"

#include <fstream>
#include <iostream>
#include <sstream>

Writer::Writer() : Config(Configuration::getInstance())
{
    std::string FileName = Config.WorkingDirectory + '/' + Config.OutputFileName;
    out("Output file:", FileName);
    out("->Opening stream...");

    outStream = new std::ofstream;
    if (Config.BinaryOutput) outStream->open(FileName, std::ios::out | std::ios::binary);
    else                     outStream->open(FileName);

    if (!outStream->is_open())
    {
        out("<-Failed!");
        delete outStream; outStream = nullptr;
    }
    else
        out("<-Open");
}

std::string Writer::write(std::vector<CoincidencePair> & CoincPairs)
{
    if (!outStream) return "Cannot open output file";

    if (bDebug) out("->Writing coincidences", "(", CoincPairs.size(), ")", "to file...");

    for (const CoincidencePair & cp : CoincPairs)
    {
        double dt = cp.Records[1].Time - cp.Records[0].Time;

        if (Config.BinaryOutput)
        {
            outStream->write((char*)&cp.Records[0].iScint, sizeof(int));
            outStream->write((char*)&cp.Records[1].iScint, sizeof(int));
            outStream->write((char*)&dt, sizeof(double));
        }
        else
        {
            *outStream << cp.Records[0].iScint << " "
                       << cp.Records[1].iScint << " "
                       << dt << std::endl;
        }
    }

    outStream->close();
    out("\n<-Coincidences are saved\n");

    return "";
}
