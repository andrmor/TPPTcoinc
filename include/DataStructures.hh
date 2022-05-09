#ifndef depositionnoderecord_h
#define depositionnoderecord_h

struct ScintPosition
{
    ScintPosition(double x, double y, double z) {pos[0] = x; pos[1] = y; pos[2] = z;}
    ScintPosition(){}

    double pos[3];
};

struct EventRecord
{
    EventRecord(int scint, double time, double energy) : iScint(scint), Time(time), Energy(energy) {}
    EventRecord(){}

    bool operator<(const EventRecord & other) const {return Time < other.Time;}

    int    iScint;
    double Time;
    double Energy;
};

struct CoincidencePair
{
    CoincidencePair(EventRecord Record1, EventRecord Record2) {Records[0] = Record1; Records[1] = Record2;}
    CoincidencePair(){}

    EventRecord Records[2];
};

#endif // depositionnoderecord_h
