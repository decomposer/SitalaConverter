#include "Converter.h"
#include "AbletonDeviceGroupReader.h"
#include "SitalaKitGenerator.h"

static String subKitName(const String &original, const String &sub)
{
    auto dir = File(original).getParentDirectory().getFullPathName();
    return File::addTrailingSeparator(dir) + sub + ".sitala";
}

static bool createKit(const String &output, std::vector<File> files, long offset = 0)
{
    auto begin = files.begin() + offset;
    auto end = files.size() - static_cast<unsigned long>(offset) <= 16 ?
               files.end() : files.begin() + offset + 16;
    std::vector<File> slice(begin, end);

    files = slice;

    for(auto file : files)
    {
        if(!file.exists())
        {
            DBG("File " << file.getFullPathName() << " doesn't exist");
            return false;
        }
    }

    SitalaKitGenerator generator(output, files);

    if(!generator.run())
    {
        DBG("Error generating kit: " << output);
        return false;
    }

    DBG("Created: " << output);

    return true;
}

void Converter::convert(const String &inputFile, const String &outputFile)
{
    AbletonDeviceGroupReader reader(inputFile);
    const auto files = reader.getContainSamplePaths();

    StringArray names;


    for(auto &name : names)
    {
        if(!name.startsWithChar('#'))
        {
            names.clear();
            break;
        }
        else
        {
            name = name.replaceFirstOccurrenceOf("#", "").trim();
        }
    }


    if(files.size() > 16)
    {
        if(files.size() % 16 == 0)
        {
            jassert(static_cast<unsigned long>(names.size()) == files.size());

            DBG("Splitting ADG into multiple kits: " << inputFile);

            for(auto i = 0; static_cast<unsigned long>(i * 16) < files.size(); i++)
            {
                createKit(subKitName(outputFile, names[i]), files, i * 16);
            }
        }
        else
        {
            DBG("Too many samples (" << files.size() << "): " << inputFile);
        }
    }
    else
    {
        createKit(outputFile, files);
    }
}
