#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

class ADGReader
{
public:
    ADGReader(const File &source);

    std::vector<String> getContainSamplePaths();

private:
    XmlElement *findSampleRef(XmlElement *parent);
    void processSampleRef(XmlElement *sampleRef);

    File m_source;
    std::vector<String> m_samples;
};
