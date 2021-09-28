#include "Lut.hh"
#include "out.hh"

#include <fstream>
#include <sstream>

Lut::Lut(const std::string & FileName)
{
    std::ifstream inStream(FileName);

    if (!inStream.is_open() || inStream.fail() || inStream.bad())
    {
        out("Cannot open file:\n", FileName);
        exit(1);
    }
    else
    {
        std::string line;
        while (!inStream.eof())
        {
            getline(inStream, line);
            //out(line);
            if (line.empty()) break;

            std::stringstream ss(line);

            LutRecord r;
            ss >> r.Position[0] >> r.Position[1] >> r.Position[2] >>
                  r.Normal[0]   >> r.Normal[1]   >> r.Normal[2]   >>
                  r.HeadNumber >>
                  r.AssemblyAngle;

            //r.print();

            LUT.push_back(r);
        }
    }
}

int Lut::countScintillators() const
{
    return LUT.size();
}

int Lut::getHeadNumber(int iScint) const
{
    if (iScint >= 0 && iScint < LUT.size())
        return LUT[iScint].HeadNumber;
    else
    {
        out("Bad scintillator index", iScint, "-> LUT has", LUT.size(), "records");
        exit(10);
    }
}

bool Lut::isDifferentHeads(int iScint1, int iScint2) const
{
    if (iScint1 >= 0 && iScint1 < LUT.size() &&
        iScint2 >= 0 && iScint2 < LUT.size())
            return ( LUT[iScint1].HeadNumber != LUT[iScint2].HeadNumber );
    out("One of the scintillator indices is bad!");
    exit(10);
}

void LutRecord::print()
{
    out("(",Position[0],Position[1],Position[2],")", "(",Normal[0],Normal[1],Normal[2],")", HeadNumber, AssemblyAngle);
}

void Lut::exportLUT(const std::string & FileName)
{
    std::ofstream hStream;
    hStream.open(FileName);

    if (!hStream.is_open())
    {
        out("Failed to open file to export LUT file!");
        return;
    }

    for (size_t i = 0; i < LUT.size(); i++)
        hStream << i << ',' << LUT[i].Position[0] << ',' << LUT[i].Position[1] << ',' << LUT[i].Position[2] << '\n';

    hStream.close();
}
