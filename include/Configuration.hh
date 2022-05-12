#ifndef configuration_h
#define configuration_h

#include <string>
#include <vector>

class Configuration
{
public:
    static Configuration & getInstance();

private:
    Configuration(){}

public:
    Configuration(Configuration const&) = delete;
    void operator=(Configuration const&) = delete;

    void saveConfig(const std::string & fileName) const;
    void loadConfig(const std::string & fileName);

    void saveHeaderFile(size_t NumPairs);

    std::string WorkingDirectory;

    std::string InputFileName;
    bool BinaryInput;

    std::string OutputFileName;
    bool BinaryOutput;

    std::string HeaderFileName = "Header.hlm";

    std::string LutFileName        = "LUT.txt";
    std::string ExportLutFileName  = "CrystalLUT.txt";

    int    FinderMethod      = 2;

    bool   GroupByAssembly   = true;

    bool   RejectSameHead    = true;

    enum EMultiRejection {None, All, EnergyWindow}; // None        - multiples are allowed, the coincidence pair takes two strongest depositions
                                                    // All         - all coincidences with the multiplicities larger than two are discarded
                                                    // EnergyWndow - multiple coincidence is discarded only if there are more than two energy-accepted depositions
    EMultiRejection RejectMultiples = None;

    double CoincidenceWindow = 4.0; // [ns]

    // consider events only in this time range [ns]
    double TimeFrom          = 0;
    double TimeTo            = 1e20;

    // consider events only in this energy range [MeV]
    double EnergyFrom     = 0.511 * 0.95;
    double EnergyTo       = 0.511 * 1.05;

private:
    int  isDirExist(const std::string  & dirName);
    int  isFileExist(const std::string & fileName);
    void writeHeaderLine(std::ofstream & hStream, std::vector<std::string> fields);
};

#endif // configuration_h
