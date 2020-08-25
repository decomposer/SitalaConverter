#include "../JuceLibraryCode/JuceHeader.h"

#include "ADGReader.h"
#include "SitalaKitGenerator.h"

int main(int argc, char* argv[])
{
    if(argc != 3)
    {
        DBG("Usage: SamplesFromMarsConverter [ADG-File] [Sitala-File]");
        return -1;
    }

    File adgFile(argv[1]);
    if(!adgFile.exists())
    {
        DBG("ADG file not found");
        return -1;
    }

    ADGReader reader(adgFile);
    const auto files = reader.getContainSamplePaths();
    for(auto file : files)
    {
        if(!file.exists())
        {
            DBG("File " << file.getFullPathName() << " doesn't exist");
            return -1;
        }
    }

    SitalaKitGenerator generator(File(argv[2]), files);

    if(!generator.run())
    {
        DBG("Error generating kit");
        return -1;
    }

    return 0;
}
