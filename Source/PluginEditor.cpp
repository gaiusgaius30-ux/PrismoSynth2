#include "PluginEditor.h"

namespace
{
const juce::Colour bg (0xff10131b);
const juce::Colour panel (0xff191e2a);
const juce::Colour accent (0xff79f2d0);
const juce::Colour text (0xffedf3ff);
}

PrismSynthAudioProcessorEditor::PrismSynthAudioProcessorEditor (PrismSynthAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    setSize (680, 430);
    title.setText ("PRISM", juce::dontSendNotification);
    title.setFont (juce::Font (juce::FontOptions (32.0f, juce::Font::bold)));
    title.setColour (juce::Label::textColourId, text);
    addAndMakeVisible (title);

    subtitle.setText ("POLYPHONIC SYNTHESIZER  /  VST3", juce::dontSendNotification);
    subtitle.setFont (juce::Font (juce::FontOptions (12.0f)));
    subtitle.setColour (juce::Label::textColourId, accent);
    addAndMakeVisible (subtitle);

    styleSlider (cutoff, cutoffLabel, "CUTOFF");
    styleSlider (tone, toneLabel, "TONE");
    styleSlider (attack, attackLabel, "ATTACK");
    styleSlider (release, releaseLabel, "RELEASE");
    styleSlider (detune, detuneLabel, "DETUNE");
    styleSlider (gain, gainLabel, "OUTPUT");

    cutoffAttachment = std::make_unique<SliderAttachment>(processor.parameters, "cutoff", cutoff);
    toneAttachment = std::make_unique<SliderAttachment>(processor.parameters, "tone", tone);
    attackAttachment = std::make_unique<SliderAttachment>(processor.parameters, "attack", attack);
    releaseAttachment = std::make_unique<SliderAttachment>(processor.parameters, "release", release);
    detuneAttachment = std::make_unique<SliderAttachment>(processor.parameters, "detune", detune);
    gainAttachment = std::make_unique<SliderAttachment>(processor.parameters, "gain", gain);
}

void PrismSynthAudioProcessorEditor::styleSlider (juce::Slider& s, juce::Label& l, const juce::String& name)
{
    s.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    s.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 82, 22);
    s.setColour (juce::Slider::rotarySliderFillColourId, accent);
    s.setColour (juce::Slider::rotarySliderOutlineColourId, juce::Colour (0xff343d50));
    s.setColour (juce::Slider::thumbColourId, text);
    s.setColour (juce::Slider::textBoxTextColourId, text);
    s.setColour (juce::Slider::textBoxBackgroundColourId, panel);
    s.setColour (juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    addAndMakeVisible (s);

    l.setText (name, juce::dontSendNotification);
    l.setJustificationType (juce::Justification::centred);
    l.setFont (juce::Font (juce::FontOptions (11.0f, juce::Font::bold)));
    l.setColour (juce::Label::textColourId, accent);
    addAndMakeVisible (l);
}

void PrismSynthAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (bg);
    g.setColour (panel);
    g.fillRoundedRectangle (juce::Rectangle<float>(20, 92, 640, 312), 16.0f);
    g.setColour (accent.withAlpha (0.18f));
    g.fillRoundedRectangle (juce::Rectangle<float>(20, 92, 640, 3), 1.5f);
    g.setColour (juce::Colour (0xff2b3446));
    g.drawRoundedRectangle (juce::Rectangle<float>(20, 92, 640, 312), 16.0f, 1.0f);
    g.setColour (accent);
    g.fillEllipse (620.0f, 32.0f, 8.0f, 8.0f);
}

void PrismSynthAudioProcessorEditor::resized()
{
    title.setBounds (26, 20, 240, 42);
    subtitle.setBounds (28, 60, 300, 22);

    juce::Slider* sliders[] = { &cutoff, &tone, &attack, &release, &detune, &gain };
    juce::Label* labels[] = { &cutoffLabel, &toneLabel, &attackLabel, &releaseLabel, &detuneLabel, &gainLabel };
    const int left = 30, top = 112, cellW = 205, cellH = 136, gapX = 0, gapY = 18;
    for (int i = 0; i < 6; ++i)
    {
        const int col = i % 3, row = i / 3;
        const int x = left + col * cellW;
        const int y = top + row * (cellH + gapY);
        labels[i]->setBounds (x, y, cellW - 4, 22);
        sliders[i]->setBounds (x + 24, y + 20, 150, 112);
    }
}
