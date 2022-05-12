#include "DataStructures.hh"
#include "Lut.hh"
#include "Reader.hh"
#include "Finder1.hh"
#include "Finder2.hh"
#include "Writer.hh"
#include "Configuration.hh"
#include "out.hh"

#include <string>
#include <vector>
#include <algorithm>

// Input file format:
//
// # iScint                     iScint is the scintillator's index, typically starts with 0
// t1 e1                        t is the event time [ns], e is the event energy [MeV]
// t2 e2
// ...
// tn en
// # iScint+1                   next scintillator index
// t1 e1
// t2 e2
// ...

//Output file format
//
// i1 i2 dt t1                    i1 and i2 are the scintullator's indexes, dt is delta time (t1 - t2) [ps], t1 is time in [ms]

// incoming LUT format:
// FacePosX FacePosY FacePosZ NormX NormY NormZ HeadNumber Angle AssemblyNumber
// outgoing LUT format:
// iScint,FacePosX,FacePosY,FacePosZ

int main(int argc, char** argv)
{
    Configuration & Config = Configuration::getInstance();

    std::string filename;
    if (argc > 1)
    {
        filename = std::string(argv[1]);
        out("\nLoading config from file:", filename);
    }
    else out("\nNo config file provided as argument, using configuration defined in the main of the coinc");

        // warning: automatically saves config (if no errors) in working directory as CoincConfig.json
        // beware of possible overright!

        //here you can directly provide the config file name
    //filename = "/home/andr/WORK/TPPT/CoincConfig1.json";

    if (!filename.empty())
    {
        Config.loadConfig(filename);
    }
    else
    {
        // --- Start of user inits ---

        Config.WorkingDirectory  = "/home/andr/WORK/TPPT/TestCoincData";

        //Config.InputFileName     = "BuilderOutput1e6.bin"; Config.BinaryInput  = true;
        Config.InputFileName     = "in-multiGoodSplit.txt";  Config.BinaryInput = false;
        //Config.OutputFileName    = "CoincPairs.bin";     Config.BinaryOutput = true;
        Config.OutputFileName    = "CoincPairs.txt";     Config.BinaryOutput = false;

        Config.HeaderFileName    = "Header.hlm";

        Config.LutFileName       = "LUT.txt";
        Config.ExportLutFileName = "CrystalLUT.txt";

        Config.FinderMethod      = 2;  // 1 - first implemented, no energy splitting allowed, strict multiple rejection
                                       // 2 - second one, energy is allowed to be split within the same assembly

        Config.GroupByAssembly   = true; // only for FinderMethod=2

        //Rejection config
        Config.RejectSameHead    = true;
        Config.RejectMultiples   = Configuration::EnergyWindow; // only for FinderMethod=2  possible choices:  None, All, EnergyWindow

        Config.CoincidenceWindow = 4.0;      // [ns]

        Config.TimeFrom          = 0;
        Config.TimeTo            = 1e50;

        Config.EnergyFrom        = 0.511 * 0.95; // [MeV]
        Config.EnergyTo          = 0.511 * 1.05;

        // --- End of user inits
    }

    Lut LUT(Config.WorkingDirectory + '/' + Config.LutFileName);

    std::vector<EventRecord>     Events;
    std::vector<CoincidencePair> Pairs;

    const bool EnforceEnergyTimeInReader = (Config.FinderMethod == 1);
    Reader reader(EnforceEnergyTimeInReader);
    std::string error = reader.read(Events);
    if (!error.empty())
    {
        out(error);
        exit(2);
    }

    out("-->Sorting events");
    std::sort(Events.begin(), Events.end());

    out("-->Finding coincidences");
    switch (Config.FinderMethod)
    {
    case 1:
        {
            Finder1 cf(Events, LUT);
            cf.findCoincidences(Pairs);
        }
        break;
    case 2:
        {
            Finder2 cf(Events, LUT);
            cf.findCoincidences(Pairs);
        }
        break;
    default:
        out("Unknown finder method");
        exit(20);
    }

    Writer writer;
    error = writer.write(Pairs);
    if (!error.empty())
    {
        out(error);
        exit(3);
    }

    Config.saveConfig("CoincConfig.json");

    Config.saveHeaderFile(Pairs.size());
    LUT.exportLUT(Config.WorkingDirectory + '/' + Config.ExportLutFileName);
}
