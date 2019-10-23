#include "../JuceLibraryCode/JuceHeader.h"

#include "ADGReader.h"

int main (int argc, char* argv[])
{
    ADGReader reader(File("/Users/luci/Projects/music/Samples From Mars/Modern Oddities From Mars/Ableton Live/Modern Oddities From Mars/Presets/Kits/Hardware Glitch Kit.adg"));
    reader.getContainSamplePaths();

    return 0;
}
