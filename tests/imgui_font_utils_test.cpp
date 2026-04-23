#define BOOST_TEST_MODULE ImGuiFontUtilsTest
#include <boost/test/unit_test.hpp>

#include "view/ImGuiFontUtils.h"

#include <imgui.h>
#include <filesystem>
#include <fstream>

namespace
{
class ScopedTempDir
{
public:
    ScopedTempDir()
        : myPath(std::filesystem::temp_directory_path()
                 / std::filesystem::path("occtimgui_imgui_font_utils_test"))
    {
        std::filesystem::remove_all(myPath);
        std::filesystem::create_directories(myPath);
    }

    ~ScopedTempDir()
    {
        std::filesystem::remove_all(myPath);
    }

    const std::filesystem::path& path() const
    {
        return myPath;
    }

private:
    std::filesystem::path myPath;
};

void touchFile(const std::filesystem::path& filePath)
{
    std::ofstream output(filePath.string(), std::ios::out | std::ios::binary);
    output << "font";
}
} // namespace

BOOST_AUTO_TEST_CASE(chinese_font_candidates_include_expected_windows_fonts)
{
    const auto& candidates = ImGuiFontUtils::chineseFontFileNames();

    BOOST_TEST(candidates.size() == 6U);
    BOOST_TEST(std::string(candidates[0]) == "msyh.ttc");
    BOOST_TEST(std::string(candidates[3]) == "simhei.ttf");
    BOOST_TEST(std::string(candidates[5]) == "simsunb.ttf");
}

BOOST_AUTO_TEST_CASE(find_first_available_chinese_font_returns_empty_for_missing_directory)
{
    const std::filesystem::path fontPath =
      ImGuiFontUtils::findFirstAvailableChineseFont(std::filesystem::path());

    BOOST_TEST(fontPath.empty());
}

BOOST_AUTO_TEST_CASE(find_first_available_chinese_font_prefers_earlier_candidate)
{
    ScopedTempDir tempDir;
    touchFile(tempDir.path() / "simsun.ttc");
    touchFile(tempDir.path() / "simhei.ttf");

    const std::filesystem::path fontPath =
      ImGuiFontUtils::findFirstAvailableChineseFont(tempDir.path());

    BOOST_TEST(fontPath == tempDir.path() / "simhei.ttf");
}

BOOST_AUTO_TEST_CASE(find_first_available_chinese_font_returns_first_present_font)
{
    ScopedTempDir tempDir;
    touchFile(tempDir.path() / "msyh.ttf");

    const std::filesystem::path fontPath =
      ImGuiFontUtils::findFirstAvailableChineseFont(tempDir.path());

    BOOST_TEST(fontPath == tempDir.path() / "msyh.ttf");
}

BOOST_AUTO_TEST_CASE(selected_system_font_contains_dfm_message_glyphs)
{
    const std::filesystem::path fontPath = ImGuiFontUtils::findFirstAvailableChineseFont(
      ImGuiFontUtils::defaultWindowsFontDirectory());
    BOOST_REQUIRE(!fontPath.empty());

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();

    ImFont* font = io.Fonts->AddFontFromFileTTF(fontPath.string().c_str(),
                                                18.0f,
                                                nullptr,
                                                io.Fonts->GetGlyphRangesChineseFull());
    BOOST_REQUIRE(font != nullptr);
    BOOST_REQUIRE(io.Fonts->Build());

    const std::u32string sample = U"孔径2.5mm小于推荐阈值3mm";
    for (char32_t codepoint : sample)
    {
        if (codepoint == U'.')
        {
            continue;
        }

        BOOST_TEST_CONTEXT("font=" << fontPath.string()
                                   << ", codepoint=U+"
                                   << std::hex << static_cast<unsigned int>(codepoint))
        {
            BOOST_TEST(font->FindGlyphNoFallback(static_cast<ImWchar>(codepoint)) != nullptr);
        }
    }

    ImGui::DestroyContext();
}
