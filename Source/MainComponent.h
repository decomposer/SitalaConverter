#pragma once

#include <JuceHeader.h>
#include "LayoutManagedComponent.h"

class MainComponent : public LayoutManagedComponent, public FileDragAndDropTarget
{
    class Label : public juce::Label, public SizeHint
    {
    public:
        Label(const String &text = String());
        void setEnabled(bool enabled = true);
    private:
        Colour m_textColour;
    };

    class ToggleButton : public juce::ToggleButton, public SizeHint
    {
    public:
        ToggleButton(const String &text = String(), bool checked = false);
    };

public:
    MainComponent();
    ~MainComponent() override;

    void paint(juce::Graphics &g) override;

    bool isInterestedInFileDrag(const StringArray &files) override;
    void fileDragEnter(const StringArray &files, int x, int y) override;
    void fileDragExit(const StringArray &files) override;
    void filesDropped(const StringArray &files, int x, int y) override;

private:
    void setDragging(bool d);
    void convert(const StringArray &files) const;

    Label m_dropLabel;
    ToggleButton m_embedButton;

    ToggleButton m_sameDirectoryButton;
    ToggleButton m_specificDirectoryButton;

    Label m_directoryLabel;

    bool m_dragging = false;

    File m_outputDirectory;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
