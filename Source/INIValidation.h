#pragma once

#include <JuceHeader.h>

/**
 * @file INIValidation.h
 * @brief Validation constants and functions for OTTO
 * 
 * This header contains all validation-related constants and functions used throughout the OTTO application.
 * Split from INIConfig.h for better maintainability and faster compilation.
 * 
 * @author OTTO Development Team
 * @version 2.0
 * @date 2024
 */

namespace INIConfig {

   namespace Validation {
       static const int MIN_THEME_ID = 1;
       static const int MAX_THEME_ID = 99;
       static const int MIN_SETTINGS_ID = 1;
       static const int MAX_SETTINGS_ID = 999;
       static const int MIN_PRESET_ID = 0;
       static const int MAX_PRESET_ID = 999;
       static const int MIN_AUDIO_SAMPLE_RATE = 44100;
       static const int MAX_AUDIO_SAMPLE_RATE = 192000;
       static const int MIN_BUFFER_SIZE = 32;
       static const int MAX_BUFFER_SIZE = 2048;
       static const float MIN_TEMPO = 30.0f;
       static const float MAX_TEMPO = 300.0f;
       static const float MIN_SWING = 0.0f;
       static const float MAX_SWING = 100.0f;
       static const float MIN_ENERGY = 0.0f;
       static const float MAX_ENERGY = 100.0f;

       static const int MIN_MIDI_NOTE = 0;
       static const int MAX_MIDI_NOTE = 127;
       static const int MIN_MIDI_CHANNEL = 1;
       static const int MAX_MIDI_CHANNEL = 16;
       static const int MIN_MIDI_VELOCITY = 0;
       static const int MAX_MIDI_VELOCITY = 127;
       static const int MIN_PATTERN_LENGTH = 1;
       static const int MAX_PATTERN_LENGTH = 64;
       static const int MIN_PATTERN_STEPS = 1;
       static const int MAX_PATTERN_STEPS = 64;
       static const float MIN_LOOP_POSITION = 0.0f;
       static const float MAX_LOOP_POSITION = 100.0f;
       static const float MIN_INTERFACE_SCALE = 0.75f;
       static const float MAX_INTERFACE_SCALE = 3.0f;
       static const float MIN_VOLUME = 0.0f;
       static const float MAX_VOLUME = 1.0f;
       static const int MIN_QUANTIZE_VALUE = 1;
       static const int MAX_QUANTIZE_VALUE = 32;
       static const int MIN_COUNT_IN_BARS = 0;
       static const int MAX_COUNT_IN_BARS = 8;
       static const int MIN_AUTO_SAVE_INTERVAL = 0;
       static const int MAX_AUTO_SAVE_INTERVAL = 3600;
       static const int MIN_PHOSPHOR_WEIGHT = 0;
       static const int MAX_PHOSPHOR_WEIGHT = 5;
       static const int MAX_THEME_NAME_LENGTH = 50;
       static const int MAX_BUTTON_INDEX = 15;
       static const float MIN_PAN = -1.0f;
       static const float MAX_PAN = 1.0f;
       static const int MIN_AUDIO_CHANNELS = 1;
       static const int MAX_AUDIO_CHANNELS = 32;
       static const int MIN_MIDI_DEVICE_ID = 0;
       static const int MAX_MIDI_DEVICE_ID = 127;
       static const int MIN_PLAYER_INDEX = 0;
       static const int MAX_PLAYER_INDEX = 7;
   } // namespace Validation

   // ========================================================================
   // GENERAL CONSTANTS
   // ========================================================================
   static const int MIN_ARRAY_INDEX = 0;

   // ========================================================================
   // VALIDATION FUNCTIONS
   // ========================================================================

   /** @brief Validate settings ID range */
   inline bool isValidSettingsID(int id) {
       return id >= Validation::MIN_SETTINGS_ID && id <= Validation::MAX_SETTINGS_ID;
   }

   /** @brief Validate settings ID string (JUCE 8 compatible) */
   inline bool isValidSettingsID(const juce::String& id) {
       if (id.isEmpty()) return false;
       int intId = id.getIntValue();
       return intId >= Validation::MIN_SETTINGS_ID && intId <= Validation::MAX_SETTINGS_ID;
   }

   /** @brief Validate tempo range */
   inline bool isValidTempo(float tempo) {
       return tempo >= Validation::MIN_TEMPO && tempo <= Validation::MAX_TEMPO;
   }

   /** @brief Validate interface scale range */
   inline bool isValidInterfaceScale(float scale) {
       return scale >= Validation::MIN_INTERFACE_SCALE && scale <= Validation::MAX_INTERFACE_SCALE;
   }

   /** @brief Validate volume range */
   inline bool isValidVolume(float volume) {
       return volume >= Validation::MIN_VOLUME && volume <= Validation::MAX_VOLUME;
   }

   /** @brief Validate quantize value range */
   inline bool isValidQuantizeValue(int value) {
       return value >= Validation::MIN_QUANTIZE_VALUE && value <= Validation::MAX_QUANTIZE_VALUE;
   }

