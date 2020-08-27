#include "../JuceLibraryCode/JuceHeader.h"

#include "ADGReader.h"
#include "SitalaKitGenerator.h"

#include <algorithm>

bool createKit(const String &output, std::vector<File> files, int offset = 0)
{
    // DBG("createKit " << output << ", offset: " << offset);
    auto begin = files.begin() + offset;
    auto end = files.size() - offset <= 16 ? files.end() : files.begin() + offset + 16;
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

std::vector<std::vector<File>> filesFromTextFile(const File &textFile)
{
    std::vector<std::vector<File>> kits;
    kits.push_back(std::vector<File>());
    FileInputStream stream(textFile);

    for(auto line = stream.readNextLine(); !stream.isExhausted(); line = stream.readNextLine())
    {
        if(line.containsNonWhitespaceChars())
        {
            kits.back().push_back(File(line.trim()));
        }
        else
        {
            kits.push_back(std::vector<File>());
        }
    }

    return kits;
}

String sequencedKitName(const String &output, int i)
{
    String extension = " - ";
    extension += i + 1;
    extension += ".sitala";
    output.replace(".sitala", extension);
}

int main(int argc, const char *argv[])
{
    if(argc == 2)
    {
        auto files = ADGReader(File(argv[1])).getContainSamplePaths();
        DBG(argv[1] << ": " << files.size() << " samples");
        for(auto file : files)
        {
            DBG(file.getRelativePathFrom(File::getCurrentWorkingDirectory()));
        }
        return 0;
    }

    if(argc != 3)
    {
        DBG("Usage: SamplesFromMarsConverter [ADG-File] [Sitala-File]");
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

    ADGReader reader(adgFile);
    const auto files = reader.getContainSamplePaths();

    File textFile(adgFile.getFullPathName().replace(".adg", ".txt"));

    if(textFile.exists())
    {
        auto kits = filesFromTextFile(textFile);
        DBG("Read " << kits.size() << " from " << textFile.getFullPathName());
        for(auto i = 0; i < kits.size(); i++)
        {
            auto &kit = kits[i];

            if(kits.size() > 16)
            {
                DBG("Text group too large: " << kit.size());
                continue;
            }

            auto name = kits.size() > 1 ? sequencedKitName(output, i) : output;
            createKit(name, files);
        }
    }
    else if(files.size() > 16)
    {
        if(files.size() % 16 == 0)
        {
            DBG("Splitting ADG into multiple kits");

            for(auto i = 0; i * 16 < files.size(); i++)
            {
                createKit(sequencedKitName(output, i), files, i * 16);
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
