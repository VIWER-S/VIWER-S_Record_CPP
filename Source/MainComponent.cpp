#include "MainComponent.h"



//==============================================================================
MainComponent::MainComponent() : juce::AudioAppComponent(m_deviceManager)
{

    m_deviceManager.initialise(CHANNELS, 0, nullptr, true);
    //audioSettings.reset(new AudioDeviceSelectorComponent(customDeviceManager, 0, 0, 0, CHANNELS_OUT, false, false, false, false));

    juce::AudioDeviceManager::AudioDeviceSetup currentAudioSetup;
    m_deviceManager.getAudioDeviceSetup(currentAudioSetup);
    currentAudioSetup.bufferSize = BLOCKSIZE;
    currentAudioSetup.sampleRate = SAMPLINGRATE;
    m_deviceManager.setAudioDeviceSetup(currentAudioSetup, true);



    

    // Some platforms require permissions to open input channels so request that here
    if (juce::RuntimePermissions::isRequired(juce::RuntimePermissions::recordAudio)
        && !juce::RuntimePermissions::isGranted(juce::RuntimePermissions::recordAudio))
    {
        juce::RuntimePermissions::request(juce::RuntimePermissions::recordAudio,
            [&](bool granted) { setAudioChannels(granted ? 2 : CHANNELS, 0); });
    }
    else
    {
        // Specify the number of input and output channels that we want to open
        setAudioChannels(CHANNELS, 0);
    }

    for (int iChannel = 0; iChannel < CHANNELS; iChannel++) {
        for (int iSample = 0; iSample < BLOCKSIZE; iSample++) {
            m_buffer[iChannel][iSample] = 0.0f;
        }
        m_levels[iChannel] = 0.0f;
        m_levels_max[iChannel] = 0.0f;
    }

    m_ui_Record.setImages(true, false, true, juce::ImageCache::getFromMemory(BinaryData::button_Record_png, BinaryData::button_Record_pngSize), 1.0f,
        juce::Colours::transparentWhite, juce::ImageCache::getFromMemory(BinaryData::button_Record_png, BinaryData::button_Record_pngSize), 1.0f,
        juce::Colours::transparentWhite, juce::ImageCache::getFromMemory(BinaryData::button_Record_down_png, BinaryData::button_Record_down_pngSize), 1.0f,
        juce::Colours::transparentWhite);
    m_ui_Record.onClick = [this] { callbackRecord(); };
    addAndMakeVisible(m_ui_Record);

    m_ui_Data.setText(m_labelData, juce::dontSendNotification);
    addAndMakeVisible(m_ui_Data);

    m_ui_SamplesBlocksAndSeconds.setText("", juce::dontSendNotification);
    addAndMakeVisible(m_ui_SamplesBlocksAndSeconds);

    setSize(350, 240);

    startTimer(100);
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

    m_labelData << "Blocksize: " << samplesPerBlockExpected << "\n";
    m_labelData << "Samplerate: " << (int) sampleRate;

}

void MainComponent::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{

    //bufferToFill.clearActiveBufferRegion();
    for (int iChannel = 0; iChannel < CHANNELS; iChannel++) {

        const float* inBuffer = bufferToFill.buffer->getReadPointer(iChannel, bufferToFill.startSample);

        m_levels[iChannel] = rmsLevel(inBuffer);
        if (m_levels[iChannel] > m_levels_max[iChannel]) {
            m_levels_max[iChannel] = m_levels[iChannel];
        } else{
            m_levels_max[iChannel] = m_alpha_release * m_levels_max[iChannel] + (1.0f - m_alpha_release) * m_levels[iChannel];
        }

        for (int iSample = 0; iSample < BLOCKSIZE; iSample++) {
            m_buffer[iChannel][iSample] = inBuffer[iSample];
        }
    }

    if (m_isRecording) {
        m_WavWriter.write(m_buffer);
        m_numBlocks++;
    }

    
}

void MainComponent::releaseResources()
{
}

float MainComponent::rmsLevel(const float* inBuffer) {

    float sum = 0;

    for (int iSample = 0; iSample < BLOCKSIZE; iSample++) {
        sum += inBuffer[iSample] * inBuffer[iSample];
    }
    sum /= BLOCKSIZE;
    sum = std::sqrt(sum);
    sum = juce::Decibels::gainToDecibels(sum);

    return sum;
}


