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

    SitalaKitGenerator(const File &destination);
    void setSamples(const std::vector<File> &samples, SampleReferences references = Embedded);
    void setVendor(const String &vendor);

    bool run();
    String getError() const;

private:
    AudioFormatManager m_formatManager;
    File m_destination;
    std::vector<File> m_samples;
    SampleReferences m_references = Embedded;
    String m_vendor;
    String m_error;
};
