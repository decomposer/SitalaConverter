#include "../JuceLibraryCode/JuceHeader.h"

#include "ADGReader.h"
#include "SitalaKitGenerator.h"

int main (int argc, char* argv[])
{
    ADGReader reader(File("/Users/luci/Projects/music/Samples From Mars/Modern Oddities From Mars/Ableton Live/Modern Oddities From Mars/Presets/Kits/Hardware Glitch Kit.adg"));
    const auto files = reader.getContainSamplePaths();
    for(auto file : files)
    {
        if(!file.exists())
        {
            DBG("File " << file.getFullPathName() << " doesn't exist");
            return -1;
        }

//        DBG(file.getFullPathName());
    }

    SitalaKitGenerator generator(File("/Users/luci/Projects/music/Samples From Mars/Modern Oddities From Mars/Sitala/Hardware Glitch Kit.sitala"),
                       files);

    if(!generator.run())
    {
        DBG("Error generating kit");
        return -1;
    }

    return 0;
}
