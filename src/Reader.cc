#include "Reader.hh"
#include "Configuration.hh"
#include "out.hh"

#include <fstream>
#include <sstream>

Reader::Reader(bool EnforceTimeEnergy) :
    Config(Configuration::getInstance()), bEnforceTimeEnergy(EnforceTimeEnergy)
{
    std::string FileName = Config.WorkingDirectory + '/' + Config.InputFileName;
    out("Input file:", FileName);

    out("->Opening stream...");
    if (Config.BinaryInput) inStream = new std::ifstream(FileName, std::ios::in | std::ios::binary);
    else                    inStream = new std::ifstream(FileName);

    if (!inStream->is_open() || inStream->fail() || inStream->bad())
    {
        out("<-Failed!");
        delete inStream; inStream = nullptr;
    }
    else
        out("<-Open");
}

std::string Reader::read(std::vector<EventRecord> & Hits)
{
    if (!inStream) return "Cannot open input file";

    out("->Reading input file...");

    int iScint = 0;
    if (Config.BinaryInput)
    {
        char ch;
        while (inStream->get(ch))
        {
            if (inStream->eof()) break;

            if (ch == (char)0xEE)
            {
                inStream->read((char*)&iScint, sizeof(int));
            }
            else if (ch == (char)0xFF)
            {
                EventRecord hit(iScint, 0, 0);
                inStream->read((char*)&hit.Time,   sizeof(double));
                inStream->read((char*)&hit.Energy, sizeof(double));

                if (bDebug) out("Extracted values:", hit.Time, hit.Energy);

                if (bEnforceTimeEnergy)
                {
                    if (hit.Energy < Config.EnergyFrom || hit.Energy > Config.EnergyTo) continue;
                    if (hit.Time   < Config.TimeFrom   || hit.Time   > Config.TimeTo)   continue;
                }

                Hits.push_back(hit);
            }
        }
    }
    else
    {
        std::string line;

        char dummy;
        double time, depo;

        while (!inStream->eof())
        {
            getline(*inStream, line);
            if (bDebug) out(line);
            if (line.empty()) break;

            std::stringstream ss(line);

            if (line[0] == '#')
            {
                //new scintillator
                ss >> dummy >> iScint;
            }
            else
            {
                //another node
                ss >> time >> depo;
                if (bDebug) out("Extracted time and depo values:", time, depo);

                if (bEnforceTimeEnergy)
                {
                    if (depo < Config.EnergyFrom || depo > Config.EnergyTo) continue;
                    if (time < Config.TimeFrom   || time > Config.TimeTo)   continue;
                }

                Hits.push_back( EventRecord(iScint, time, depo) );
                if (bDebug) out("  Added to HitRecords");
            }
        }
    }

    inStream->close();
    out("<-Read completed");
    out("Loaded",Hits.size(),"events");

    return "";
}
