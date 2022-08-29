#include "AbletonDeviceGroupReader.h"

#define ADG_EXTENSION ".adg"

AbletonDeviceGroupReader::AbletonDeviceGroupReader(const File &source) :
    m_source(source)
{
}

bool AbletonDeviceGroupReader::isAbletonKit(const File &file)
{
    return file.getFileExtension() == ADG_EXTENSION;
}

std::vector<File> AbletonDeviceGroupReader::getSamples()
{
    jassert(m_source.exists());

    FileInputStream inFile(m_source);

    GZIPDecompressorInputStream streamIn(&inFile, false,
        GZIPDecompressorInputStream::Format::gzipFormat);

    jassert(!streamIn.isExhausted());
    const auto str = streamIn.readEntireStreamAsString();

    XmlDocument doc(str);
    std::unique_ptr<XmlElement> root(doc.getDocumentElement());

    std::vector<File> samples;

    processElements(root.get(), "SampleRef", [this, &samples](const XmlElement *e) {
        processSampleRef(e, samples);
    });

    std::map<int, File> sorted;

    processElements(root.get(), "ReceivingNote", [&samples, &sorted](const XmlElement *e) {
        sorted[128 - e->getIntAttribute("Value")] = samples[sorted.size()];
    });

    samples.clear();

    for(auto [note, sample] : sorted)
    {
        samples.push_back(sample);
    }

    return samples;
}

void AbletonDeviceGroupReader::processElements(
    const XmlElement *parent, const String &tag,
    const std::function<void(const XmlElement *)> &processor)
{
    if(parent->getTagName() == tag)
    {
        processor(parent);
        return;
    }

    for(auto i = 0; i < parent->getNumChildElements(); i++)
    {
        auto child = parent->getChildElement(i);
        processElements(child, tag, processor);
    }
}

void AbletonDeviceGroupReader::processSampleRef(const XmlElement *sampleRef,
                                                std::vector<File> &samples)
{
    auto fileRef = sampleRef->getChildByName("FileRef");
    if(fileRef)
    {
        String path;

        auto relativePath = fileRef->getChildByName("RelativePath");
        if(relativePath)
        {
            for(auto i = 0; i < relativePath->getNumChildElements(); i++)
            {
                auto pathCrumb = relativePath->getChildElement(i);
                if(pathCrumb->getTagName() == "RelativePathElement"
                   && pathCrumb->hasAttribute("Dir"))
                {
                    auto dir = pathCrumb->getStringAttribute("Dir");
                    if(dir.isEmpty())
                    {
                        path += "../";
                    }
                    else
                    {
                        path += dir + "/";
                    }
                }
            }
        }

        auto nameElement = fileRef->getChildByName("Name");
        if(nameElement)
        {
            auto name = nameElement->getStringAttribute("Value");
            path += name;
        }

        auto file = m_source.getChildFile(String("../") + path);
        samples.push_back(file);
    }
}
