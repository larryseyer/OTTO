# Create binary data target for OTTO assets
# This target is completely independent and generates BinaryData.h
# Asset names must match exactly what the code expects in BinaryData namespace
juce_add_binary_data(OTTOBinaryData
    HEADER_NAME BinaryData.h
    NAMESPACE BinaryData
    SOURCES
        # GUI Assets (all referenced in source code)
        ${CMAKE_CURRENT_SOURCE_DIR}/../Assets/GUI/Button100.png
        ${CMAKE_CURRENT_SOURCE_DIR}/../Assets/GUI/Button080.png
        ${CMAKE_CURRENT_SOURCE_DIR}/../Assets/GUI/SliderKnob.png
        ${CMAKE_CURRENT_SOURCE_DIR}/../Assets/GUI/Slider100.png
        ${CMAKE_CURRENT_SOURCE_DIR}/../Assets/GUI/PanKnob.png
        ${CMAKE_CURRENT_SOURCE_DIR}/../Assets/GUI/Pan100.png
        ${CMAKE_CURRENT_SOURCE_DIR}/../Assets/GUI/OTTO.icns
        ${CMAKE_CURRENT_SOURCE_DIR}/../Assets/GUI/OTTO_Button_Glyph.png
        ${CMAKE_CURRENT_SOURCE_DIR}/../Assets/GUI/NameBackground.png
        "${CMAKE_CURRENT_SOURCE_DIR}/../Assets/GUI/OTTO Splash Screen.png"
        
        # Font files that match expected BinaryData symbols
        # These names will be converted to C++ identifiers by JUCE
        ${CMAKE_CURRENT_SOURCE_DIR}/../Assets/Fonts/Orbitron/Orbitron-VariableFont_wght.ttf
        ${CMAKE_CURRENT_SOURCE_DIR}/../Assets/Fonts/Roboto/Roboto-VariableFont_wdth,wght.ttf
        ${CMAKE_CURRENT_SOURCE_DIR}/../Assets/Fonts/Roboto_Condensed/RobotoCondensed-VariableFont_wght.ttf
        ${CMAKE_CURRENT_SOURCE_DIR}/../Assets/Fonts/Open_Sans/OpenSans-VariableFont_wdth,wght.ttf
        ${CMAKE_CURRENT_SOURCE_DIR}/../Assets/Fonts/Playfair_Display/PlayfairDisplay-VariableFont_wght.ttf
        ${CMAKE_CURRENT_SOURCE_DIR}/../Assets/Fonts/Montserrat/Montserrat-VariableFont_wght.ttf
        
        # Phosphor Icon Fonts (required by FontManager)
        ${CMAKE_CURRENT_SOURCE_DIR}/../Assets/Fonts/Phosphor/thin/Phosphor-Thin.ttf
        ${CMAKE_CURRENT_SOURCE_DIR}/../Assets/Fonts/Phosphor/light/Phosphor-Light.ttf
        ${CMAKE_CURRENT_SOURCE_DIR}/../Assets/Fonts/Phosphor/regular/Phosphor.ttf
        ${CMAKE_CURRENT_SOURCE_DIR}/../Assets/Fonts/Phosphor/bold/Phosphor-Bold.ttf
        ${CMAKE_CURRENT_SOURCE_DIR}/../Assets/Fonts/Phosphor/fill/Phosphor-Fill.ttf
        ${CMAKE_CURRENT_SOURCE_DIR}/../Assets/Fonts/Phosphor/duotone/Phosphor-Duotone.ttf
)
