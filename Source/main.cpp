#include "JuceHeader.h"
#include "MainComponent.h"

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

START_JUCE_APPLICATION(SitalaConverterApplication)
