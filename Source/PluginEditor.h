/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "Components/FilmStripKnob.h"
#include "Components/FunctionGraph.h"

//==============================================================================
/**
*/
class BlobDriveAudioProcessorEditor  : public juce::AudioProcessorEditor, private juce::Slider::Listener
{
public:
    BlobDriveAudioProcessorEditor (BlobDriveAudioProcessor&);
    ~BlobDriveAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    void changeLines();

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    BlobDriveAudioProcessor& audioProcessor;
    void sliderValueChanged(juce::Slider* slider) override;

    juce::Image BackGround;
    juce::Rectangle<float> linesArea;

    //Sampling Knob
    juce::Image samplingKnobImg = juce::ImageCache::getFromMemory(BinaryData::SamplingKnobImg_png, BinaryData::SamplingKnobImg_pngSize);
    FilmStripKnob samplingKnob;

    //FilterKnob
    juce::Image filterKnobImg = juce::ImageCache::getFromMemory(BinaryData::FilterKnobImg_png, BinaryData::FilterKnobImg_pngSize);
    FilmStripKnob filterKnob;

    //G1-G2 dist Knob
    juce::Image g1distKnobImg = juce::ImageCache::getFromMemory(BinaryData::g1KnobImg_png, BinaryData::g1KnobImg_pngSize);
    juce::Image g2distKnobImg = juce::ImageCache::getFromMemory(BinaryData::g2KnobImg_png, BinaryData::g2KnobImg_pngSize);
    FilmStripKnob g1Knob;
    FilmStripKnob g2Knob;

    //drywet Knob
    juce::Image drywetKnobImg = juce::ImageCache::getFromMemory(BinaryData::drywetKnobImg_png, BinaryData::drywetKnobImg_pngSize);
    FilmStripKnob drywetKnob;

    //eq Knob
    juce::Image highKnobImg = juce::ImageCache::getFromMemory(BinaryData::highKnobImg_png, BinaryData::highKnobImg_pngSize);
    juce::Image midhighKnobImg = juce::ImageCache::getFromMemory(BinaryData::midhighKnobImg_png, BinaryData::midhighKnobImg_pngSize);
    juce::Image midlowKnobImg = juce::ImageCache::getFromMemory(BinaryData::midlowKnobImg_png, BinaryData::midlowKnobImg_pngSize);
    juce::Image lowKnobImg = juce::ImageCache::getFromMemory(BinaryData::lowKnobImg_png, BinaryData::lowKnobImg_pngSize);
    FilmStripKnob highKnob;
    FilmStripKnob midhighKnob;
    FilmStripKnob midlowKnob;
    FilmStripKnob lowKnob; 

    //vol Knob
    juce::Image volumeKnobImg = juce::ImageCache::getFromMemory(BinaryData::volumeKnobImg_png, BinaryData::volumeKnobImg_pngSize);
    FilmStripKnob volumeKnob;

    //lpf-hpf button
    juce::Image LPFImg;
    juce::DrawableImage LPFDraw;
    juce::Image HPFImg;
    juce::DrawableImage HPFDraw;
    juce::DrawableButton filterButt;

    //even-odd button
    juce::Image evenImg;
    juce::DrawableImage evenDraw;
    juce::Image oddImg;
    juce::DrawableImage oddDraw;
    juce::DrawableButton symButt;

    //on-off button
    bool isOn = true; //meglio usare parametro l'audio processor! da modificare dunque anche la funzione changeLines();
    juce::Image ledOnImg;
    juce::DrawableImage ledOnDraw;
    juce::Image ledOffImg;
    juce::DrawableImage ledOffDraw;
    juce::DrawableButton onoffButt;
    juce::Image Lines;
    
    //Distrortion graph
    juce::Rectangle <int> graphRegion = juce::Rectangle <int>(642, 160, 230, 100);
    juce::Colour bg = juce::Colours::white;
    std::vector<float> arange(float min, float max, float step);
    float sign(float a);
    std::vector<float> distortion_graph(std::vector<float> x_axis, float g, bool isLeft, float sym);
    float step;
    std::vector<float> x_axisL;
    std::vector<float> x_axisR;
    std::vector<float> y_axisL;
    std::vector<float> y_axisR;

    //Attachements
    juce::OwnedArray<juce::AudioProcessorValueTreeState::SliderAttachment> attachments;
    juce::AudioProcessorValueTreeState::ButtonAttachment onoffAttachment;
    juce::AudioProcessorValueTreeState::ButtonAttachment filterButtAttachment;
    juce::AudioProcessorValueTreeState::ButtonAttachment symAttachment;

    //Presets
    juce::Image loadImg;
    juce::Image saveImg;
    juce::Image deleteImg;
    juce::DrawableImage loadDraw;
    juce::DrawableImage saveDraw;
    juce::DrawableImage deleteDraw;
    juce::DrawableButton saveButt;
    juce::DrawableButton loadButt;
    juce::DrawableButton deleteButt;
    juce::TextEditor presetNameSave;
    juce::PopupMenu presetMenu;
    juce::File presetsFolder = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory).getChildFile("AKGigi").getChildFile("BlobDrivePresets");
    juce::String name;
    void sortMenu();


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BlobDriveAudioProcessorEditor)
};
