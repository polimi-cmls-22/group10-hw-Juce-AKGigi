/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
BlobDriveAudioProcessorEditor::BlobDriveAudioProcessorEditor(BlobDriveAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p),
    //Custom Slider
    samplingKnob(samplingKnobImg, 4, false),
    filterKnob(filterKnobImg, 270, false),
    g1Knob(g1distKnobImg, 100, false),
    g2Knob(g2distKnobImg, 100, false),
    drywetKnob(drywetKnobImg, 100, false),
    highKnob(highKnobImg, 50, false),
    midhighKnob(midhighKnobImg, 50, false),
    midlowKnob(midlowKnobImg, 50, false),
    lowKnob(lowKnobImg, 50, false),
    volumeKnob(volumeKnobImg, 100, false),
    //Drawable Buttons
    filterButt("filterButt", juce::DrawableButton::ButtonStyle::ImageFitted),
    symButt("symButt", juce::DrawableButton::ButtonStyle::ImageFitted),
    onoffButt("onoffButt", juce::DrawableButton::ButtonStyle::ImageFitted),
    loadButt("loadButt", juce::DrawableButton::ButtonStyle::ImageFitted),
    saveButt("saveButt", juce::DrawableButton::ButtonStyle::ImageFitted),
    deleteButt("deleteButt", juce::DrawableButton::ButtonStyle::ImageFitted),
    //Drawable Button attachments
    symAttachment(p.get_Apvts(), "sym", symButt),
    onoffAttachment(p.get_Apvts(), "bypass", onoffButt),
    filterButtAttachment(p.get_Apvts(), "filter", filterButt)
{
    //Background Image
    BackGround = juce::ImageCache::getFromMemory(BinaryData::BackGround_png, BinaryData::BackGround_pngSize);

    //Sampling Knob
    addAndMakeVisible(samplingKnob);
    samplingKnob.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    samplingKnob.setRange(0.0, 3.0, 1.0);
    attachments.add(new juce::AudioProcessorValueTreeState::SliderAttachment(p.get_Apvts(), "samplFact", samplingKnob));
    samplingKnob.setMouseCursor(juce::MouseCursor::PointingHandCursor);
    samplingKnob.onDragStart = [this]() {
        samplingKnob.setMouseCursor(juce::MouseCursor::DraggingHandCursor);
    };
    samplingKnob.onDragEnd = [this]() {
        samplingKnob.setMouseCursor(juce::MouseCursor::PointingHandCursor);
    };

    //FilterKnob
    addAndMakeVisible(filterKnob);
    filterKnob.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    filterKnob.setRange(20, 20000); //da decidere insieme
    attachments.add(new juce::AudioProcessorValueTreeState::SliderAttachment(p.get_Apvts(), "freq", filterKnob));
    filterKnob.setMouseCursor(juce::MouseCursor::PointingHandCursor);
    filterKnob.onDragStart = [this]() {
        filterKnob.setMouseCursor(juce::MouseCursor::DraggingHandCursor);
    };
    filterKnob.onDragEnd = [this]() {
        filterKnob.setMouseCursor(juce::MouseCursor::PointingHandCursor);
    };

    //G1-G2 distKnob
    addAndMakeVisible(g1Knob);
    addAndMakeVisible(g2Knob);
    g1Knob.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    g2Knob.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    g1Knob.setRange(0.0, 10.0, 0.1);
    g2Knob.setRange(0.0, 10.0, 0.1);
    g1Knob.addListener(this);
    g2Knob.addListener(this);
    attachments.add(new juce::AudioProcessorValueTreeState::SliderAttachment(p.get_Apvts(), "g1", g1Knob));
    attachments.add(new juce::AudioProcessorValueTreeState::SliderAttachment(p.get_Apvts(), "g2", g2Knob));
    g1Knob.setMouseCursor(juce::MouseCursor::PointingHandCursor);
    g1Knob.onDragStart = [this]() {
        g1Knob.setMouseCursor(juce::MouseCursor::DraggingHandCursor);
    };
    g1Knob.onDragEnd = [this]() {
        g1Knob.setMouseCursor(juce::MouseCursor::PointingHandCursor);
    };
    g2Knob.setMouseCursor(juce::MouseCursor::PointingHandCursor);
    g2Knob.onDragStart = [this]() {
        g2Knob.setMouseCursor(juce::MouseCursor::DraggingHandCursor);
    };
    g2Knob.onDragEnd = [this]() {
        g2Knob.setMouseCursor(juce::MouseCursor::PointingHandCursor);
    };

    //drywetKnob
    addAndMakeVisible(drywetKnob);
    drywetKnob.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    drywetKnob.setRange(0.0, 1.0, 0.01);
    attachments.add(new juce::AudioProcessorValueTreeState::SliderAttachment(p.get_Apvts(), "dw", drywetKnob));
    drywetKnob.setMouseCursor(juce::MouseCursor::PointingHandCursor);
    drywetKnob.onDragStart = [this]() {
        drywetKnob.setMouseCursor(juce::MouseCursor::DraggingHandCursor);
    };
    drywetKnob.onDragEnd = [this]() {
        drywetKnob.setMouseCursor(juce::MouseCursor::PointingHandCursor);
    };

    //eq Knob
    addAndMakeVisible(highKnob);
    addAndMakeVisible(midhighKnob);
    addAndMakeVisible(midlowKnob);
    addAndMakeVisible(lowKnob);
    highKnob.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    midhighKnob.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    midlowKnob.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    lowKnob.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    highKnob.setRange(-12.0, 12.0, 0.1);
    midhighKnob.setRange(-12.0, 12.0, 0.1);
    midlowKnob.setRange(-12.0, 12.0, 0.1);
    lowKnob.setRange(-12.0, 12.0, 0.1);
    attachments.add(new juce::AudioProcessorValueTreeState::SliderAttachment(p.get_Apvts(), "low", lowKnob));
    attachments.add(new juce::AudioProcessorValueTreeState::SliderAttachment(p.get_Apvts(), "midlow", midlowKnob));
    attachments.add(new juce::AudioProcessorValueTreeState::SliderAttachment(p.get_Apvts(), "midhigh", midhighKnob));
    attachments.add(new juce::AudioProcessorValueTreeState::SliderAttachment(p.get_Apvts(), "high", highKnob));
    highKnob.setMouseCursor(juce::MouseCursor::PointingHandCursor);
    highKnob.onDragStart = [this]() {
        highKnob.setMouseCursor(juce::MouseCursor::DraggingHandCursor);
    };
    highKnob.onDragEnd = [this]() {
        highKnob.setMouseCursor(juce::MouseCursor::PointingHandCursor);
    };
    midhighKnob.setMouseCursor(juce::MouseCursor::PointingHandCursor);
    midhighKnob.onDragStart = [this]() {
        midhighKnob.setMouseCursor(juce::MouseCursor::DraggingHandCursor);
    };
    midhighKnob.onDragEnd = [this]() {
        midhighKnob.setMouseCursor(juce::MouseCursor::PointingHandCursor);
    };
    midlowKnob.setMouseCursor(juce::MouseCursor::PointingHandCursor);
    midlowKnob.onDragStart = [this]() {
        midlowKnob.setMouseCursor(juce::MouseCursor::DraggingHandCursor);
    };
    midlowKnob.onDragEnd = [this]() {
        midlowKnob.setMouseCursor(juce::MouseCursor::PointingHandCursor);
    };
    lowKnob.setMouseCursor(juce::MouseCursor::PointingHandCursor);
    lowKnob.onDragStart = [this]() {
        lowKnob.setMouseCursor(juce::MouseCursor::DraggingHandCursor);
    };
    lowKnob.onDragEnd = [this]() {
        lowKnob.setMouseCursor(juce::MouseCursor::PointingHandCursor);
    };


    //Volume knob
    addAndMakeVisible(volumeKnob);
    volumeKnob.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    volumeKnob.setRange(-60.0, 6.0, 0.1);
    attachments.add(new juce::AudioProcessorValueTreeState::SliderAttachment(p.get_Apvts(), "volume", volumeKnob));
    volumeKnob.setMouseCursor(juce::MouseCursor::PointingHandCursor);
    volumeKnob.onDragStart = [this]() {
        volumeKnob.setMouseCursor(juce::MouseCursor::DraggingHandCursor);
    };
    volumeKnob.onDragEnd = [this]() {
        volumeKnob.setMouseCursor(juce::MouseCursor::PointingHandCursor);
    };

    //filter button LPF HPF
    addAndMakeVisible(filterButt);
    LPFImg = juce::ImageCache::getFromMemory(BinaryData::LPFImg_png, BinaryData::LPFImg_pngSize);
    LPFDraw.setImage(LPFImg);
    HPFImg = juce::ImageCache::getFromMemory(BinaryData::HPFImg_png, BinaryData::HPFImg_pngSize);
    HPFDraw.setImage(HPFImg);
    filterButt.setImages(&LPFDraw, nullptr, nullptr, nullptr, &HPFDraw);
    filterButt.setClickingTogglesState(true);
    filterButt.setColour(juce::DrawableButton::backgroundOnColourId, juce::Colours::transparentWhite);
    filterButt.setMouseCursor(juce::MouseCursor::PointingHandCursor);

    //even-odd button
    addAndMakeVisible(symButt);
    evenImg = juce::ImageCache::getFromMemory(BinaryData::evenImg_png, BinaryData::evenImg_pngSize);
    evenDraw.setImage(evenImg);
    oddImg = juce::ImageCache::getFromMemory(BinaryData::oddImg_png, BinaryData::oddImg_pngSize);
    oddDraw.setImage(oddImg);
    symButt.setImages(&evenDraw, nullptr, nullptr, nullptr, &oddDraw);
    symButt.setClickingTogglesState(true);
    symButt.setColour(juce::DrawableButton::backgroundOnColourId, juce::Colours::transparentWhite);
    symButt.setMouseCursor(juce::MouseCursor::PointingHandCursor);
    symButt.onClick = [this]() {
        y_axisL = distortion_graph(x_axisL, g1Knob.getValue(), true, symButt.getToggleState());
        repaint(graphRegion);
    };

    //onoffButton
    addAndMakeVisible(onoffButt);
    ledOffImg = juce::ImageCache::getFromMemory(BinaryData::ledOffImg_png, BinaryData::ledOffImg_pngSize);
    ledOffDraw.setImage(ledOffImg);
    ledOnImg = juce::ImageCache::getFromMemory(BinaryData::ledOnImg_png, BinaryData::ledOnImg_pngSize);
    ledOnDraw.setImage(ledOnImg);
    onoffButt.setImages(&ledOnDraw, nullptr, nullptr, nullptr, &ledOffDraw);
    onoffButt.setClickingTogglesState(true);
    onoffButt.setColour(juce::DrawableButton::backgroundOnColourId, juce::Colours::transparentWhite);
    onoffButt.setMouseCursor(juce::MouseCursor::PointingHandCursor);
    onoffButt.onClick = [this]() { changeLines(); };

    //Dist Graph
    step = 0.1f;
    x_axisL = arange(-1.0f, 0.0f + step, step);
    x_axisR = arange(0.0f, 1.0f + step, step);
    y_axisL = distortion_graph(x_axisL, g1Knob.getValue(), true, symButt.getToggleState());
    y_axisR = distortion_graph(x_axisR, g2Knob.getValue(), false, symButt.getToggleState());


    //Presets Buttons
    addAndMakeVisible(loadButt);
    addAndMakeVisible(saveButt);
    addAndMakeVisible(deleteButt);
    addAndMakeVisible(presetNameSave);
    loadImg = juce::ImageCache::getFromMemory(BinaryData::LoadImg_png, BinaryData::LoadImg_pngSize);
    loadDraw.setImage(loadImg);
    saveImg = juce::ImageCache::getFromMemory(BinaryData::SaveImg_png, BinaryData::SaveImg_pngSize);
    saveDraw.setImage(saveImg);
    deleteImg = juce::ImageCache::getFromMemory(BinaryData::DeleteImg_png, BinaryData::DeleteImg_pngSize);
    deleteDraw.setImage(deleteImg);
    loadButt.setImages(&loadDraw);
    saveButt.setImages(&saveDraw);
    deleteButt.setImages(&deleteDraw);
    loadButt.setMouseCursor(juce::MouseCursor::PointingHandCursor);
    saveButt.setMouseCursor(juce::MouseCursor::PointingHandCursor);
    deleteButt.setMouseCursor(juce::MouseCursor::PointingHandCursor);
    presetNameSave.setJustification(juce::Justification::centred);
    presetNameSave.setColour(0x1000200, juce::Colours::white);
    presetNameSave.setColour(0x1000205, juce::Colours::transparentBlack);
    presetNameSave.setColour(0x1000206, juce::Colours::transparentBlack);
    presetNameSave.setColour(0x1000201, juce::Colours::black);
    presetNameSave.setInputRestrictions(12);
    presetNameSave.focusLost(juce::Component::focusChangedByMouseClick);
    sortMenu();
    loadButt.onClick = [this] {
        presetMenu.showMenuAsync(juce::PopupMenu::Options{}.withTargetComponent(loadButt),
            [this](int result)
            {
                if (result != 0) {
                    audioProcessor.loadPreset(presetsFolder.findChildFiles(2, false)[result - 1].getFileName());
                }   
            });
    };
    saveButt.onClick = [this] {
        if (presetNameSave.getText() != "") {
            name = presetNameSave.getText();
            audioProcessor.savePreset(name);
            sortMenu();
            presetNameSave.clear();
            presetNameSave.repaint();
        }  
    };
    deleteButt.onClick = [this] {
        presetMenu.showMenuAsync(juce::PopupMenu::Options{}.withTargetComponent(deleteButt),
            [this](int result)
            {
                if (result != 0) {
                    audioProcessor.deletePreset(presetsFolder.findChildFiles(2, false)[result - 1].getFileName());
                }
                sortMenu();
            });
    };


    //Window Parameters
    setSize (942, 629);
    setResizable(false, false);
}

