#pragma once

/**
 * @file INIAudio.h
 * @brief Audio-related constants for OTTO
 * 
 * This header contains all audio-related constants used throughout the OTTO application.
 * Split from INIConfig.h for better maintainability and faster compilation.
 * 
 * @author OTTO Development Team
 * @version 2.0
 * @date 2024
 */

namespace INIConfig {

   // ========================================================================
   // AUDIO NAMESPACE
   // ========================================================================
   namespace Audio {
       static const int NUM_DRUM_PADS = 16;
       static const int NUM_EQ_BANDS = 4;
       static const int NUM_SEND_TYPES = 2;
       static const int WAVEFORM_THUMBNAIL_CACHE_SIZE = 512;
       static const float DEFAULT_ATTACK_TIME = 0.01f;
       static const float DEFAULT_DECAY_TIME = 0.1f;
       static const float DEFAULT_SUSTAIN_LEVEL = 0.7f;
       static const float DEFAULT_RELEASE_TIME = 0.3f;
       static const float DEFAULT_START_POSITION = 0.0f;
       static const float DEFAULT_END_POSITION = 1.0f;
       static const float DEFAULT_CURVE_AMOUNT = 0.0f;
       static const float DEFAULT_PITCH_SHIFT = 0.0f;
       static const int DEFAULT_FILTER_TYPE = 0;
       static const float DEFAULT_FILTER_FREQUENCY = 1000.0f;
       static const float DEFAULT_PAN = 0.0f;
       static const bool DEFAULT_MUTE = false;
       static const bool DEFAULT_SOLO = false;
       static const float EQ_ATOMIC_INIT = 0.0f;
       static const float SEND_ATOMIC_INIT = 0.0f;
       static const int DEFAULT_SYNC_DIVISION = 4;
       static const int DEFAULT_SIDECHAIN_SOURCE = 0;
       static const int BIT_DEPTH_16 = 16;
       static const float DEFAULT_SAMPLERATE_REDUCTION = 1.0f;
       static const float DEFAULT_PRE_GAIN = 0.0f;
       static const float DEFAULT_POST_GAIN = 0.0f;
   } // namespace Audio

} // namespace INIConfig