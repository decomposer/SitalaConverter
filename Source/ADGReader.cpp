#include "ADGReader.h"

ADGReader::ADGReader(const File &source) :
    m_source(source)
{
}

std::vector<File> ADGReader::getContainSamplePaths()
{
    jassert(m_source.exists());

    FileInputStream inFile(m_source);

    GZIPDecompressorInputStream streamIn(&inFile, false,
        GZIPDecompressorInputStream::Format::gzipFormat);

    jassert(!streamIn.isExhausted());
    const auto str = streamIn.readEntireStreamAsString();

    XmlDocument doc(str);
    std::unique_ptr<XmlElement> root(doc.getDocumentElement());

    findSampleRef(root.get());

    jassert(root);

    return m_samples;
}

XmlElement *ADGReader::findSampleRef(XmlElement *parent)
{
    if(parent->getTagName() == "SampleRef")
    {
        processSampleRef(parent);
        return parent;
    }

    for(auto i = 0; i < parent->getNumChildElements(); i++)
    {
        auto child = parent->getChildElement(i);
        findSampleRef(child);
    }

    return nullptr;
}

void ADGReader::processSampleRef(XmlElement *sampleRef)
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
        m_samples.push_back(file);
    }
}
