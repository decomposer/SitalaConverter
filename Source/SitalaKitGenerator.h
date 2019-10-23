#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include <vector>

class SitalaKitGenerator
{
public:
    SitalaKitGenerator(const File &destination, std::vector<File> samples);

    bool run();

private:
    AudioFormatManager m_formatManager;
    File m_destination;
    std::vector<File> m_samples;
};
