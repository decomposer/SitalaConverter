#include "MainComponent.h"
#include "AbletonDeviceGroupReader.h"

MainComponent::MainComponent()
{
    setSize(600, 400);
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
    g.setFont(juce::Font(16.0f));
    g.setColour(juce::Colours::white);
    g.drawText(TRANS("Drop Ableton Live Devices Here"),
               getLocalBounds(),
               juce::Justification::centred, true);
}

void MainComponent::resized()
{

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
}

void MainComponent::setDragging(bool d)
{
    m_dragging = d;
    repaint();
}
