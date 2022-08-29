#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include <vector>

class SitalaKitGenerator
{
public:
    enum SampleReferences
    {
        Embedded,
        Referenced
    };

    SitalaKitGenerator(const File &destination, const std::vector<File> &samples,
                       SampleReferences references = Embedded);

    bool run();

private:
    AudioFormatManager m_formatManager;
    File m_destination;
    std::vector<File> m_samples;
    SampleReferences m_references = Embedded;
};
