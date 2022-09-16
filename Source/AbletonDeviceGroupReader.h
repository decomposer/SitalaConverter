#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

class AbletonDeviceGroupReader
{
public:
    AbletonDeviceGroupReader(const File &source);

    std::vector<File> getSamples();

    static bool isAbletonKit(const File &file);

    static bool isSampleEncrypted(const File &file);

private:
    void processElements(const XmlElement *parent, const String &tag,
                         const std::function<void(const XmlElement *)> &processor);
    bool readRelativePathFromElement(const XmlElement *sampleRef, std::vector<File> *samples);

    File m_source;
};
