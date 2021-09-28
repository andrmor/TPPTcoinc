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

    out("->Writing coincidences", "(", CoincPairs.size(), ")", "to file...");

    for (const CoincidencePair & cp : CoincPairs)
    {
        float dt    = (cp.Records[0].Time - cp.Records[1].Time) * 1000; // in ps
        float time0 = cp.Records[0].Time * 1e-6; // in ms

        if (Config.BinaryOutput)
        {
            outStream->write((char*)&cp.Records[0].iScint, sizeof(int));
            outStream->write((char*)&cp.Records[1].iScint, sizeof(int));
            outStream->write((char*)&dt,                   sizeof(float));
            outStream->write((char*)&time0,                 sizeof(float));
        }
        else
        {
            *outStream << cp.Records[0].iScint << ','
                       << cp.Records[1].iScint << ','
                       << dt                   << ','
                       << time0                << '\n';
        }
    }

    outStream->close();
    out("\n<-Coincidences are saved\n");

    return "";
}
