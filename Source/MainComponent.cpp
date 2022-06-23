#include "MainComponent.h"

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
