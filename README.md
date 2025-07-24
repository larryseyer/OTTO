OTTO is a powerful drum machine and sample playback system with these key features:

- **8 Independent MIDI Players**: Each with (synced MIDI playback to host or stand alone), SFZ sample engine, unique sounds and midi & audio mixer with multiple outputs and FX for each.
- **Pattern Management**: 4x4 grids of MIDI Beat patterns organized in groups of 16
- **Smart Fill System**: Automatic and manual drum fills
- **Professional Audio**: SFZ 2.0 sample playback with effects for each drumkit element (kick, snare, hat , toms, cymbals, etc)
- **Cross-Platform**: Works on Mac, Windows, Linux, and IOS
- **Plugin Support**: VST3, AU, CLAP, Standalone, and IOS stand alone and AUV3 versions

### Technical Architecture

OTTO uses these core components:
**JUCE Framework** - Cross-platform audio application framework using Projucer and Xcode
**SFZ Engine** - Industry-standard sample playback
**MIDI Engine** - Pattern playback and timing
**Audio Engine** - Mixing, effects, and output
**High Quality convolution and audio processing FX
**User Interface** - Modern, responsive GUI
**Reaper INI style .ini plain text database

OTTO uses 8 MIDI Player Playback engines consisting of:
- **Note events**: Which drum to hit (kick, snare, hi-hat, etc.)
- **Timing**: When each hit occurs (measured in ticks or beats or samples)
- **Velocity**: How hard each hit should be (0-127) modified from 10% to 200% by the "Energy" slider (defaults to 100%)
- **Length**: Total pattern duration (no limit in length)
- **Each MIDI player triggers it's own SFZ Drumkit playback audio engine with mixer and FX
- **Each Player has its own drumkit selected by drop down menu or increment/decrement chevrons
- **Custom MIDI in and MIDI out mappings
- **AI Beat generation capable.

### Audio Processing Flow
1. **MIDI Input** → Pattern selection and timing
2. **Fills Logic** → When the midi fills patterns are to be inserted (every 2,4,8,16 bars at "x" bars from the end of the loop)
3. **SFZ Engine** → Converts MIDI to stereo audio using samples and SFZ mapping
4. **Mixer** → Combines and processes audio channels for each drumkit element with send and return for each
5. **Effects** → Adds reverb, delay, and other processing as send/return and insert for each drum element.
6. **Audio Output** → Sends final audio for each player to speakers/DAW using a multi-output method (from stereo to 32 channels per player engine)

### Preset System
OTTO Stores settings for the program in 'presets'
Each preset remembers current midi drum grove group, beat, fill pattern file, fill frequency, drumkit, energy, swing, volume, mixer and fx settings for each player

SFZ 2.0 (Simple File Format) is an industry-standard format for describing how to play audio samples. An SFZ file is a text file that defines:

- **Samples**: Which audio files to use (WAV, AIFF, etc.)
- **Mapping**: Which MIDI notes trigger which samples
- **Parameters**: Volume, pitch, filtering, envelopes
- **Conditions**: Velocity ranges, round-robin samples

Each player has control over:

1) Reset: bool
2) Transport: enum { Stop, Record, Play, Pause, Punch In, Punch Out, Import, Export, Ignore, Locate Zero, Locate 1, Locate 2, Locate 3, Locate 4 }
3) Recording Mode: enum { Locked=0, Replace, Overdub }
4) Record MIDI Channel: int [1..16]
5) Monitor: bool
6) Point 1–4: float [0..600, e.g. seconds]
7) Point 1–4 Action: enum (same as Transport)
8) Sync Type: enum { Off=0, Host, Trigger }
9) Quantize Amount %: int [0..100]
10) Swing Amount %: int [0..100]
11) Humanize Amount %: int [0..100]
12) Primary Quantize: enum { None, Whole Note, Dotted Half Note, Half Note, Dotted Quarter Note, Quarter Note, Eighth Note, Eighth Note Triplet, Sixteenth Note, Sixteenth Note Triplet, Thirty-Second Note, Thirty-Second Note Triplet, Sixty-Fourth Note, Sixty-Fourth Note Triplet }
13) Secondary Quantize: enum (same range)
14) Quantize Note Off: bool
15) Transpose: int [-24..24]
16) Velocity Shift %: int [-64..64]
17) Timing Shift (Samples): int [-10000..10000]
18) Import / Export: bool triggers
19) Export to Folder (ask for folder)
20) Show Graphics: bool
21) Drum Conversion MIDI Notes: int [0..127] for Kick, Snare, Hat, Ride, Crash, Cowbell in/out
22) Snare -> Stick: bool
23) Hat -> Ride: bool
24) Crash Next Bar, Mute Next Bar, etc.: bool triggers
25) Drum Conversion: bool
26) Sample Position: read-only int
27) Send Crash: bool
28) Power: bool
29) Current Drum Folder: string
30) Energy Level: enum { Asleep=0, Chilled=1, Relaxed=2, Normal=3, Aroused=4, Excited=5, Energetic=6 }
