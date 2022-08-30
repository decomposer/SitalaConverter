#pragma once

#include <JuceHeader.h>
#include "LayoutManagedComponent.h"

class MainComponent : public LayoutManagedComponent, public FileDragAndDropTarget
{
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

    bool m_dragging = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
