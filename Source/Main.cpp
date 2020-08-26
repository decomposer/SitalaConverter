#include "../JuceLibraryCode/JuceHeader.h"

#include "ADGReader.h"
#include "SitalaKitGenerator.h"

int main(int argc, char* argv[])
{
    if(argc != 3)
    {
        DBG("Usage: SamplesFromMarsConverter [ADG-File] [Sitala-File]");
        return 1;
    }

    File adgFile(argv[1]);
    if(!adgFile.exists())
    {
        DBG("ADG file not found");
        return 2;
    }

    ADGReader reader(adgFile);
    const auto files = reader.getContainSamplePaths();

    if(files.size() > 16)
    {
        DBG("Too many samples (" << files.size() << "): " << argv[1]);
        return 3;
    }

    for(auto file : files)
    {
        if(!file.exists())
        {
            DBG("File " << file.getFullPathName() << " doesn't exist");
            return 4;
        }
    }

    SitalaKitGenerator generator(File(argv[2]), files);

    if(!generator.run())
    {
        DBG("Error generating kit");
        return 5;
    }

    DBG("Created: " << argv[2]);

    return 0;
}
