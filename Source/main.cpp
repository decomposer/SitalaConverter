#include "../JuceLibraryCode/JuceHeader.h"

#include "AbletonDeviceGroupReader.h"
#include "SitalaKitGenerator.h"
#include "Config.h"
#include "MainComponent.h"

#include <algorithm>

class SitalaConverterApplication  : public juce::JUCEApplication
{
public:
    SitalaConverterApplication() {}

    const juce::String getApplicationName() override
    {
        return ProjectInfo::projectName;
    }
    const juce::String getApplicationVersion() override
    {
        return ProjectInfo::versionString;
    }

    bool moreThanOneInstanceAllowed() override
    {
        return true;
    }

    void initialise(const juce::String & /* commandLine */) override
    {
        mainWindow.reset(new MainWindow(getApplicationName()));
    }

    void shutdown() override
    {
        mainWindow = nullptr;
    }

    void systemRequestedQuit() override
    {
        quit();
    }

    void anotherInstanceStarted(const juce::String & /* commandLine */) override
    {

    }

    class MainWindow : public juce::DocumentWindow
    {
    public:
        MainWindow(juce::String name)
            : DocumentWindow(
                name,
                juce::Desktop::getInstance().getDefaultLookAndFeel().findColour(
                    juce::ResizableWindow::backgroundColourId),
                DocumentWindow::allButtons)
        {
            setUsingNativeTitleBar(true);
            setContentOwned(new MainComponent(), true);

#if JUCE_IOS || JUCE_ANDROID
            setFullScreen(true);
#else
            setResizable(true, true);
            centreWithSize(getWidth(), getHeight());
#endif

            setVisible(true);
        }

        void closeButtonPressed() override
        {
            JUCEApplication::getInstance()->systemRequestedQuit();
        }

    private:
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainWindow)
    };

private:
    std::unique_ptr<MainWindow> mainWindow;
};

static std::map<String, std::vector<File>> filesFromTextFile(const File &textFile)
{
    std::map<String, std::vector<File>> kits;
    String name;

    StringArray lines;
    textFile.readLines(lines);

    for(auto &line : lines)
    {
        if(line.trim().isEmpty())
        {
            continue;
        }

        if(line.startsWithChar('#'))
        {
            name = line.replaceFirstOccurrenceOf("#", "").trim();
            continue;
        }

        auto dir = textFile.getParentDirectory().getFullPathName();
        auto sample = File::addTrailingSeparator(dir) + line.trim();
        kits[name].push_back(File(sample));
    }

    return kits;
}

START_JUCE_APPLICATION(SitalaConverterApplication)
