#define BOOST_TEST_MODULE OcctDfmCapiTest
#include <boost/test/unit_test.hpp>

#include "api/OcctDfmCapi.h"

#include <BRepPrimAPI_MakeBox.hxx>
#include <IFSelect_ReturnStatus.hxx>
#include <STEPControl_Writer.hxx>

#include <filesystem>
#include <vector>

namespace
{
std::filesystem::path writeBoxStepFile(const std::filesystem::path& outputPath)
{
    std::filesystem::create_directories(outputPath.parent_path());

    STEPControl_Writer writer;
    const TopoDS_Shape box = BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape();
    writer.Transfer(box, STEPControl_AsIs);
    const IFSelect_ReturnStatus status = writer.Write(outputPath.string().c_str());
    BOOST_REQUIRE(status == IFSelect_RetDone);
    return outputPath;
}

std::wstring readWideStringFromApi(int length,
                                   int (*copyFn)(OcctDfmHandle, wchar_t*, int),
                                   OcctDfmHandle handle)
{
    std::vector<wchar_t> buffer(static_cast<size_t>(length) + 1U, L'\0');
    BOOST_REQUIRE(copyFn(handle, buffer.data(), static_cast<int>(buffer.size())) == 1);
    return std::wstring(buffer.data());
}
} // namespace

BOOST_AUTO_TEST_CASE(wide_c_api_loads_step_and_report_and_exports_json)
{
    const std::filesystem::path tempDir =
      std::filesystem::temp_directory_path() / "occt_dfm_capi_test";
    const std::filesystem::path stepPath = writeBoxStepFile(tempDir / "part.step");

    OcctDfmHandle handle = OcctDfm_CreateSession();
    BOOST_REQUIRE(handle != nullptr);

    BOOST_REQUIRE_EQUAL(OcctDfm_InitializeLoggingW(handle, (tempDir / "logs").c_str()), 1);
    BOOST_REQUIRE_EQUAL(OcctDfm_LoadTargetStepFileW(handle, stepPath.c_str()), 1);

    const wchar_t* reportJson =
      L"{\"face_highlight_map\":{},\"violations\":[{\"severity\":\"red\",\"message\":\"dfm-red\","
      L"\"suggestions\":[\"s1\",\"s2\"],\"affected_face_ids\":[\"1\"]}]}";
    BOOST_REQUIRE_EQUAL(OcctDfm_LoadDfmReportFromJsonW(handle, reportJson), 1);
    BOOST_CHECK_EQUAL(OcctDfm_HasDfmReport(handle), 1);
    BOOST_CHECK_EQUAL(OcctDfm_IsProcessable(handle), 0);
    BOOST_CHECK_EQUAL(OcctDfm_GetFaceSeverityW(handle, L"1"), OCCT_DFM_SEVERITY_RED);

    OcctDfmColor color{};
    BOOST_REQUIRE_EQUAL(OcctDfm_GetFaceColorW(handle, L"1", &color), 1);
    BOOST_TEST(color.red > color.green);
    BOOST_TEST(color.red > color.blue);

    const int jsonLength = OcctDfm_GetVisualizationJsonLengthW(handle);
    BOOST_TEST(jsonLength > 0);
    const std::wstring jsonText =
      readWideStringFromApi(jsonLength, OcctDfm_CopyVisualizationJsonW, handle);
    BOOST_CHECK(jsonText.find(L"\"face_id\":\"1\"") != std::wstring::npos);
    BOOST_CHECK(jsonText.find(L"dfm-red") != std::wstring::npos);

    OcctDfm_DestroySession(handle);
}

BOOST_AUTO_TEST_CASE(wide_c_api_reports_step_load_error)
{
    OcctDfmHandle handle = OcctDfm_CreateSession();
    BOOST_REQUIRE(handle != nullptr);

    BOOST_CHECK_EQUAL(
      OcctDfm_LoadTargetStepFileW(handle, L"Z:\\this\\path\\does\\not\\exist\\missing.step"),
      0);

    const int errorLength = OcctDfm_GetLastErrorLengthW(handle);
    BOOST_TEST(errorLength > 0);
    const std::wstring errorText =
      readWideStringFromApi(errorLength, OcctDfm_CopyLastErrorW, handle);
    BOOST_CHECK(errorText.find(L"failed to read STEP file") != std::wstring::npos);

    OcctDfm_DestroySession(handle);
}
