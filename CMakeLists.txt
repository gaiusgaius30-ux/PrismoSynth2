cmake_minimum_required(VERSION 3.22)
project(PrismSynth VERSION 1.0.0)

# Download JUCE separately, then configure with -DJUCE_DIR="C:/path/to/JUCE"
if(NOT DEFINED JUCE_DIR)
    message(FATAL_ERROR "Set JUCE_DIR to your JUCE folder, e.g. -DJUCE_DIR=C:/JUCE")
endif()
add_subdirectory("${JUCE_DIR}" JUCE)

juce_add_plugin(PrismSynth
    COMPANY_NAME "Studio Tools"
    BUNDLE_ID "com.studiotools.prismsynth"
    PLUGIN_MANUFACTURER_CODE StTo
    PLUGIN_CODE PrSm
    FORMATS VST3
    PRODUCT_NAME "PrismSynth"
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS TRUE
    COPY_PLUGIN_AFTER_BUILD TRUE
)

juce_generate_juce_header(PrismSynth)

target_sources(PrismSynth PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginProcessor.h
    Source/PluginEditor.cpp
    Source/PluginEditor.h
)

target_compile_features(PrismSynth PRIVATE cxx_std_17)
target_compile_definitions(PrismSynth PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries(PrismSynth PRIVATE
    juce::juce_audio_utils
    juce::juce_audio_processors
    juce::juce_dsp
    PUBLIC juce::juce_recommended_config_flags
           juce::juce_recommended_lto_flags
           juce::juce_recommended_warning_flags
)
