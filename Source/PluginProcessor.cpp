#include "PluginProcessor.h"
#include "PluginEditor.h"

namespace
{
class PrismVoice final : public juce::SynthesiserVoice
{
public:
    bool canPlaySound (juce::SynthesiserSound* sound) override
    {
        return dynamic_cast<juce::SynthesiserSound*> (sound) != nullptr;
    }

    void startNote (int midiNote, float velocity, juce::SynthesiserSound*, int) override
    {
        level = velocity * 0.18f;
        frequency = juce::MidiMessage::getMidiNoteInHertz (midiNote);
        phase = 0.0;
        phase2 = 0.0;
        env = 0.0f;
        releasing = false;
        attackStep = 1.0f / juce::jmax (1.0f, (float)(getSampleRate() * 0.012));
    }

    void stopNote (float, bool allowTailOff) override
    {
        if (allowTailOff)
            releasing = true;
        else
            clearCurrentNote();
    }

    void pitchWheelMoved (int) override {}
    void controllerMoved (int, int) override {}

    void renderNextBlock (juce::AudioBuffer<float>& output, int start, int count) override
    {
        if (! isVoiceActive() || getSampleRate() <= 0.0)
            return;

        auto* left = output.getWritePointer (0);
        auto* right = output.getNumChannels() > 1 ? output.getWritePointer (1) : nullptr;
        const double sr = getSampleRate();
        const float cutoff = *cutoffParam;
        const float tone = *toneParam;
        const float attack = juce::jmax (0.001f, *attackParam);
        const float release = juce::jmax (0.02f, *releaseParam);
        const float detune = *detuneParam;
        const float gain = *gainParam;
        const float ratio = 1.0f + detune * 0.012f;

        // One-pole low-pass for a warm, controllable tone.
        const float fc = juce::jlimit (80.0f, 18000.0f, cutoff * (0.45f + tone * 1.35f));
        const float a = 1.0f - std::exp (-2.0f * juce::MathConstants<float>::pi * fc / (float)sr);

        for (int i = 0; i < count; ++i)
        {
            if (! releasing)
                env = juce::jmin (1.0f, env + 1.0f / juce::jmax (1.0f, attack * (float)sr));
            else
            {
                env *= std::exp (-1.0f / (release * (float)sr));
                if (env < 0.0001f)
                {
                    clearCurrentNote();
                    break;
                }
            }

            const double inc1 = juce::MathConstants<double>::twoPi * frequency / sr;
            const double inc2 = juce::MathConstants<double>::twoPi * frequency * ratio / sr;
            const float saw = (float)(2.0 * (phase / juce::MathConstants<double>::twoPi) - 1.0);
            const float saw2 = (float)(2.0 * (phase2 / juce::MathConstants<double>::twoPi) - 1.0);
            const float pulse = phase < juce::MathConstants<double>::pi ? 1.0f : -1.0f;
            const float raw = (saw * (1.0f - tone * 0.45f) + saw2 * (tone * 0.25f) + pulse * 0.2f) * gain;
            filterState += a * (raw - filterState);
            const float sample = filterState * env * level;

            left[start + i] += sample;
            if (right != nullptr)
                right[start + i] += sample;

            phase += inc1;
            phase2 += inc2;
            if (phase >= juce::MathConstants<double>::twoPi) phase -= juce::MathConstants<double>::twoPi;
            if (phase2 >= juce::MathConstants<double>::twoPi) phase2 -= juce::MathConstants<double>::twoPi;
        }
    }

    void setParameters (std::atomic<float>* c, std::atomic<float>* t,
                        std::atomic<float>* a, std::atomic<float>* r,
                        std::atomic<float>* d, std::atomic<float>* g)
    {
        cutoffParam = c; toneParam = t; attackParam = a; releaseParam = r;
        detuneParam = d; gainParam = g;
    }

private:
    double phase = 0.0, phase2 = 0.0, frequency = 440.0;
    float level = 0.0f, env = 0.0f, filterState = 0.0f;
    float attackStep = 0.0f;
    bool releasing = false;
    std::atomic<float>* cutoffParam = nullptr;
    std::atomic<float>* toneParam = nullptr;
    std::atomic<float>* attackParam = nullptr;
    std::atomic<float>* releaseParam = nullptr;
    std::atomic<float>* detuneParam = nullptr;
    std::atomic<float>* gainParam = nullptr;
};

class PrismSound final : public juce::SynthesiserSound
{
public:
    bool appliesToNote (int) override { return true; }
    bool appliesToChannel (int) override { return true; }
};
}

PrismSynthAudioProcessor::APVTS::ParameterLayout PrismSynthAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> p;
    p.push_back (std::make_unique<juce::AudioParameterFloat>("cutoff", "Cutoff",
        juce::NormalisableRange<float>(200.0f, 16000.0f, 1.0f, 0.35f), 9000.0f));
    p.push_back (std::make_unique<juce::AudioParameterFloat>("tone", "Tone",
        juce::NormalisableRange<float>(0.0f, 1.0f), 0.45f));
    p.push_back (std::make_unique<juce::AudioParameterFloat>("attack", "Attack",
        juce::NormalisableRange<float>(0.001f, 2.0f, 0.001f, 0.4f), 0.012f));
    p.push_back (std::make_unique<juce::AudioParameterFloat>("release", "Release",
        juce::NormalisableRange<float>(0.02f, 4.0f, 0.001f, 0.4f), 0.35f));
    p.push_back (std::make_unique<juce::AudioParameterFloat>("detune", "Detune",
        juce::NormalisableRange<float>(0.0f, 1.0f), 0.22f));
    p.push_back (std::make_unique<juce::AudioParameterFloat>("gain", "Output",
        juce::NormalisableRange<float>(0.1f, 1.0f), 0.65f));
    return { p.begin(), p.end() };
}

PrismSynthAudioProcessor::PrismSynthAudioProcessor()
    : AudioProcessor (BusesProperties().withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      parameters (*this, nullptr, "PARAMETERS", createParameterLayout())
{
    for (int i = 0; i < 8; ++i)
    {
        auto* voice = new PrismVoice();
        voice->setParameters (parameters.getRawParameterValue("cutoff"),
                              parameters.getRawParameterValue("tone"),
                              parameters.getRawParameterValue("attack"),
                              parameters.getRawParameterValue("release"),
                              parameters.getRawParameterValue("detune"),
                              parameters.getRawParameterValue("gain"));
        synth.addVoice (voice);
    }
    synth.addSound (new PrismSound());
}

void PrismSynthAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    synth.setCurrentPlaybackSampleRate (sampleRate);
    juce::ignoreUnused (samplesPerBlock);
}

bool PrismSynthAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    return layouts.getMainOutputChannelSet() == juce::AudioChannelSet::mono()
        || layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo();
}

void PrismSynthAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi)
{
    juce::ScopedNoDenormals noDenormals;
    buffer.clear();
    synth.renderNextBlock (buffer, midi, 0, buffer.getNumSamples());
}

juce::AudioProcessorEditor* PrismSynthAudioProcessor::createEditor()
{
    return new PrismSynthAudioProcessorEditor (*this);
}

void PrismSynthAudioProcessor::getStateInformation (juce::MemoryBlock& dest)
{
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, dest);
}

void PrismSynthAudioProcessor::setStateInformation (const void* data, int size)
{
    std::unique_ptr<juce::XmlElement> xml (getXmlFromBinary (data, size));
    if (xml != nullptr && xml->hasTagName (parameters.state.getType()))
        parameters.replaceState (juce::ValueTree::fromXml (*xml));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PrismSynthAudioProcessor();
}
