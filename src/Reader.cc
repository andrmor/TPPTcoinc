#include "Reader.hh"
#include "out.hh"

#include <fstream>
#include <sstream>

Reader::Reader(const std::string & FileName, bool BinaryInput, double MinEnergy, double MaxEnergy) :
    bBinary(BinaryInput), MinEnergy(MinEnergy), MaxEnergy(MaxEnergy)
{
    if (bDebug)
    {
        out("Input file:", FileName);
        out("->Opening stream...");
    }

    if (bBinary) inStream = new std::ifstream(FileName, std::ios::in | std::ios::binary);
    else         inStream = new std::ifstream(FileName);

    if (!inStream->is_open() || inStream->fail() || inStream->bad())
    {
        if (bDebug) out("<-Failed!");
        delete inStream; inStream = nullptr;
    }
    else
    {
        if (bDebug) out("<-Open");
    }
}

std::string Reader::read(std::vector<HitRecord> & Hits)
{
    if (!inStream) return "Cannot open input file";

    if (bDebug) out("->Reading input file...");

    int iScint;
    if (bBinary)
    {
        char ch;
        double depo;
        while (inStream->get(ch))
        {
            if (inStream->eof()) break;

            if (ch == (char)0xEE)
            {
                inStream->read((char*)&iScint, sizeof(int));
            }
            else if (ch == (char)0xFF)
            {
                HitRecord hit(iScint, 0);
                inStream->read((char*)&hit.Time, sizeof(double));
                inStream->read((char*)&depo,     sizeof(double));
                if (bDebug) out("Extracted values:", hit.Time, depo);
                if (depo < MinEnergy || depo > MaxEnergy) continue;
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
                if (bDebug) out("Extracted values:", time, depo);

                if (depo < MinEnergy || depo > MaxEnergy) continue;
                Hits.push_back( HitRecord(iScint, time) );
                if (bDebug) out("  Added to HitRecords");
            }
        }
    }

    inStream->close();
    if (bDebug)
    {
        out("\n<-Read completed\n");
        out("Loaded",Hits.size(),"events");
    }
    return "";
}
