#include "Configuration.hh"
#include "out.hh"
#include "json11.hh"
#include "jstools.hh"

#include <iostream>
#include <fstream>

Configuration & Configuration::getInstance()
{
    static Configuration instance; // Guaranteed to be destroyed, instantiated on first use.
    return instance;
}

void Configuration::saveConfig(const std::string & fileName) const
{
    json11::Json::object json;

    json["WorkingDirectory"]  = WorkingDirectory;

    json["InputFileName"]     = InputFileName;
    json["BinaryInput"]       = BinaryInput;

    json["OutputFileName"]    = OutputFileName;
    json["BinaryOutput"]      = BinaryOutput;

    json["HeaderFileName"]    = HeaderFileName;

    json["LutFileName"]       = LutFileName;
    json["ExportLutFileName"] = ExportLutFileName;

    json["RejectSameHead"]    = RejectSameHead;

    json["CoincidenceWindow"] = CoincidenceWindow;

    json["TimeFrom"]          = TimeFrom;
    json["TimeTo"]            = TimeTo;

    json["EnergyFrom"]        = EnergyFrom;
    json["EnergyTo"]          = EnergyTo;

    std::string json_str = json11::Json(json).dump();
    std::ofstream confStream;
    confStream.open(WorkingDirectory + '/' + fileName);
    if (confStream.is_open())
        confStream << json_str << std::endl;
    confStream.close();
}

#include <sstream>
void Configuration::loadConfig(const std::string & fileName)
{
    if (!isFileExist(fileName))
    {
        out("Config file", fileName, "does not exist or cannot be open!");
        exit(1);
    }

    out("\nReading config file:", fileName);
    std::ifstream in(fileName);
    std::stringstream sstr;
    sstr << in.rdbuf();
    in.close();
    std::string cs = sstr.str();

    std::string err;
    json11::Json json = json11::Json::parse(cs, err);
    if (!err.empty())
    {
        out(err);
        exit(2);
    }

    jstools::readString(json, "WorkingDirectory",  WorkingDirectory);
    if (!isDirExist(WorkingDirectory))
    {
        out("Directory does not exist:", WorkingDirectory);
        exit(3);
    }

    jstools::readString(json, "InputFileName",     InputFileName);
    jstools::readBool  (json, "BinaryInput",       BinaryInput);

    jstools::readString(json, "OutputFileName",    OutputFileName);
    jstools::readBool  (json, "BinaryOutput",      BinaryOutput);

    jstools::readString(json, "HeaderFileName",    HeaderFileName);

    jstools::readString(json, "LutFileName",       LutFileName);
    jstools::readString(json, "ExportLutFileName", ExportLutFileName);

    jstools::readBool  (json, "RejectSameHead",    RejectSameHead);

    jstools::readDouble(json, "CoincidenceWindow", CoincidenceWindow);

    jstools::readDouble(json, "TimeFrom",          TimeFrom);
    jstools::readDouble(json, "TimeTo",            TimeTo);

    jstools::readDouble(json, "EnergyFrom",        EnergyFrom);
    jstools::readDouble(json, "EnergyTo",          EnergyTo);
}

void Configuration::writeHeaderLine(std::ofstream & hStream, std::vector<std::string> fields)
{
    for (size_t i = 0; i < fields.size(); i++)
    {
        if (i != 0) hStream << ',';
        hStream << fields[i];
    }
    hStream << '\n';
}

void Configuration::saveHeaderFile(size_t NumPairs)
{
    std::ofstream hStream;
    hStream.open(WorkingDirectory + '/' + HeaderFileName);

    if (!hStream.is_open())
    {
        out("Failed to open header file!");
        return;
    }

    writeHeaderLine(hStream, {"\"Format version\"", "1.1"});
    writeHeaderLine(hStream, {"\"Format type\"", "1"});
    writeHeaderLine(hStream, {"\"Data file\"", "\""+OutputFileName+"\""});
    writeHeaderLine(hStream, {"\"Data type\"", "1"});
    writeHeaderLine(hStream, {"\"Number of events\"", std::to_string(NumPairs)});
    writeHeaderLine(hStream, {"\"Data mode\"", (BinaryOutput ? "2" : "1")});
    writeHeaderLine(hStream, {"\"Crystal LUT file\"", "\""+ExportLutFileName+"\""});
}

#include <sys/types.h>
#include <sys/stat.h>
int Configuration::isDirExist(const std::string & dirName)
{
    struct stat info;

    if (stat(dirName.data(), &info) != 0) return false;
    else if (info.st_mode & S_IFDIR)      return true;
    else                                  return false;
}

int Configuration::isFileExist(const std::string &fileName)
{
    std::ifstream infile(fileName);
    return infile.good();
}

