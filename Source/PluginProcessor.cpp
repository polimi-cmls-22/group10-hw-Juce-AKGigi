/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
BlobDriveAudioProcessor::BlobDriveAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
    ),
    parameters(*this, nullptr, "Parameters", {
    //list of parameters
    std::make_unique<juce::AudioParameterBool>("bypass", "BYPASS", false), //1 bypass on, 0  bypass off

    std::make_unique<juce::AudioParameterFloat>("samplFact", "SAMPLFACT", juce::NormalisableRange<float>{0.0, 3.0, 1.0}, 0.0),

    std::make_unique<juce::AudioParameterFloat>("freq", "FREQ", juce::NormalisableRange<float>{7.0, 13.0, 0.1}, 12.2877),
    std::make_unique<juce::AudioParameterBool>("filter", "FILTER", 0.0), //1 hpf, 0 lpf

    std::make_unique<juce::AudioParameterFloat>("g1", "G1", juce::NormalisableRange<float>{0.0, 10.0, 0.1}, 5.0),
    std::make_unique<juce::AudioParameterFloat>("g2", "G2", juce::NormalisableRange<float>{0.0, 10.0, 0.1}, 5.0),
    std::make_unique<juce::AudioParameterFloat>("dw", "DW", juce::NormalisableRange<float>{0.0, 1.0, 0.01}, 1.0),
    std::make_unique<juce::AudioParameterBool>("sym", "SYM", true),

    std::make_unique<juce::AudioParameterFloat>("low", "LOW", juce::NormalisableRange<float>{-12.0, 12.0, 0.1}, 0.0),
    std::make_unique<juce::AudioParameterFloat>("midlow", "MIDLOW", juce::NormalisableRange<float>{-12.0, 12.0, 0.1}, 0.0),
    std::make_unique<juce::AudioParameterFloat>("midhigh", "MIDHIGH", juce::NormalisableRange<float>{-12.0, 12.0, 0.1}, 0.0),
    std::make_unique<juce::AudioParameterFloat>("high", "HIGH", juce::NormalisableRange<float>{-12.0, 12.0, 0.1}, 0.0),

    std::make_unique<juce::AudioParameterFloat>("volume", "VOLUME", juce::NormalisableRange<float>{-60.0, 6.0, 0.1, 1.8}, 0.0f)
        }),

    //Define Oversampling 
    oversampling2(2, 1, juce::dsp::Oversampling<float>::FilterType::filterHalfBandPolyphaseIIR),
    oversampling4(2, 2, juce::dsp::Oversampling<float>::FilterType::filterHalfBandPolyphaseIIR),
    oversampling8(2, 3, juce::dsp::Oversampling<float>::FilterType::filterHalfBandPolyphaseIIR),

    //Define low/High pass filter
    lowPassFilter(juce::dsp::IIR::Coefficients<float>::makeLowPass(44100, 200.0, 0.707)),
    highPassFilter(juce::dsp::IIR::Coefficients<float>::makeHighPass(44100, 200.0, 0.707)),

    //Define Equalizer
    //low
    lowPassFilterEq(juce::dsp::IIR::Coefficients<float>::makeLowPass(44100, 200.0, 0.707)),
    //midlow
    lowPassFilterMidLowEq(juce::dsp::IIR::Coefficients<float>::makeLowPass(44100, 1000.0, 0.707)),
    highPassFilterMidLowEq(juce::dsp::IIR::Coefficients<float>::makeHighPass(44100, 200.0, 0.707)),
    //midhigh
    lowPassFilterMidHighEq(juce::dsp::IIR::Coefficients<float>::makeLowPass(44100, 5000.0, 0.707)),
    highPassFilterMidHighEq(juce::dsp::IIR::Coefficients<float>::makeHighPass(44100, 1000.0, 0.707)),
    //high
    highPassFilterEq(juce::dsp::IIR::Coefficients<float>::makeHighPass(44100, 5000.0, 0.707))
#endif
{
    //parameters.addParameterListener("ID", this);
    parameters.addParameterListener("bypass", this);

    parameters.addParameterListener("samplFact", this);
    parameters.addParameterListener("freq", this);
    parameters.addParameterListener("filter", this);

    parameters.addParameterListener("g1", this);
    parameters.addParameterListener("g2", this);
    parameters.addParameterListener("dw", this);
    parameters.addParameterListener("sym", this);

    parameters.addParameterListener("low", this);
    parameters.addParameterListener("midlow", this);
    parameters.addParameterListener("midhigh", this);
    parameters.addParameterListener("high", this);

    parameters.addParameterListener("volume", this);
}

