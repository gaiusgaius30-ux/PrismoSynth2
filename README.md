# PrismSynth — JUCE VST3 starter synth

PrismSynth is a small polyphonic subtractive-style software synthesizer project for Windows and Cubase. It includes a custom dark interface and six automatable parameters: Cutoff, Tone, Attack, Release, Detune, and Output.

## Requirements

- Windows 10/11, 64-bit
- Visual Studio 2022 with **Desktop development with C++**
- CMake 3.22 or newer
- JUCE 7 or JUCE 8 (download from https://juce.com/download/ or the official JUCE GitHub repository)
- Cubase 12 or newer recommended

## Build

1. Extract this project to a folder, for example `C:\AudioDev\PrismSynth`.
2. Download and extract JUCE to a stable path, for example `C:\JUCE`.
3. Open **x64 Native Tools Command Prompt for VS 2022**.
4. Run these commands (adjust paths if yours differ):

```bat
cd /d C:\AudioDev\PrismSynth
cmake -S . -B build -DJUCE_DIR=C:\JUCE -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

CMake should produce `PrismSynth.vst3` under the build tree and, because `COPY_PLUGIN_AFTER_BUILD` is enabled, attempt to copy it to the system VST3 directory. If it does not, find the bundle under `build\PrismSynth_artefacts\Release\VST3\PrismSynth.vst3` (exact intermediate path can vary by generator) and copy the whole `.vst3` folder to:

`C:\Program Files\Common Files\VST3\`

Do not copy only the files inside the `.vst3` bundle; copy the entire folder.

## Load in Cubase

1. Restart Cubase, or open Studio > VST Plug-in Manager and rescan.
2. Find **PrismSynth** in the instrument list.
3. Create an Instrument Track and select PrismSynth.
4. Play notes from a MIDI keyboard or draw MIDI notes in a part.
5. Turn Cutoff/Tone for timbre, Attack/Release for envelope feel, Detune for thickness, and Output for level.

## Notes and limitations

- This is an educational starter instrument, not a commercial-grade synth yet.
- The oscillator implementation is intentionally simple and may alias at high pitches; replace it with band-limited oscillators or JUCE DSP oscillator/wavetable code for production use.
- The sound uses a simple one-pole low-pass and basic amplitude envelope; it does not yet include presets, effects, modulation matrix, oversampling, or a preset browser.
- Keep JUCE's licence terms in mind if distributing the plugin. Review the current JUCE licensing page before publishing or selling a build.
- If compilation reports a JUCE API mismatch, use a current JUCE release and share the complete error output so the source can be adjusted.


## Easier option: build automatically in the cloud

The `.github/workflows/build-windows.yml` workflow lets GitHub build the Windows VST3 for you, so you do not need to install Visual Studio, CMake, or JUCE locally.

1. Sign in to GitHub and create a **new empty repository**.
2. Upload the contents of this project ZIP, including the hidden `.github` folder. (If the web upload hides dot-folders, use GitHub Desktop to upload the folder.)
3. Open the repository's **Actions** tab and select **Build PrismSynth VST3 (Windows)**.
4. Choose **Run workflow**. If the workflow already ran after upload, open its latest successful run.
5. At the bottom of the completed run, download the `PrismSynth-Windows-VST3` artifact ZIP.
6. Extract the artifact ZIP. Copy the complete `PrismSynth.vst3` folder into `C:\\Program Files\\Common Files\\VST3\\`, then restart/rescan Cubase.

This cloud build is provided as an automation convenience; it still needs a GitHub account and a successful build run. The generated artifact should be tested in Cubase before relying on it for projects or performances.
