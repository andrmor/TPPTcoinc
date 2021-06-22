#ifndef lut_h
#define lut_h

#include <string>
#include <vector>

struct LutRecord
{
    double Position[3];
    double Normal[3];

    int    HeadNumber;
    double AssemblyAngle;

    void   print();
};

class Lut
{
public:
    Lut(const std::string & FileName);

    int  countScintillators() const;

    int  getHeadNumber(int iScint) const;
    bool isDifferentHeads(int iScint1, int iScint2) const;

private:
    std::vector<LutRecord> LUT;

};

#endif // lut_h
