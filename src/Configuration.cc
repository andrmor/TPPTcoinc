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

    json["InputFileNames"]    = InputFileName;
    json["BinaryInput"]       = BinaryInput;

    json["OutputFileName"]    = OutputFileName;
    json["BinaryOutput"]      = BinaryOutput;

    json["LutFileName"]       = LutFileName;

    json["RejectSameHead"]    = RejectSameHead;

    json["CoincidenceWindow"] = CoincidenceWindow;

    json["TimeFrom"]          = TimeFrom;
    json["TimeTo"]            = TimeTo;

    json["EnergyFrom"]        = EnergyFrom;
    json["EnergyTo"]          = EnergyTo;

    std::string json_str = json11::Json(json).dump();
    std::ofstream confStream;
    confStream.open(fileName);
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

    jstools::readString(json, "LutFileName",       LutFileName);

    jstools::readBool  (json, "RejectSameHead",    RejectSameHead);

    jstools::readDouble(json, "CoincidenceWindow", CoincidenceWindow);

    jstools::readDouble(json, "TimeFrom",          TimeFrom);
    jstools::readDouble(json, "TimeTo",            TimeTo);

    jstools::readDouble(json, "EnergyFrom",        EnergyFrom);
    jstools::readDouble(json, "EnergyTo",          EnergyTo);
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

