/**
 * @file ImGuiFontUtils.h
 * @brief Helpers for locating a Chinese-capable font for ImGui text rendering.
 */
#pragma once

#include <array>
#include <cstdlib>
#include <filesystem>

namespace ImGuiFontUtils
{
/**
 * @brief Returns candidate font file names ordered by preferred UI readability.
 * @return Ordered font file name list to probe in the Windows Fonts directory.
 */
inline const std::array<const char*, 6>& chineseFontFileNames()
{
    static const std::array<const char*, 6> kCandidates = {
      "msyh.ttc",
      "msyh.ttf",
      "msyhbd.ttc",
      "simhei.ttf",
      "simsun.ttc",
      "simsunb.ttf"};
    return kCandidates;
}

/**
 * @brief Returns the default Windows Fonts directory when discoverable.
 * @return Font directory path, or an empty path when the Windows root is unavailable.
 */
inline std::filesystem::path defaultWindowsFontDirectory()
{
    if (const char* windowsDir = std::getenv("WINDIR"))
    {
        return std::filesystem::path(windowsDir) / "Fonts";
    }

    return {};
}

/**
 * @brief Finds the first existing Chinese-capable font under a font directory.
 * @param fontDirectory Directory to search for candidate font files.
 * @return First matching font path, or an empty path when no candidate exists.
 */
inline std::filesystem::path findFirstAvailableChineseFont(const std::filesystem::path& fontDirectory)
{
    if (fontDirectory.empty())
    {
        return {};
    }

    for (const char* fileName : chineseFontFileNames())
    {
        const std::filesystem::path candidate = fontDirectory / fileName;
        if (std::filesystem::exists(candidate))
        {
            return candidate;
        }
    }

    return {};
}
} // namespace ImGuiFontUtils
