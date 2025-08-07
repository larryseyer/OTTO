#pragma once
#include <JuceHeader.h>
#include "INIConfig.h"

class INIUtils {
public:
    static bool writeINIFile(const juce::File& file, const juce::StringArray& sections,
                            const std::map<juce::String, juce::StringArray>& sectionData) {
        if (!file.getParentDirectory().exists()) {
            auto result = file.getParentDirectory().createDirectory();
            if (!result.wasOk()) {
                return false;
            }
        }

        juce::StringArray lines;

        for (const auto& section : sections) {
            lines.add("[" + section + "]");

            auto it = sectionData.find(section);
            if (it != sectionData.end()) {
                for (const auto& line : it->second) {
                    lines.add(line);
                }
            }
            lines.add("");
        }

        return file.replaceWithText(lines.joinIntoString("\n"), false, false, "\n");
    }

    static bool readINIFile(const juce::File& file,
                           std::map<juce::String, std::map<juce::String, juce::String>>& data) {
        if (!file.existsAsFile()) {
            return false;
        }

        data.clear();

        juce::StringArray lines;
        file.readLines(lines);

        juce::String currentSection;

        for (const auto& line : lines) {
            juce::String trimmedLine = line.trim();

            if (trimmedLine.isEmpty() || trimmedLine.startsWith("#") || trimmedLine.startsWith(";")) {
                continue;
            }

            if (trimmedLine.startsWith("[") && trimmedLine.endsWith("]")) {
                currentSection = trimmedLine.substring(INIConfig::MIDI::STRING_LENGTH_SUBTRACT_ONE, trimmedLine.length() - INIConfig::MIDI::STRING_LENGTH_SUBTRACT_ONE);
                continue;
            }

            int equalsPos = trimmedLine.indexOf("=");
            if (equalsPos > INIConfig::Validation::MIN_ARRAY_INDEX && currentSection.isNotEmpty()) {
                juce::String key = trimmedLine.substring(INIConfig::Defaults::ZERO_VALUE, equalsPos).trim();
                juce::String value = trimmedLine.substring(equalsPos + INIConfig::Defaults::ONE_VALUE).trim();
                data[currentSection][key] = value;
            }
        }

        return true;
    }

    static juce::String escapeINIValue(const juce::String& value) {
        return value.replaceCharacter('\n', ' ').replaceCharacter('\r', ' ');
    }

    static juce::String formatTimestamp() {
        return juce::Time::getCurrentTime().formatted("%Y-%m-%d-%H-%M-%S");
    }

    static juce::String boolToString(bool value) {
        return value ? "1" : "0";
    }

    static bool stringToBool(const juce::String& str) {
        return str.getIntValue() != INIConfig::Defaults::ZERO_VALUE;
    }

    static juce::String floatToString(float value, int precision = static_cast<int>(INIConfig::Defaults::SCURVE_FACTOR)) {
        return juce::String(value, precision);
    }

    static float stringToFloat(const juce::String& str, float defaultValue = INIConfig::Validation::MIN_VOLUME) {
        float result = str.getFloatValue();
        return std::isfinite(result) ? result : defaultValue;
    }

    static int stringToInt(const juce::String& str, int defaultValue = INIConfig::Defaults::ZERO_VALUE) {
        return str.isEmpty() ? defaultValue : str.getIntValue();
    }

    static juce::String createComment(const juce::String& comment) {
        return "# " + comment;
    }

    static bool isComment(const juce::String& line) {
        return line.trim().startsWith("#") || line.trim().startsWith(";");
    }

    static bool isEmpty(const juce::String& line) {
        return line.trim().isEmpty();
    }

    static bool createBackup(const juce::File& originalFile) {
        if (!originalFile.existsAsFile()) {
            return false;
        }

        juce::String timestamp = formatTimestamp();
        juce::File backupFile = originalFile.getParentDirectory().getChildFile(
            originalFile.getFileNameWithoutExtension() + "_backup_" + timestamp + ".ini");

        return originalFile.copyFileTo(backupFile);
    }

    static bool validateINIFile(const juce::File& file) {
        std::map<juce::String, std::map<juce::String, juce::String>> data;
        return readINIFile(file, data) && !data.empty();
    }

private:
    JUCE_DECLARE_NON_COPYABLE(INIUtils)
};
