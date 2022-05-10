/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
*/
class BlobDriveAudioProcessor  : public juce::AudioProcessor, juce::AudioProcessorValueTreeState::Listener
{
public:
    //==============================================================================
    BlobDriveAudioProcessor();
    ~BlobDriveAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    //AudioProcessorValueTreeState
    juce::AudioProcessorValueTreeState& get_Apvts();

    //Save Presets
    juce::File getPresetsFolder();
    void savePreset(juce::String presetName);
    void loadPreset(juce::String presetName);
    //Delete Presets
    void deletePreset(juce::String presetName);

private:
    //AudioProcessorValueTreeState
    void parameterChanged(const juce::String& parameterID, float newValue) override;
    juce::AudioProcessorValueTreeState parameters;

    float bypass;

    //Oversampling x2 x4 x8 with clean sig Buffers
    float samplFactor;
    juce::dsp::Oversampling<float> oversampling2;
    juce::dsp::Oversampling<float> oversampling4;
    juce::dsp::Oversampling<float> oversampling8;
    juce::AudioSampleBuffer cleanSig2;
    juce::AudioSampleBuffer cleanSig4;
    juce::AudioSampleBuffer cleanSig8;

    //Low/High Pass
    float freq;
    float filterType;
    float lastSampleRate;
    juce::dsp::ProcessorDuplicator <juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients <float>> lowPassFilter;
    juce::dsp::ProcessorDuplicator <juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients <float>> highPassFilter;

    //Distortion function
    float g1;
    float g2;
    float sym;
    float wet;
    float Distortion(float samples);

    //Equalizer
    juce::AudioBuffer<float> lowBuff;
    juce::AudioBuffer<float> midlowBuff;
    juce::AudioBuffer<float> midhighBuff;
    juce::AudioBuffer<float> highBuff;
    float l;
    float ml;
    float mh;
    float h;
    juce::dsp::Gain<float> lowGain;
    juce::dsp::Gain<float> midlowGain;
    juce::dsp::Gain<float> midhighGain;
    juce::dsp::Gain<float> highGain;
    //low
    juce::dsp::ProcessorDuplicator <juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients <float>> lowPassFilterEq;
    //midlow
    juce::dsp::ProcessorDuplicator <juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients <float>> lowPassFilterMidLowEq;
    juce::dsp::ProcessorDuplicator <juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients <float>> highPassFilterMidLowEq;
    //midhigh
    juce::dsp::ProcessorDuplicator <juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients <float>> lowPassFilterMidHighEq;
    juce::dsp::ProcessorDuplicator <juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients <float>> highPassFilterMidHighEq;
    //high
    juce::dsp::ProcessorDuplicator <juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients <float>> highPassFilterEq;

    //Volume Output
    float vol;
    juce::dsp::Gain<float> outputVolume;



    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BlobDriveAudioProcessor)
};
