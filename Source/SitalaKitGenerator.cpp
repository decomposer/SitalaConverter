#include "Config.h"
#include "SitalaKitGenerator.h"
#include "AbletonDeviceGroupReader.h"

static constexpr auto ZipLevel = 9;

SitalaKitGenerator::SitalaKitGenerator(const File &destination) :
    m_destination(destination)
{
    m_formatManager.registerBasicFormats();
}

void SitalaKitGenerator::setSamples(const std::vector<File> &samples, SampleReferences references)
{
    m_samples = samples;
    m_references = references;
}

void SitalaKitGenerator::setVendor(const String &vendor)
{
    m_vendor = vendor;
}

bool SitalaKitGenerator::run()
{
    ZipFile::Builder fileBuilder;

    ValueTree soundTree("sounds");

    for(auto i = 0; static_cast<unsigned long>(i) < m_samples.size(); i++)
    {
        const auto &file = m_samples[static_cast<unsigned long>(i)];
        auto relative = file.getRelativePathFrom(m_destination.getParentDirectory());

        ValueTree node("sound");
        node.setProperty("slot", i, nullptr);
        node.setProperty("name", file.getFileNameWithoutExtension(), nullptr);

        if(!file.exists())
        {
            m_error = "Sample " + file.getFullPathName() + " not found";
            return false;
        }

        if(m_references == Embedded)
        {
            auto name = String("Samples/") + file.getFileName();
            node.setProperty("internal", name, nullptr);
            fileBuilder.addFile(file, ZipLevel, name);
        }
        else
        {
            node.setProperty("external", relative, nullptr);
        }

        MD5 checksum(file);
        node.setProperty("md5", checksum.toHexString(), nullptr);

        std::unique_ptr<AudioFormatReader>
            reader(m_formatManager.createReaderFor(file));

        if(!reader)
        {
            if(AbletonDeviceGroupReader::isSampleEncrypted(file))
            {
                m_error = "Sample " + file.getFullPathName() + " is encrypted";
            }
            else
            {
                m_error = "Couldn't read audio from " + file.getFullPathName();
            }

            return false;
        }

        double len = reader->lengthInSamples / reader->sampleRate * 1000;

        ValueTree parameters("parameters");

        ValueTree volume("volume");
        volume.setProperty("db", 0.0f, nullptr);
        parameters.appendChild(volume, nullptr);

        ValueTree shape("shape");
        shape.setProperty("macro", 0.5f, nullptr);

        ValueTree attack("attack");
        attack.setProperty("ms", 0, nullptr);
        shape.appendChild(attack, nullptr);

        ValueTree hold("hold");
        hold.setProperty("ms", len, nullptr);
        shape.appendChild(hold, nullptr);

        ValueTree decay("decay");
        decay.setProperty("ms", 0, nullptr);
        shape.appendChild(decay, nullptr);

        ValueTree makeup("makeup");
        makeup.setProperty("db", 0, nullptr);
        shape.appendChild(makeup, nullptr);

        parameters.appendChild(shape, nullptr);

        ValueTree compression("compression");
        compression.setProperty("macro", 0, nullptr);
        ValueTree bypassed("bypassed");
        compression.appendChild(bypassed, nullptr);

        parameters.appendChild(compression, nullptr);

        ValueTree tuning("tuning");
        tuning.setProperty("ct", 0, nullptr);
        parameters.appendChild(tuning, nullptr);

        ValueTree pan("pan");
        pan.setProperty("position", "C", nullptr);
        parameters.appendChild(pan, nullptr);

        ValueTree tone("tone");
        tone.setProperty("macro", 0.5, nullptr);

        ValueTree lowPass("lowPass");
        lowPass.setProperty("hz", 22000, nullptr);
        lowPass.setProperty("q", 0.7, nullptr);
        tone.appendChild(lowPass, nullptr);

        ValueTree notch("notch");
        notch.setProperty("hz", 400, nullptr);
        notch.setProperty("q", 3.5, nullptr);
        notch.setProperty("gainDb", 0, nullptr);
        tone.appendChild(notch, nullptr);

        ValueTree highPass("highPass");
        highPass.setProperty("hz", 1, nullptr);
        highPass.setProperty("q", 0.7, nullptr);
        tone.appendChild(highPass, nullptr);

        parameters.appendChild(tone, nullptr);

        node.appendChild(parameters, nullptr);
        soundTree.appendChild(node, nullptr);
    }

    ValueTree root("sitala");
    root.setProperty("version", 4, nullptr);
    root.setProperty("creator", PROJECT_NAME " " PROJECT_VERSION, nullptr);

    ValueTree kit("kit");
    kit.setProperty("label", m_destination.getFileNameWithoutExtension(), nullptr);
    kit.appendChild(soundTree, nullptr);

    if(!m_vendor.isEmpty())
    {
        ValueTree meta("meta");
        meta.setProperty("creator", m_vendor, nullptr);
        meta.setProperty("description", "", nullptr);
        kit.appendChild(meta, nullptr);
    }

    root.appendChild(kit, nullptr);

    MemoryOutputStream out;
    std::unique_ptr<XmlElement> rootElement(root.createXml());
    rootElement->writeTo(out);
    out.flush();

    auto in = new MemoryInputStream(out.getData(), out.getDataSize(), false);

    fileBuilder.addEntry(in, ZipLevel, "kit1.xml", Time());

    FileOutputStream zipOut(m_destination);

    zipOut.setPosition(0);
    zipOut.truncate();

    fileBuilder.writeToStream(zipOut, nullptr);

    return true;
}

String SitalaKitGenerator::getError() const
{
    return m_error;
}
