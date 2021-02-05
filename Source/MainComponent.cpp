#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
{
    juce::AudioDeviceManager::AudioDeviceSetup currentAudioSetup;
    deviceManager.getAudioDeviceSetup(currentAudioSetup);
    currentAudioSetup.bufferSize = 256;
    currentAudioSetup.sampleRate = 44100;
    deviceManager.setAudioDeviceSetup(currentAudioSetup, true);


    juce::AudioDeviceManager::AudioDeviceSetup deviceSetup = juce::AudioDeviceManager::AudioDeviceSetup();

    deviceManager.initialise(16, 0, 0, true, std::string(), &deviceSetup);

    // Some platforms require permissions to open input channels so request that here
    if (juce::RuntimePermissions::isRequired(juce::RuntimePermissions::recordAudio)
        && !juce::RuntimePermissions::isGranted(juce::RuntimePermissions::recordAudio))
    {
        juce::RuntimePermissions::request(juce::RuntimePermissions::recordAudio,
            [&](bool granted) { setAudioChannels(granted ? 2 : 16, 0); });
    }
    else
    {
        // Specify the number of input and output channels that we want to open
        setAudioChannels(16, 0);
    }

    //startTimer(100);
}

MainComponent::~MainComponent()
{
    // This shuts down the audio device and clears the audio source.
    shutdownAudio();
    stopTimer();
}

//==============================================================================
void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    // This function will be called when the audio device is started, or when
    // its settings (i.e. sample rate, block size, etc) are changed.

    // You can use this function to initialise any resources you might need,
    // but be careful - it will be called on the audio thread, not the GUI thread.

    // For more details, see the help for AudioProcessor::prepareToPlay()
}

void MainComponent::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{

    //bufferToFill.clearActiveBufferRegion();



}

void MainComponent::releaseResources()
{
    // This will be called when the audio device stops, or when it is being
    // restarted due to a setting change.

    // For more details, see the help for AudioProcessor::releaseResources()
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    // You can add your drawing code here!
}

void MainComponent::resized()
{
    // This is called when the MainContentComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.
}
