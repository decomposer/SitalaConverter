#include "MainComponent.h"
#include "AbletonDeviceGroupReader.h"
#include "SitalaKitGenerator.h"

MainComponent::Label::Label(const String &text) : juce::Label(String(), text)
{
    setSizeHint(LayoutManager::Orientation::Vertical, Constraints::fixed(Drawing::ControlHeight));
}

MainComponent::ToggleButton::ToggleButton(const String &text, bool checked) : juce::ToggleButton(text)
{
    setToggleState(checked, dontSendNotification);
    setSizeHint(LayoutManager::Orientation::Vertical, Constraints::fixed(Drawing::ControlHeight));
}

enum
{
    DirectoryButtonGroup = 1001
};

MainComponent::MainComponent() :
    LayoutManagedComponent(Orientation::Vertical),
    m_dropLabel(TRANS("Drop Ableton Live devices here:")),
    m_embedButton(TRANS("Embed samples in kit"), true),
    m_sameDirectoryButton(TRANS("Put Sitala kits into same folder as the Ableton kit"), true),
    m_specificDirectoryButton(TRANS("Put Sitala kits into a specific folder")),
    m_directoryLabel(TRANS("None selected"))
{
    setSize(400, 300);
    setBorderSizes(BorderSize(10));

    addSpacer();

    appendComponent(&m_dropLabel);

    addSpacer(Constraints::fixed(Drawing::ControlHeight));

    appendComponent(&m_embedButton);

    addSpacer(Constraints::fixed(Drawing::ControlHeight));

    m_sameDirectoryButton.setRadioGroupId(DirectoryButtonGroup);
    appendComponent(&m_sameDirectoryButton);

    m_specificDirectoryButton.setRadioGroupId(DirectoryButtonGroup);
    appendComponent(&m_specificDirectoryButton);
    appendComponent(&m_directoryLabel);

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