   /** @brief Validate hex color string */
   inline bool isValidHexColor(const juce::String& color) {
       if (color.isEmpty()) return false;
       if (!color.startsWith("#")) return false;
       if (color.length() != 7) return false;

       for (int i = 1; i < color.length(); ++i) {
           juce::juce_wchar c = color[i];
           if (!((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f'))) {
               return false;
           }
       }
       return true;
   }

   /** @brief Validate sample rate */
   inline bool isValidSampleRate(int sampleRate) {
       return sampleRate >= Validation::MIN_AUDIO_SAMPLE_RATE && sampleRate <= Validation::MAX_AUDIO_SAMPLE_RATE;
   }

   /** @brief Validate buffer size */
   inline bool isValidBufferSize(int bufferSize) {
       return bufferSize >= Validation::MIN_BUFFER_SIZE && bufferSize <= Validation::MAX_BUFFER_SIZE;
   }

   /** @brief Validate MIDI device ID */
   inline bool isValidMidiDeviceID(int deviceID) {
       return deviceID >= Validation::MIN_MIDI_DEVICE_ID && deviceID <= Validation::MAX_MIDI_DEVICE_ID;
   }

   /** @brief Validate player index */
   inline bool isValidPlayerIndex(int index) {
       return index >= Validation::MIN_PLAYER_INDEX && index <= Validation::MAX_PLAYER_INDEX;
   }

   /** @brief Validate phosphor weight */
   inline int clampPhosphorWeight(int weight) {
       if (weight < Validation::MIN_PHOSPHOR_WEIGHT) return Validation::MIN_PHOSPHOR_WEIGHT;
       if (weight > Validation::MAX_PHOSPHOR_WEIGHT) return Validation::MAX_PHOSPHOR_WEIGHT;
       return weight;
   }

   /** @brief Validate pan value */
   inline bool isValidPan(float pan) {
       return pan >= Validation::MIN_PAN && pan <= Validation::MAX_PAN;
   }

   /** @brief Validate MIDI channel */
   inline bool isValidMidiChannel(int channel) {
       return channel >= Validation::MIN_MIDI_CHANNEL && channel <= Validation::MAX_MIDI_CHANNEL;
   }

   /** @brief Validate button index */
   inline bool isValidButtonIndex(int index) {
       return index >= 0 && index <= Validation::MAX_BUTTON_INDEX;
   }

   /** @brief Validate swing value */
   inline bool isValidSwing(float swing) {
       return swing >= Validation::MIN_SWING && swing <= Validation::MAX_SWING;
   }

   /** @brief Validate energy value */
   inline bool isValidEnergy(float energy) {
       return energy >= Validation::MIN_ENERGY && energy <= Validation::MAX_ENERGY;
   }

   /** @brief Clamp scale value to valid range */
   inline float clampScale(float scale) {
       if (scale < Validation::MIN_INTERFACE_SCALE) return Validation::MIN_INTERFACE_SCALE;
       if (scale > Validation::MAX_INTERFACE_SCALE) return Validation::MAX_INTERFACE_SCALE;
       return scale;
   }

   /** @brief Clamp tempo value to valid range */
   inline int clampTempo(int tempo) {
       if (tempo < static_cast<int>(Validation::MIN_TEMPO)) return static_cast<int>(Validation::MIN_TEMPO);
       if (tempo > static_cast<int>(Validation::MAX_TEMPO)) return static_cast<int>(Validation::MAX_TEMPO);
       return tempo;
   }

   /** @brief Clamp swing value to valid range */
   inline float clampSwing(float swing) {
       if (swing < Validation::MIN_SWING) return Validation::MIN_SWING;
       if (swing > Validation::MAX_SWING) return Validation::MAX_SWING;
       return swing;
   }

   /** @brief Clamp energy value to valid range */
   inline float clampEnergy(float energy) {
       if (energy < Validation::MIN_ENERGY) return Validation::MIN_ENERGY;
       if (energy > Validation::MAX_ENERGY) return Validation::MAX_ENERGY;
       return energy;
   }

   /** @brief Clamp volume value to valid range */
   inline float clampVolume(float volume) {
       if (volume < Validation::MIN_VOLUME) return Validation::MIN_VOLUME;
       if (volume > Validation::MAX_VOLUME) return Validation::MAX_VOLUME;
       return volume;
   }

   /** @brief Clamp pan value to valid range */
   inline float clampPan(float pan) {
       if (pan < Validation::MIN_PAN) return Validation::MIN_PAN;
       if (pan > Validation::MAX_PAN) return Validation::MAX_PAN;
       return pan;
   }

   /** @brief Clamp preset index to valid range */
   inline int clampPresetIndex(int preset) {
       if (preset < Validation::MIN_PRESET_ID) return Validation::MIN_PRESET_ID;
       if (preset > Validation::MAX_PRESET_ID) return Validation::MAX_PRESET_ID;
       return preset;
   }

   /** @brief Clamp window size to valid range */
   inline int clampWindowSize(int size) {
       if (size < 400) return 400;  // Minimum reasonable window size
       if (size > 3840) return 3840;  // Maximum reasonable window size (4K)
       return size;
   }

   /** @brief Get OTTO data directory */
   inline juce::File getOTTODataDirectory() {
       return juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory)
           .getChildFile("OTTO");
   }

   /** @brief Get themes directory */
   inline juce::File getThemesDirectory() {
       return getOTTODataDirectory().getChildFile("Themes");
   }

   // ========================================================================
   // COLOR NAMESPACE
   // ========================================================================
   namespace Color {
       static const uint32_t DEFAULT_ERROR_COLOR = 0xFFFF4444;
       static const uint32_t DEFAULT_SUCCESS_COLOR = 0xFF44FF44;
       static const uint32_t DEFAULT_METER_LOW_COLOR = 0xFF44FF44;
       static const uint32_t DEFAULT_METER_MID_COLOR = 0xFFFFFF44;
       static const uint32_t DEFAULT_METER_HIGH_COLOR = 0xFFFF4444;
       static const uint32_t DEFAULT_WARNING_COLOR = 0xFFFFAA44;
   } // namespace Color

} // namespace INIConfig