BlobDriveAudioProcessor::~BlobDriveAudioProcessor()
{
    //parameters.removeParameterListener("ID", this);
    parameters.removeParameterListener("bypass", this);

    parameters.removeParameterListener("samplFact", this);
    parameters.removeParameterListener("freq", this);
    parameters.removeParameterListener("filter", this);

    parameters.removeParameterListener("g1", this);
    parameters.removeParameterListener("g2", this);
    parameters.removeParameterListener("dw", this);
    parameters.removeParameterListener("sym", this);

    parameters.removeParameterListener("low", this);
    parameters.removeParameterListener("midlow", this);
    parameters.removeParameterListener("midhigh", this);
    parameters.removeParameterListener("high", this);

    parameters.removeParameterListener("volume", this);
}

//==============================================================================
const juce::String BlobDriveAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool BlobDriveAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool BlobDriveAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool BlobDriveAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double BlobDriveAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int BlobDriveAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int BlobDriveAudioProcessor::getCurrentProgram()
{
    return 0;
}

void BlobDriveAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String BlobDriveAudioProcessor::getProgramName (int index)
{
    return {};
}

void BlobDriveAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void BlobDriveAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    //spec
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumInputChannels();

    //Clean sig buffers are created
    cleanSig2.setSize(getNumInputChannels(), 2 * samplesPerBlock);
    cleanSig4.setSize(getNumInputChannels(), 4 * samplesPerBlock);
    cleanSig8.setSize(getNumInputChannels(), 8 * samplesPerBlock);

    //bypass = parameters.getParameter("bypass")->getValue();
    bypass = *parameters.getRawParameterValue("bypass");

    //init Oversampling processes
    //samplFactor = parameters.getParameter("samplFact")->getValue();
    samplFactor = *parameters.getRawParameterValue("samplFact");
    oversampling2.initProcessing(samplesPerBlock);
    oversampling4.initProcessing(samplesPerBlock);
    oversampling8.initProcessing(samplesPerBlock);

    //init Low/High Pass Filter
    freq = pow(2, *parameters.getRawParameterValue("freq"));
    filterType = *parameters.getRawParameterValue("filter");
    lastSampleRate = sampleRate;
    lowPassFilter.prepare(spec);
    lowPassFilter.reset();
    highPassFilter.prepare(spec);
    highPassFilter.reset();

    //Distortion
    g1 = *parameters.getRawParameterValue("g1");
    g2 = *parameters.getRawParameterValue("g2");
    sym = *parameters.getRawParameterValue("sym");
    wet = *parameters.getRawParameterValue("dw");

    //init Equalizer
    lowBuff.setSize(getNumInputChannels(), samplesPerBlock);
    midlowBuff.setSize(getNumInputChannels(), samplesPerBlock);
    midhighBuff.setSize(getNumInputChannels(), samplesPerBlock);
    highBuff.setSize(getNumInputChannels(), samplesPerBlock);
    /*lowBuff.clear();
    midlowBuff.clear();
    midhighBuff.clear();
    highBuff.clear();*/
    l = *parameters.getRawParameterValue("low");
    ml = *parameters.getRawParameterValue("midlow");
    mh = *parameters.getRawParameterValue("midhigh");
    h = *parameters.getRawParameterValue("high");
    lowGain.prepare(spec);
    lowGain.reset();
    midlowGain.prepare(spec);
    midlowGain.reset();
    midhighGain.prepare(spec);
    midhighGain.reset();
    highGain.prepare(spec);
    highGain.reset();
    //low
    lowPassFilterEq.prepare(spec);
    lowPassFilterEq.reset();
    //midlow
    lowPassFilterMidLowEq.prepare(spec);
    lowPassFilterMidLowEq.reset();
    highPassFilterMidLowEq.prepare(spec);
    highPassFilterMidLowEq.reset();
    //midhigh
    lowPassFilterMidHighEq.prepare(spec);
    lowPassFilterMidHighEq.reset();
    highPassFilterMidHighEq.prepare(spec);
    highPassFilterMidHighEq.reset();
    //high
    highPassFilterEq.prepare(spec);
    highPassFilterEq.reset();

    //init outputVolume
    vol = *parameters.getRawParameterValue("volume");
    outputVolume.prepare(spec);
    outputVolume.reset();
}

void BlobDriveAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool BlobDriveAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void BlobDriveAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    //Audio Block for oversampling
    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::AudioBlock<float> upblock(buffer);
    juce::dsp::AudioBlock<float> cleanSigblock2(cleanSig2);
    juce::dsp::AudioBlock<float> cleanSigblock4(cleanSig4);
    juce::dsp::AudioBlock<float> cleanSigblock8(cleanSig8);


    //////////////////////////////////////////////////////////////////////////////////////////////////////
    // Plugin Implementation: 
    // Upsampling -> Low/High Pass Filter -> Distortion Function (dry/wet) -> Downsampling (+filter) -> EQ
    //////////////////////////////////////////////////////////////////////////////////////////////////////

    if (!bypass) {

        //No Oversampling
        if (samplFactor == 0.0) {

            
            ///////////////////
            // Low/High Pass
            ///////////////////
            if (filterType == 0.0) {
                *lowPassFilter.state = *juce::dsp::IIR::Coefficients<float>::makeLowPass(lastSampleRate, freq, 0.707);
                lowPassFilter.process(juce::dsp::ProcessContextReplacing<float>(block));
            }
            else if (filterType == 1.0) {
                *highPassFilter.state = *juce::dsp::IIR::Coefficients<float>::makeHighPass(lastSampleRate, freq, 0.707);
                highPassFilter.process(juce::dsp::ProcessContextReplacing<float>(block));
            }

            ///////////////////
            // Distortion
            ///////////////////
            for (int sample = 0; sample < block.getNumSamples(); ++sample)
            {
                for (int ch = 0; ch < block.getNumChannels(); ++ch)
                {
                    float* data = block.getChannelPointer(ch);
                    float* dataClean = block.getChannelPointer(ch);
                    data[sample] = (wet * Distortion(data[sample])) + ((1 - wet) * dataClean[sample]);
                }
            }
            

        }

        //Oversampling x2
        else if (samplFactor == 1.0) {
            upblock = oversampling2.processSamplesUp(block);
            cleanSigblock2.copyFrom(upblock);

            ///////////////////
            // Low/High Pass
            ///////////////////
            if (filterType == 0.0) {
                *lowPassFilter.state = *juce::dsp::IIR::Coefficients<float>::makeLowPass(lastSampleRate*2, freq, 0.707);
                lowPassFilter.process(juce::dsp::ProcessContextReplacing<float>(upblock));
            }
            else if (filterType == 1.0) {
                *highPassFilter.state = *juce::dsp::IIR::Coefficients<float>::makeHighPass(lastSampleRate*2, freq, 0.707);
                highPassFilter.process(juce::dsp::ProcessContextReplacing<float>(upblock));
            }

            ///////////////////
            // Distortion
            ///////////////////
            for (int sample = 0; sample < upblock.getNumSamples(); ++sample)
            {
                for (int ch = 0; ch < upblock.getNumChannels(); ++ch)
                {
                    float* data = upblock.getChannelPointer(ch);
                    float* dataClean = cleanSigblock2.getChannelPointer(ch);
                    data[sample] = (wet * Distortion(data[sample])) + ((1 - wet) * dataClean[sample]);
                }
            }

            oversampling2.processSamplesDown(block);
        }

        //Oversampling x4
        else if (samplFactor == 2.0) {
            upblock = oversampling4.processSamplesUp(block);
            cleanSigblock4.copyFrom(upblock);

            ///////////////////
            // Low/High Pass
            ///////////////////
            if (filterType == 0.0) {
                *lowPassFilter.state = *juce::dsp::IIR::Coefficients<float>::makeLowPass(lastSampleRate*4, freq, 0.707);
                lowPassFilter.process(juce::dsp::ProcessContextReplacing<float>(upblock));
            }
            else if (filterType == 1.0) {
                *highPassFilter.state = *juce::dsp::IIR::Coefficients<float>::makeHighPass(lastSampleRate*4, freq, 0.707);
                highPassFilter.process(juce::dsp::ProcessContextReplacing<float>(upblock));
            }

            ///////////////////
            // Distortion
            ///////////////////
            for (int sample = 0; sample < upblock.getNumSamples(); ++sample)
            {
                for (int ch = 0; ch < upblock.getNumChannels(); ++ch)
                {
                    float* data = upblock.getChannelPointer(ch);
                    float* dataClean = cleanSigblock4.getChannelPointer(ch);
                    data[sample] = (wet * Distortion(data[sample])) + ((1 - wet) * dataClean[sample]);
                }
            }

            oversampling4.processSamplesDown(block);
        }

        //Oversampling x8
        else if (samplFactor == 3.0) {
            upblock = oversampling8.processSamplesUp(block);
            cleanSigblock8.copyFrom(upblock);

            ///////////////////
            // Low/High Pass
            ///////////////////
            if (filterType == 0.0) {
                *lowPassFilter.state = *juce::dsp::IIR::Coefficients<float>::makeLowPass(lastSampleRate*8, freq, 0.707);
                lowPassFilter.process(juce::dsp::ProcessContextReplacing<float>(upblock));
            }
            else if (filterType == 1.0) {
                *highPassFilter.state = *juce::dsp::IIR::Coefficients<float>::makeHighPass(lastSampleRate*8, freq, 0.707);
                highPassFilter.process(juce::dsp::ProcessContextReplacing<float>(upblock));
            }

            ///////////////////
            // Distortion
            ///////////////////
            for (int sample = 0; sample < upblock.getNumSamples(); ++sample)
            {
                for (int ch = 0; ch < upblock.getNumChannels(); ++ch)
                {
                    float* data = upblock.getChannelPointer(ch);
                    float* dataClean = cleanSigblock8.getChannelPointer(ch);
                    data[sample] = (wet * Distortion(data[sample])) + ((1 - wet) * dataClean[sample]);
                }
            }

            oversampling8.processSamplesDown(block);
        }

        
        ////////////////////
        /// Equalizer
        ////////////////////

        lowBuff.makeCopyOf(buffer);
        midlowBuff.makeCopyOf(buffer);
        midhighBuff.makeCopyOf(buffer);
        highBuff.makeCopyOf(buffer);
        //Audio Block for Equalizer
        juce::dsp::AudioBlock <float> lblock(lowBuff);
        juce::dsp::AudioBlock <float> mlblock(midlowBuff);
        juce::dsp::AudioBlock <float> mhblock(midhighBuff);
        juce::dsp::AudioBlock <float> hblock(highBuff);

        lblock.copyFrom(block);
        mlblock.copyFrom(block);
        mhblock.copyFrom(block);
        hblock.copyFrom(block);
        //low
        *lowPassFilterEq.state = *juce::dsp::IIR::Coefficients<float>::makeLowPass(lastSampleRate, 200.0, 0.707);
        lowPassFilterEq.process(juce::dsp::ProcessContextReplacing<float>(lblock));

        //midlow
        *lowPassFilterMidLowEq.state = *juce::dsp::IIR::Coefficients<float>::makeLowPass(lastSampleRate, 1000.0, 0.707);
        lowPassFilterMidLowEq.process(juce::dsp::ProcessContextReplacing<float>(mlblock));

        *highPassFilterMidLowEq.state = *juce::dsp::IIR::Coefficients<float>::makeHighPass(lastSampleRate, 200.0, 0.707);
        highPassFilterMidLowEq.process(juce::dsp::ProcessContextReplacing<float>(mlblock));

        //midhigh
        *lowPassFilterMidHighEq.state = *juce::dsp::IIR::Coefficients<float>::makeLowPass(lastSampleRate, 5000.0, 0.707);
        lowPassFilterMidHighEq.process(juce::dsp::ProcessContextReplacing<float>(mhblock));

        *highPassFilterMidHighEq.state = *juce::dsp::IIR::Coefficients<float>::makeHighPass(lastSampleRate, 1000.0, 0.707);
        highPassFilterMidHighEq.process(juce::dsp::ProcessContextReplacing<float>(mhblock));

        //high
        *highPassFilterEq.state = *juce::dsp::IIR::Coefficients<float>::makeHighPass(lastSampleRate, 5000.0, 0.707);
        highPassFilterEq.process(juce::dsp::ProcessContextReplacing<float>(hblock));

        lowGain.setGainDecibels(l);
        midlowGain.setGainDecibels(ml);
        midhighGain.setGainDecibels(mh);
        highGain.setGainDecibels(h);

        lowGain.process(juce::dsp::ProcessContextReplacing<float>(lblock));
        midlowGain.process(juce::dsp::ProcessContextReplacing<float>(mlblock));
        midhighGain.process(juce::dsp::ProcessContextReplacing<float>(mhblock));
        highGain.process(juce::dsp::ProcessContextReplacing<float>(hblock));

        block.replaceWithSumOf(lblock, mlblock);
        block.replaceWithSumOf(block, mhblock);
        block.replaceWithSumOf(block, hblock);

     

        //OutputVolume
        outputVolume.setGainDecibels(vol);
        outputVolume.process(juce::dsp::ProcessContextReplacing<float>(block));
    }

    /*else {
        DBG("Dist Off");
    }*/
}

