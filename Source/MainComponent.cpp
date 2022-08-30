#include "MainComponent.h"
#include "AbletonDeviceGroupReader.h"
#include "SitalaKitGenerator.h"

MainComponent::Label::Label(const String &text) :
    juce::Label(String(), text),
    m_textColour(findColour(Label::textColourId))
{
    setSizeHint(LayoutManager::Orientation::Vertical, Constraints::fixed(Drawing::ControlHeight));
    setMinimumHorizontalScale(1.0f);
}

void MainComponent::Label::setEnabled(bool enabled)
{
    setColour(Label::textColourId, enabled ? m_textColour : m_textColour.withAlpha(0.3f));
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

static const auto noneSelected = TRANS("None selected");

MainComponent::MainComponent() :
    LayoutManagedComponent(Orientation::Vertical),
    m_embedButton(TRANS("Embed samples in kit"), true),
    m_sameDirectoryButton(TRANS("Put Sitala kits into same folder as the Ableton kit"), true),
    m_specificDirectoryButton(TRANS("Put Sitala kits into a specific folder")),
    m_directoryLabel(noneSelected),
    m_convertButton(TRANS("Convert!"))
{
    setSize(400, 300);
    setBorderSizes(BorderSize(10));

    addSpacer();

    m_selectButton.setButtonText(TRANS("Select (or drop) Ablton Live kits..."));
    appendComponent(&m_selectButton, Constraints::fixed(Drawing::ButtonHeight));
    m_selectButton.onClick = [this] {
        FileChooser chooser(TRANS("Select Ableton Live drum kits..."), File(), "*.adg");
        if(chooser.browseForMultipleFilesToOpen())
        {
            setFilesToConvert(chooser.getResults());
        }
    };

    appendComponent(&m_fileCountLabel);

    addSpacer(Constraints::fixed(Drawing::ControlHeight));

    appendComponent(&m_embedButton);

    addSpacer(Constraints::fixed(Drawing::ControlHeight));

    m_sameDirectoryButton.setRadioGroupId(DirectoryButtonGroup);
    m_sameDirectoryButton.onClick = [this] {
        m_directoryLabel.setEnabled(false);
        m_directoryLabel.setText(noneSelected, dontSendNotification);
    };

    appendComponent(&m_sameDirectoryButton);

    m_directoryLabel.setEnabled(false);
    auto directoryLabelBorderSize = m_directoryLabel.getBorderSize();
    directoryLabelBorderSize.setLeft(26);
    m_directoryLabel.setBorderSize(directoryLabelBorderSize);

    m_specificDirectoryButton.setRadioGroupId(DirectoryButtonGroup);
    appendComponent(&m_specificDirectoryButton);
    appendComponent(&m_directoryLabel);

    m_specificDirectoryButton.onClick = [this] {
        if(!m_specificDirectoryButton.getToggleState())
        {
            return;
        }

        FileChooser chooser(TRANS("Select folder to place kits..."));
        if(chooser.browseForDirectory())
        {
            m_outputDirectory = chooser.getResult();
            m_directoryLabel.setEnabled(true);
            m_directoryLabel.setText(m_outputDirectory.getFullPathName(), dontSendNotification);
        }
        else
        {
            m_sameDirectoryButton.setToggleState(true, dontSendNotification);
            m_directoryLabel.setText(noneSelected, dontSendNotification);
        }
    };

    addSpacer();

    m_convertButton.setEnabled(false);
    appendComponent(&m_convertButton, Constraints::fixed(Drawing::ButtonHeight));
    m_convertButton.onClick = [this] {
        convert();
    };

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

void MainComponent::filesDropped(const StringArray &fileNames, int /* x */, int /* y */)
{
    setDragging(false);

    Array<File> files;

    for(auto name : fileNames)
    {
        File file(name);

        if(AbletonDeviceGroupReader::isAbletonKit(file))
        {
            files.add(file);
        }
    }

    if(!files.isEmpty())
    {
        setFilesToConvert(files);
    }
}

void MainComponent::setDragging(bool d)
{
    m_dragging = d;
    repaint();
}

void MainComponent::setFilesToConvert(const Array<File> &files)
{
    m_abletonKits = files;
    m_fileCountLabel.setText(String::formatted(TRANS("%i kit(s) selected"), files.size()),
                             dontSendNotification);
    m_convertButton.setEnabled(true);
}

void MainComponent::convert() const
{
    for(auto file : m_abletonKits)
    {
        if(!AbletonDeviceGroupReader::isAbletonKit(file))
        {
            continue;
        }

        AbletonDeviceGroupReader reader(file);

        auto path = m_sameDirectoryButton.getToggleState() ?
                    file.getParentDirectory().getFullPathName() :
                    m_outputDirectory.getFullPathName();

        auto sitalaKit = path +
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
