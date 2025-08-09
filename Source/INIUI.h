#pragma once

/**
 * @file INIUI.h
 * @brief UI-related constants for OTTO
 * 
 * This header contains all UI-related constants used throughout the OTTO application.
 * Split from INIConfig.h for better maintainability and faster compilation.
 * 
 * @author OTTO Development Team
 * @version 2.0
 * @date 2024
 */

namespace INIConfig {

   // ========================================================================
   // UI NAMESPACE
   // ========================================================================
   namespace UI {
       static const float SPLASH_SCREEN_ON_TIME = 2.0f;
       static const int DARK_THEME_ID = 1;
       static const int MAX_TOGGLE_STATES = 16;
       static const int MAX_FILL_STATES = 16;
       static const int MAX_PHOSPHOR_TYPEFACES = 6;
   } // namespace UI

} // namespace INIConfig