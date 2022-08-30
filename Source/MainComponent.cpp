#include "MainComponent.h"
#include "AbletonDeviceGroupReader.h"
#include "SitalaKitGenerator.h"

MainComponent::MainComponent() :
    LayoutManagedComponent(Orientation::Vertical),
    m_dropLabel("dropLabel", TRANS("Drop Ableton Live devices here:")),
    m_embedButton(TRANS("Embed samples in kit"))
{
    setSize(400, 200);
    setBorderSizes(BorderSize(10));

    addSpacer();

    appendComponent(&m_dropLabel, Constraints::fixed(Drawing::ControlHeight));

    addSpacer(Constraints::fixed(Drawing::ControlHeight / 2));

    appendComponent(&m_embedButton, Constraints::fixed(Drawing::ControlHeight));
    m_embedButton.setToggleState(true, dontSendNotification);

    addSpacer();
}

MainComponent::~MainComponent()
{

}

void MainComponent::paint(juce::Graphics &g)
{
    auto colour = getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId);

    if(m_dragging)
    {
        colour = colour.brighter(0.1f);
    }

    g.fillAll(colour);

    LayoutManagedComponent::paint(g);
}

bool MainComponent::isInterestedInFileDrag(const StringArray &files)
{
    for(auto file : files)
    {
        if(AbletonDeviceGroupReader::isAbletonKit(file))
        {
            return true;
        }
    }

    return false;
}

void MainComponent::fileDragEnter(const StringArray & /* files */, int /* x */, int /* y */)
{
    setDragging(true);
}

void MainComponent::fileDragExit(const StringArray & /* files */)
{
    setDragging(false);
}

void MainComponent::filesDropped(const StringArray &files, int /* x */, int /* y */)
{
    setDragging(false);
    convert(files);
}

void MainComponent::setDragging(bool d)
{
    m_dragging = d;
    repaint();
}

void MainComponent::convert(const StringArray &files) const
{
    for(auto fileName : files)
    {
        File file(fileName);

        if(!AbletonDeviceGroupReader::isAbletonKit(file))
        {
            continue;
        }

        AbletonDeviceGroupReader reader(file);

        auto sitalaKit = file.getParentDirectory().getFullPathName() +
                         File::getSeparatorChar() +
                         file.getFileNameWithoutExtension() +
                         ".sitala";

        SitalaKitGenerator(sitalaKit,
                           reader.getSamples(),
                           (m_embedButton.getToggleState() ?
                            SitalaKitGenerator::Embedded :
                            SitalaKitGenerator::Referenced)).run();
    }
}
