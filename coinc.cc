#include "DataStructures.hh"
#include "Enums.hh"
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

    if (!filename.empty())
    {
        Config.loadConfig(filename);
    }
    else
    {
        // --- Start of user inits ---

        Config.WorkingDirectory  = "/home/andr/WORK/tmp";

        // input
        //Config.InputFileName     = "BuilderOutput1e6.bin"; Config.BinaryInput  = true;
        Config.InputFileName     = "BuilderOutput2e8.txt"; Config.BinaryInput = false;
        Config.LutFileName       = "LUT.txt";

        // output
        //Config.OutputFileName    = "CoincPairs.bin"; Config.BinaryOutput = true;
        Config.OutputFileName    = "CoincPairsLineY2e8.txt"; Config.BinaryOutput = false;
        Config.HeaderFileName    = "Header.hlm";
        Config.ExportLutFileName = "CrystalLUT.txt";

        // finder
        Config.FinderMethod      = FinderMethods::Basic;
            // Basic    - no energy splitting allowed, strict multiple rejection
            // Advanced - energy is allowed to be split within the same assembly, flexible multiple rejection

        // next two settings have effect only if "Advanced" FinderMethod is selected
            Config.GroupByAssembly = true;
            Config.RejectMultiples = RejectionMethods::EnergyWindow;
                // None        - multiples are allowed, the coincidence pair takes two strongest depositions
                // All         - all coincidences with the multiplicities larger than two are discarded
                // EnergyWndow - multiple coincidence is discarded only if there are more than two energy-accepted depositions

        Config.RejectSameHead    = true;
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

    const bool EnforceEnergyInReader = (Config.FinderMethod == FinderMethods::Basic);
    Reader reader(EnforceEnergyInReader);
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
    case FinderMethods::Basic :
        {
            Finder1 cf(Events, LUT);
            cf.findCoincidences(Pairs);
        }
        break;
    case FinderMethods::Advanced :
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
