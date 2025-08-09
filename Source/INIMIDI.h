#pragma once

/**
 * @file INIMIDI.h
 * @brief MIDI-related constants for OTTO
 * 
 * This header contains all MIDI-related constants used throughout the OTTO application.
 * Split from INIConfig.h for better maintainability and faster compilation.
 * 
 * @author OTTO Development Team
 * @version 2.0
 * @date 2024
 */

namespace INIConfig {

   // ========================================================================
   // MIDI NAMESPACE
   // ========================================================================
   namespace MIDI {
       static const int STRING_LENGTH_SUBTRACT_ONE = 1;
       static const int INACTIVE_PATTERN = -1;
       static const int INACTIVE_SCENE = -1;
       static const float VELOCITY_DIVISOR = 127.0f;
       static const int DEFAULT_CC_NUMBER = 1;
       static const double DEFAULT_LAST_TIME = -1.0;
       static const double DEFAULT_POSITION = 0.0;
       static const float DEFAULT_VELOCITY_RANGE = 0.2f;
       static const float DEFAULT_VELOCITY_VARIATION = 0.1f;
       static const float DEFAULT_TIMING_DEVIATION = 0.05f;
       static const float DEFAULT_NOTE_DENSITY = 0.7f;
       static const int DEFAULT_FEEDBACK_TYPE = 0;
       static const int DEFAULT_SYSEX_DEVICE_ID = 0;
       static const int ALL_PLAYERS = -1;
   } // namespace MIDI

} // namespace INIConfig