//==============================================================================
bool BlobDriveAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* BlobDriveAudioProcessor::createEditor()
{
    return new BlobDriveAudioProcessorEditor (*this);
}

//==============================================================================
void BlobDriveAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void BlobDriveAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(parameters.state.getType()))
            parameters.replaceState(juce::ValueTree::fromXml(*xmlState));
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new BlobDriveAudioProcessor();
}
//////////////////////////////////
//AudioProcessorValueTreeState
//////////////////////////////////
juce::AudioProcessorValueTreeState& BlobDriveAudioProcessor::get_Apvts()
{
    return parameters;
}

//////////////////////////////////
// Distortion Function
//////////////////////////////////

float BlobDriveAudioProcessor::Distortion(float samples)
{
    float sign;
    if (samples < 0) {
        if (sym == 1) {
            sign = -1;
        }
        else
            sign = 1;

        samples = sign * (1 - exp(-1 * g1 * abs(samples)));
    }
    else {
        samples = (1 - exp(-1 * g2 * abs(samples)));
    }
    return samples;
}

void BlobDriveAudioProcessor::parameterChanged(const juce::String& parameterID, float newValue)
{
    if (parameterID == "samplFact") {
        samplFactor = newValue;
    }
    else if(parameterID == "bypass") {
        bypass = newValue;
    }
    else if (parameterID == "freq") {
        freq = pow(2, newValue);
    }
    else if (parameterID == "filter") {
        filterType = newValue; 
    }
    else if (parameterID == "g1") {
        g1 = newValue;
    }
    else if (parameterID == "g2") {
        g2 = newValue;
    }
    else if (parameterID == "sym") {
        sym = newValue;
    }
    else if (parameterID == "dw") {
        wet = newValue;
    }
    else if (parameterID == "low") {
        l = newValue;
    }
    else if (parameterID == "midlow") {
        ml = newValue;
    }
    else if (parameterID == "midhigh") {
        mh = newValue;
    }
    else if (parameterID == "high") {
        h = newValue;
    }
    else if (parameterID == "volume") {
        vol = newValue;
    }
}