//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    m_ui_Data.setBounds(10, 10, 130, 40);

    m_ui_Record.setBounds(15, 80, 65, 50);

    m_ui_SamplesBlocksAndSeconds.setBounds(10, 150, 130, 80);

    int nBars = CHANNELS;
    int nWidth = 10;
    int nDistance = 4;
    int nOffset = 110;
    int nPosY = 74;

    for (int iBar = 0; iBar < nBars; iBar++) {

        if (m_levels[iBar] >= -40) {
            g.setColour(juce::Colours::red);
        }
        else if (m_levels[iBar] < -40 && m_levels[iBar] >= -50) {
            g.setColour(juce::Colours::orange);
        }
        else if (m_levels[iBar] < -50 && m_levels[iBar] >= -60) {
            g.setColour(juce::Colours::yellow);
        }
        else if (m_levels[iBar] < -60 && m_levels[iBar] >= -70) {
            g.setColour(juce::Colours::greenyellow);
        }
        else {
            g.setColour(juce::Colours::green);
        }

        g.fillRect(100 + nDistance + iBar * (nDistance + nWidth), -(int)m_levels[iBar] + nOffset - nPosY, nWidth, (int)m_levels[iBar] + nOffset);

        if (m_levels_max[iBar] >= -40) {
            g.setColour(juce::Colours::red);
        }
        else if (m_levels_max[iBar] < -40 && m_levels_max[iBar] >= -50) {
            g.setColour(juce::Colours::orange);
        }
        else if (m_levels_max[iBar] < -50 && m_levels_max[iBar] >= -60) {
            g.setColour(juce::Colours::yellow);
        }
        else if (m_levels_max[iBar] < -60 && m_levels_max[iBar] >= -70) {
            g.setColour(juce::Colours::greenyellow);
        }
        else {
            g.setColour(juce::Colours::green);
        }

        g.fillRect(100 + nDistance + iBar * (nDistance + nWidth), -(int)m_levels_max[iBar] + nOffset - nPosY, nWidth, 2);
    }

    g.setColour(juce::Colours::white);
    g.drawRect(100, 58, 228, 92);




}

void MainComponent::resized()
{
}

void MainComponent::callbackRecord() {

    if (m_isRecording && m_WavWriter.getClear()) {
        if (m_WavWriter.close()) {
            m_ui_Record.setImages(true, false, true, juce::ImageCache::getFromMemory(BinaryData::button_Record_png, BinaryData::button_Record_pngSize), 1.0f,
                juce::Colours::transparentWhite, juce::ImageCache::getFromMemory(BinaryData::button_Record_png, BinaryData::button_Record_pngSize), 1.0f,
                juce::Colours::transparentWhite, juce::ImageCache::getFromMemory(BinaryData::button_Record_down_png, BinaryData::button_Record_down_pngSize), 1.0f,
                juce::Colours::transparentWhite);
            m_isRecording = !m_isRecording;
        }
    }
    else if (!m_isRecording && m_WavWriter.getClear()) {
        if (m_WavWriter.init(SAMPLINGRATE, BITS_OUT, CHANNELS)) {
            m_ui_Record.setImages(true, false, true, juce::ImageCache::getFromMemory(BinaryData::button_Record_active_png, BinaryData::button_Record_active_pngSize), 1.0f,
                juce::Colours::transparentWhite, juce::ImageCache::getFromMemory(BinaryData::button_Record_active_png, BinaryData::button_Record_active_pngSize), 1.0f,
                juce::Colours::transparentWhite, juce::ImageCache::getFromMemory(BinaryData::button_Record_active_down_png, BinaryData::button_Record_active_down_pngSize), 1.0f,
                juce::Colours::transparentWhite);
            m_numBlocks = 0;
            m_isRecording = !m_isRecording;
        }
    }
}

void MainComponent::timerCallback() {

    m_otherData = "Data written:\n";
    m_otherData << "Blocks: " << (int)m_numBlocks << "\n";
    m_otherData << "Samples: " << (int)(m_numBlocks * BLOCKSIZE) << "\n";
    m_otherData << "Seconds: " << (int)(m_numBlocks * BLOCKSIZE / SAMPLINGRATE * 10) / 10;
    m_ui_SamplesBlocksAndSeconds.setText(m_otherData, juce::dontSendNotification);
    repaint();
}