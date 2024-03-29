#include "MainComponent.h"
#include "AbletonDeviceGroupReader.h"
#include "SitalaKitGenerator.h"

MainComponent::ResultsModel::ResultsModel()
{

}

void MainComponent::ResultsModel::addResult(const Result &result)
{
    m_results.add(result);
}

void MainComponent::ResultsModel::clear()
{
    m_results.clear();
}

int MainComponent::ResultsModel::getNumRows()
{
    return m_results.size();
}

void MainComponent::ResultsModel::paintRowBackground(Graphics &g,
                                                     int /* rowNumber */,
                                                     int /* width */,
                                                     int /* height */,
                                                     bool /* rowIsSelected */)
{
    g.fillAll(juce::Colours::lightblue);
}

String MainComponent::ResultsModel::getText(int row, int column)
{
    if(column == 1)
    {
        return column == 1 ? m_results[row].source.getFileName() : m_results[row].message;
    }

    return m_results[row].message.isEmpty() ? TRANS("Success") : m_results[row].message;
}

String MainComponent::ResultsModel::getCellTooltip(int row, int column)
{
    return getText(row, column);
}

void MainComponent::ResultsModel::paintCell(Graphics &g,
                                            int rowNumber,
                                            int columnId,
                                            int width,
                                            int height,
                                            bool /* rowIsSelected */)
{
    g.drawText(getText(rowNumber, columnId), 2, 0, width - 4, height, juce::Justification::centredLeft, true);
}

void MainComponent::ResultsModel::cellDoubleClicked(
    int rowNumber, int, const MouseEvent &)
{
    if(rowNumber < 0 || rowNumber >= m_results.size())
    {
        return;
    }

    const auto &output = m_results[rowNumber].output;

    if(!output.exists())
    {
        return;
    }

    m_results[rowNumber].output.revealToUser();
}

MainComponent::Label::Label(const String &text) :
    juce::Label(String(), text),
    m_textColour(findColour(Label::textColourId))
{
    setJustificationType(Justification::verticallyCentred);
    setMinimumHorizontalScale(1.0f);
}

LayoutManager::Constraints MainComponent::Label::getSizeHint(
    LayoutManager::Orientation orientation) const
{
    if(orientation == LayoutManager::Orientation::Vertical)
    {
        return Constraints::fixed(
            isEditable() ? Drawing::ControlHeight + 4 : Drawing::ControlHeight);
    }

    return SizeHint::getSizeHint(orientation);
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
    m_vendorLabel(TRANS("Vendor name for kits:")),
    m_results(String(), &m_resultsModel),
    m_convertButton(TRANS("Convert!"))
{
    PropertiesFile::Options preferenceOptions;
    preferenceOptions.applicationName = ProjectInfo::projectName;
    preferenceOptions.osxLibrarySubFolder = "Preferences";
    m_preferences.setStorageParameters(preferenceOptions);

    setSize(400, 600);
    setBorderSizes(BorderSize(10));

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

    appendComponent(&m_vendorLabel);
    m_vendorInput.setEditable(true);
    m_vendorInput.setColour(juce::Label::outlineColourId, juce::Colours::darkgrey);
    m_vendorInput.setText(m_preferences.getUserSettings()->getValue("vendor"), dontSendNotification);
    appendComponent(&m_vendorInput);

    appendComponent(&m_results, Constraints().setMinimum(Unit::Absolute, 200));

    auto column = 1;
    m_results.getHeader().addColumn(TRANS("File"), column++, m_results.getWidth() / 2,
                                    30, -1, TableHeaderComponent::notSortable);
    m_results.getHeader().addColumn(TRANS("Result"), column++, m_results.getWidth() / 2,
                                    30, -1, TableHeaderComponent::notSortable);

    m_results.setVisible(false);

    m_convertButton.setEnabled(false);
    appendComponent(&m_convertButton, Constraints::fixed(Drawing::ButtonHeight));
    m_convertButton.onClick = [this] {
        m_preferences.getUserSettings()->setValue("vendor", m_vendorInput.getText());
        convert();
    };
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
    m_results.setVisible(false);
    m_resultsModel.clear();
    m_results.updateContent();

    m_abletonKits = files;
    m_fileCountLabel.setText(String::formatted(TRANS("%i kit(s) selected"), files.size()),
                             dontSendNotification);
    m_convertButton.setEnabled(true);
}

void MainComponent::convert()
{
    Array<File> sitalaKits;

    m_results.setVisible(true);

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

        sitalaKits.add(sitalaKit);

        auto samples = reader.getSamples();

        if(samples.size() == 0)
        {
            addResult(Result { file, File(), TRANS("No samples found") });
            continue;
        }

        SitalaKitGenerator generator(sitalaKit);
        generator.setSamples(samples,
                             (m_embedButton.getToggleState() ?
                              SitalaKitGenerator::Embedded :
                              SitalaKitGenerator::Referenced));
        generator.setVendor(m_vendorInput.getText());

        if(generator.run())
        {
            if(samples.size() <= 16)
            {
                addResult(Result { file, File(sitalaKit), String() });
            }
            else
            {
                addResult(Result { file, File(sitalaKit),
                                   String::formatted("16 of %i samples added", samples.size()) });
            }
        }
        else
        {
            addResult(Result { file, File(), generator.getError() });
        }
    }
}

void MainComponent::addResult(const Result &result)
{
    m_resultsModel.addResult(result);
    m_results.updateContent();
}