BlobDriveAudioProcessorEditor::~BlobDriveAudioProcessorEditor()
{
    attachments.clear();
    //deleteAllChildren();
}

//==============================================================================
void BlobDriveAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
    juce::Rectangle<float> windowArea(0, 0, 942.0, 629.0);
    g.drawImage(BackGround, windowArea);

    //Lines On/Off
    linesArea = juce::Rectangle<float>(146, 128, 637, 377);
    if (*audioProcessor.get_Apvts().getRawParameterValue("bypass")) {
        Lines = juce::ImageCache::getFromMemory(BinaryData::linesOffImg_png, BinaryData::linesOffImg_pngSize);
    }
    else {
        Lines = juce::ImageCache::getFromMemory(BinaryData::linesOnImg_png, BinaryData::linesOnImg_pngSize);
    }
    
    g.drawImage(Lines, linesArea);

    //Distortion Graph
    FunctionGraph graph = FunctionGraph(graphRegion, bg);
    PointsVectorToDraw dataL = PointsVectorToDraw(juce::Colours::red);
    PointsVectorToDraw dataR = PointsVectorToDraw(juce::Colours::red);
    for (int i = 0; i < x_axisL.size(); i++) {
        dataL.push(Point(x_axisL[i], y_axisL[i]));
        dataR.push(Point(x_axisR[i], y_axisR[i]));
    }
    graph.push(dataL);
    graph.push(dataR);
    graph.paint(g);

}

