#pragma once

#include <JuceHeader.h>
#include "WavWriter.h"
#include "Constants.h"


//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent  : public juce::AudioAppComponent, public juce::Timer
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent() override;

    //==============================================================================
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    //==============================================================================
    void paint (juce::Graphics& g) override;
    void resized() override;

private:
    //==============================================================================

    void timerCallback() override;
    void callbackRecord();
    void callbackStop();
    float rmsLevel(const float* inBuffer);

    float m_buffer[CHANNELS][BLOCKSIZE];
    float m_levels[CHANNELS];
    float m_levels_max[CHANNELS];

    float m_alpha_release = 0.95;

    juce::AudioDeviceManager m_deviceManager;

    juce::ImageButton m_ui_Record;
    juce::Label m_ui_Data;
    juce::String m_labelData;
    juce::Label m_ui_SamplesBlocksAndSeconds;
    juce::String m_otherData;

    WavWriter m_WavWriter;
    int m_numBlocks = 0;
    bool m_isRecording = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
