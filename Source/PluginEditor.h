#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"

class PrismSynthAudioProcessorEditor final : public juce::AudioProcessorEditor
{
public:
    explicit PrismSynthAudioProcessorEditor (PrismSynthAudioProcessor&);
    ~PrismSynthAudioProcessorEditor() override = default;
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    PrismSynthAudioProcessor& processor;
    juce::Label title, subtitle;
    juce::Slider cutoff, tone, attack, release, detune, gain;
    juce::Label cutoffLabel, toneLabel, attackLabel, releaseLabel, detuneLabel, gainLabel;
    std::unique_ptr<SliderAttachment> cutoffAttachment, toneAttachment, attackAttachment,
                                     releaseAttachment, detuneAttachment, gainAttachment;
    void styleSlider (juce::Slider&, juce::Label&, const juce::String&);
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PrismSynthAudioProcessorEditor)
};
