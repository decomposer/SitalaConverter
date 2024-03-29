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
        LayoutManager::Constraints getSizeHint(
            LayoutManager::Orientation orientation) const override;

    private:
        Colour m_textColour;
    };

    class ToggleButton : public juce::ToggleButton, public SizeHint
    {
    public:
        ToggleButton(const String &text = String(), bool checked = false);
    };

    struct Result
    {
        File source;
        File output;
        String message;
    };

    class ResultsModel : public TableListBoxModel
    {
    public:
        ResultsModel();
        void addResult(const Result &result);
        void clear();
    protected:
        int getNumRows() override;
        String getText(int row, int column);
        String getCellTooltip(int row, int column) override;
        void paintRowBackground(Graphics &g,
                                int rowNumber,
                                int width,
                                int height,
                                bool rowIsSelected) override;
        void paintCell(Graphics &g,
                       int rowNumber,
                       int columnId,
                       int width,
                       int height,
                       bool rowIsSelected) override;
        void cellDoubleClicked(int rowNumber, int columnId, const MouseEvent &) override;
    private:
        Array<Result> m_results;
    };

public:
    MainComponent();
    ~MainComponent() override;

    void paint(juce::Graphics &g) override;

    bool isInterestedInFileDrag(const StringArray &files) override;
    void fileDragEnter(const StringArray &files, int x, int y) override;
    void fileDragExit(const StringArray &files) override;
    void filesDropped(const StringArray &fileNames, int x, int y) override;

private:
    void setDragging(bool d);
    void setFilesToConvert(const Array<File> &files);
    void convert();
    void addResult(const Result &result);

    ApplicationProperties m_preferences;

    TextButton m_selectButton;
    Label m_fileCountLabel;

    ToggleButton m_embedButton;

    ToggleButton m_sameDirectoryButton;
    ToggleButton m_specificDirectoryButton;

    Label m_directoryLabel;

    Label m_vendorLabel;
    Label m_vendorInput;

    ResultsModel m_resultsModel;
    TableListBox m_results;

    TextButton m_convertButton;

    bool m_dragging = false;

    Array<File> m_abletonKits;
    File m_outputDirectory;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