void BlobDriveAudioProcessorEditor::resized()
{
    //knob
    samplingKnob.setBounds(429,506,94, 94);
    filterKnob.setBounds(698, 506, 80, 80);
    g1Knob.setBounds(622, 28, 68, 68);
    g2Knob.setBounds(723.5, 28, 68, 68);
    drywetKnob.setBounds(822, 28, 68, 68);
    highKnob.setBounds(470.5, 27, 68, 68);
    midhighKnob.setBounds(470.5, 123, 68, 68);
    midlowKnob.setBounds(470.5, 219, 68, 68);
    lowKnob.setBounds(470.5, 317, 68, 68);
    volumeKnob.setBounds(284, 161, 68, 68);

    //filterButt
    filterButt.setBounds(833, 495, 31, 54);

    //symButt
    symButt.setBounds(730, 113, 54, 31);

    //onoff Button
    onoffButt.setBounds(128, 292, 48,48);

    //presets buttons
    saveButt.setBounds(250, 72, 63, 39);
    loadButt.setBounds(315, 70, 66, 43);
    deleteButt.setBounds(338, 31, 42, 42);
    presetNameSave.setBounds(257, 37, 74, 29);
}

void BlobDriveAudioProcessorEditor::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &g1Knob) {
        y_axisL = distortion_graph(x_axisL, g1Knob.getValue(), true, symButt.getToggleState());
    }
    else if (slider == &g2Knob) {
        y_axisR = distortion_graph(x_axisR, g2Knob.getValue(), false, symButt.getToggleState());
    }
    repaint(graphRegion);
}

