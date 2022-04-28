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
// # iScint                     iScint is the scintillator's index
// t1 e1                        t is the time of the hit [ns], e is the event energy [MeV]
// t2 e2
// ...
// tn en
// # ...                        next scintillator index

//Output file format
//
// i1 i2 dt t1                    i1 and i2 are the scintullator's indexes, dt is delta time (t1 - t2) [ps], t1 is time in [ms]

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

        Config.WorkingDirectory  = "/home/andr/WORK/TPPT/Na22/AfterEnergyBlurAdded";

        Config.InputFileName     = "BuilderOutputTest.bin"; Config.BinaryInput  = true;
        //Config.InputFileName     = "BuilderOutput.txt";  Config.BinaryInput = false;
        //Config.OutputFileName    = "CoincPairs.bin";     Config.BinaryOutput = true;
        Config.OutputFileName    = "CoincPairsTest.txt";     Config.BinaryOutput = false;

        Config.HeaderFileName    = "Header.hlm";

        Config.LutFileName       = "LUT.txt";
        Config.ExportLutFileName = "CrystalLUT.txt";

        Config.FinderMethod      = 2;  // 1 - first implemented, no energy splitting allowed
                                       // 2 - second one, energy is allowed to be split within the same assembly
        Config.RejectSameHead    = true;

        Config.CoincidenceWindow = 4.0;      // [ns]

        Config.TimeFrom          = 0;
        Config.TimeTo            = 1e50;

        Config.EnergyFrom        = 0.511 * 0.95; // [MeV]
        Config.EnergyTo          = 0.511 * 1.05;

        // --- End of user inits
    }

    Lut LUT(Config.WorkingDirectory + '/' + Config.LutFileName);

    std::vector<HitRecord> Hits;
    std::vector<CoincidencePair> Pairs;

    const bool EnforceEnergyTimeInReader = (Config.FinderMethod == 1);
    Reader reader(EnforceEnergyTimeInReader);
    std::string error = reader.read(Hits);
    if (!error.empty())
    {
        out(error);
        exit(2);
    }

    out("-->Sorting hits");
    std::sort(Hits.begin(), Hits.end());

    out("-->Finding coincidences");
    switch (Config.FinderMethod)
    {
    case 1:
        {
            Finder1 cf(Hits, LUT);
            cf.findCoincidences(Pairs);
        }
        break;
    case 2:
        {
            Finder2 cf(Hits, LUT);
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
