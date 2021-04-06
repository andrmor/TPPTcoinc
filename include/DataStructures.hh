#ifndef depositionnoderecord_h
#define depositionnoderecord_h

struct ScintPosition
{
    ScintPosition(double x, double y, double z) {pos[0] = x; pos[1] = y; pos[2] = z;}
    ScintPosition(){}

    double pos[3];
};

struct HitRecord
{
    HitRecord(int scint, double time) : iScint(scint), Time(time) {}
    HitRecord(){}

    bool operator<(const HitRecord & other) const {return Time < other.Time;}

    int    iScint;
    double Time;
};

struct CoincidencePair
{
    CoincidencePair(HitRecord Record1, HitRecord Record2) {Records[0] = Record1; Records[1] = Record2;}
    CoincidencePair(){}

    HitRecord Records[2];
};

#endif // depositionnoderecord_h
