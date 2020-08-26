#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

class ADGReader
{
public:
    ADGReader(const File &source);

    std::vector<File> getContainSamplePaths();

private:
    void processElements(const XmlElement *parent, const String &tag,
                         const std::function<void(const XmlElement *)> &processor);

    void processSampleRef(const XmlElement *sampleRef);

    File m_source;
    std::vector<File> m_samples;
    std::vector<int> m_notes;
};
