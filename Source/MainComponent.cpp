#include "MainComponent.h"

#define ADG_EXTENSION ".adg"

MainComponent::MainComponent()
{
    setSize(600, 400);
}

MainComponent::~MainComponent()
{

}

void MainComponent::paint(juce::Graphics &g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
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
        if(file.endsWith(ADG_EXTENSION))
        {
            return true;
        }
    }

    return false;
}

void MainComponent::filesDropped(const StringArray &files, int /* x */, int /* y */)
{

}