///////////////////////////////////////
/// Background Lines change function
///////////////////////////////////////

void BlobDriveAudioProcessorEditor::changeLines()
{
    repaint();
}

/////////////////////////////
//Distortion Functions
/////////////////////////////

//sign Function
float BlobDriveAudioProcessorEditor::sign(float a)
{
    if (a < 0)
        return -1;
    else if (a > 0)
        return 1;
    else
        return 0;
}

//arange Function
std::vector<float> BlobDriveAudioProcessorEditor::arange(float min, float max, float step)
{
    std::vector<float> values;
    for (float value = min; value < max; value += step)
        values.push_back(value);
    return values;
}

std::vector<float> BlobDriveAudioProcessorEditor::distortion_graph(std::vector<float> x_axis, float g, bool isLeft, float sym)
{
    std::vector<float> y_axis;
    if (isLeft) {
        for (int i = 0; i < x_axis.size(); i++) {
            float value = sign(pow(x_axis[i], sym)) * (1 - exp(-g * abs(x_axis[i])));
            y_axis.push_back(value);
        }
    }
    else {
        for (int i = 0; i < x_axis.size(); i++) {
            float value = (1 - exp(-g * abs(x_axis[i])));
            y_axis.push_back(value);
        }
    }
    return y_axis;
}

void BlobDriveAudioProcessorEditor::sortMenu()
{
    presetMenu.clear();
    for (int i = 0; i < presetsFolder.getNumberOfChildFiles(2); i++) {
        name = presetsFolder.findChildFiles(2, false)[i].getFileName();
        presetMenu.addItem(i + 1, name);
    }
}