/////////////
//Preset
/////////////

//Get Preset Folder
juce::File BlobDriveAudioProcessor::getPresetsFolder()
{
    juce::File rootFolder = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory);

    auto folder{ rootFolder.getChildFile("AKGigi").getChildFile("BlobDrivePresets")};
    if (folder.createDirectory().wasOk())
        rootFolder = folder;

    return rootFolder;
}

//Save Preset
void BlobDriveAudioProcessor::savePreset(juce::String presetName)
{
    juce::File writingTo = getPresetsFolder().getChildFile(presetName);

    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());

    bool existed = writingTo.existsAsFile();

    xml->writeTo(writingTo);

    if (!existed)
        writingTo.setCreationTime(juce::Time::getCurrentTime());

    writingTo.setLastModificationTime(juce::Time::getCurrentTime());
}

//Load Preset
void BlobDriveAudioProcessor::loadPreset(juce::String presetName)
{
    juce::File presetToLoad = getPresetsFolder().getChildFile(presetName);

    if (presetToLoad.existsAsFile())
    {
        auto xml{ juce::XmlDocument::parse(presetToLoad) };
        parameters.replaceState(juce::ValueTree::fromXml(*xml));
        presetToLoad.setLastAccessTime(juce::Time::getCurrentTime());
    }
}

//Delete Preset
void BlobDriveAudioProcessor::deletePreset(juce::String presetName)
{
    juce::File presetToDelete = getPresetsFolder().getChildFile(presetName);
  
    if (presetToDelete.existsAsFile())
    {
        presetToDelete.moveToTrash();
    }
}