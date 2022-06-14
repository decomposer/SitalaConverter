#include "../JuceLibraryCode/JuceHeader.h"

#include "AbletonDeviceGroupReader.h"
#include "SitalaKitGenerator.h"
#include "Config.h"

#include <algorithm>

#include <QApplication>
#include <QMainWindow>

static bool createKit(const String &output, std::vector<File> files, long offset = 0)
{
    // DBG("createKit " << output << ", offset: " << offset);
    auto begin = files.begin() + offset;
    auto end = files.size() - static_cast<unsigned long>(offset) <= 16 ? files.end() : files.begin() + offset + 16;
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

static std::map<String, std::vector<File>> filesFromTextFile(const File &textFile)
{
    std::map<String, std::vector<File>> kits;
    String name;

    StringArray lines;
    textFile.readLines(lines);

    for(auto &line : lines)
    {
        if(line.trim().isEmpty())
        {
            continue;
        }

        if(line.startsWithChar('#'))
        {
            name = line.replaceFirstOccurrenceOf("#", "").trim();
            continue;
        }

        auto dir = textFile.getParentDirectory().getFullPathName();
        auto sample = File::addTrailingSeparator(dir) + line.trim();
        kits[name].push_back(File(sample));
    }

    return kits;
}

static String subKitName(const String &original, const String &sub)
{
    auto dir = File(original).getParentDirectory().getFullPathName();
    return File::addTrailingSeparator(dir) + sub + ".sitala";
}

int main(int argc, char *argv[])
{
    if(argc == 1)
    {
        QApplication app(argc, argv);
        QMainWindow win;
        win.show();
        return app.exec();
    }

    if(argc == 2)
    {
        auto files = AbletonDeviceGroupReader(File(argv[1])).getContainSamplePaths();
        DBG(argv[1] << ": " << files.size() << " samples");
        for(auto file : files)
        {
            DBG(file.getRelativePathFrom(File::getCurrentWorkingDirectory()));
        }
        return 0;
    }

    if(argc != 3)
    {
        DBG("Usage: " PROJECT_NAME " [ADG-File] [Sitala-File]");
        return 1;
    }

    String input(argv[1]);
    String output(argv[2]);

    File adgFile(input);

    if(!adgFile.exists())
    {
        DBG("ADG file not found");
        return 2;
    }

    AbletonDeviceGroupReader reader(adgFile);
    const auto files = reader.getContainSamplePaths();

    File textFile(adgFile.getFullPathName().replace(".adg", ".txt"));
    StringArray names;
    textFile.readLines(names);


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

    if(names.isEmpty() && textFile.exists())
    {
        auto kits = filesFromTextFile(textFile);
        DBG("Read " << kits.size() << " from " << textFile.getFullPathName());
        for(auto [name, files] : kits)
        {
            if(files.size() > 16)
            {
                DBG("Text group too large: " << files.size());
                continue;
            }

            createKit(name.isEmpty() ? output : subKitName(output, name), files);
        }
    }
    else if(files.size() > 16)
    {
        if(files.size() % 16 == 0)
        {
            jassert(static_cast<unsigned long>(names.size()) == files.size());

            DBG("Splitting ADG into multiple kits: " << input);

            for(auto i = 0; static_cast<unsigned long>(i * 16) < files.size(); i++)
            {
                createKit(subKitName(output, names[i]), files, i * 16);
            }
        }
        else
        {
            DBG("Too many samples (" << files.size() << "): " << argv[1]);
            return 3;
        }
    }
    else
    {
        createKit(output, files);
    }

    return 0;
}
