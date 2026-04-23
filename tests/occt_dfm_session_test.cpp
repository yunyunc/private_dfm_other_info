#define BOOST_TEST_MODULE OcctDfmSessionTest
#include <boost/test/unit_test.hpp>

#include "api/OcctDfmSession.h"

#include <BRepPrimAPI_MakeBox.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <nlohmann/json.hpp>

namespace
{
using json = nlohmann::json;

std::string buildDfmJson(const std::string& faceId,
                         const std::string& severity,
                         const std::string& message)
{
    json report;
    report["face_highlight_map"] = json::object();
    report["violations"] = json::array(
      {{{"severity", severity},
        {"message", message},
        {"suggestions", json::array({"s1", "s2"})},
        {"affected_face_ids", json::array({faceId})}}});
    return report.dump();
}

TopoDS_Face firstFaceOf(const TopoDS_Shape& shape)
{
    for (TopExp_Explorer exp(shape, TopAbs_FACE); exp.More(); exp.Next())
    {
        return TopoDS::Face(exp.Current());
    }
    return TopoDS_Face();
}
} // namespace

BOOST_AUTO_TEST_CASE(load_dfm_json_and_query_face_data)
{
    OcctDfmSession session;
    const TopoDS_Shape box = BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape();
    BOOST_REQUIRE(session.loadTargetShape(box));

    const TopoDS_Face firstFace = firstFaceOf(box);
    BOOST_REQUIRE(!firstFace.IsNull());

    const std::string firstFaceId = session.getFaceId(firstFace);
    BOOST_REQUIRE_EQUAL(firstFaceId, "1");

    BOOST_REQUIRE(session.loadDfmReportFromJson(buildDfmJson(firstFaceId, "red", "dfm-red")));
    BOOST_TEST(session.hasDfmReport());
    BOOST_TEST(!session.isDfmProcessable());
    BOOST_CHECK_EQUAL(static_cast<int>(session.getFaceSeverity(firstFaceId)),
                      static_cast<int>(OcctDfmSession::Severity::Red));

    const auto violations = session.getViolationsForFace(firstFaceId);
    BOOST_REQUIRE_EQUAL(violations.size(), 1U);
    BOOST_CHECK_EQUAL(violations[0].message, "dfm-red");
    BOOST_REQUIRE_EQUAL(violations[0].suggestions.size(), 2U);
}

BOOST_AUTO_TEST_CASE(build_overlay_returns_colored_shape_for_loaded_report)
{
    OcctDfmSession session;
    const TopoDS_Shape box = BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape();
    BOOST_REQUIRE(session.loadTargetShape(box));
    BOOST_REQUIRE(session.loadDfmReportFromJson(buildDfmJson("1", "yellow", "dfm-yellow")));

    int coloredFaceCount = 0;
    const Handle(AIS_ColoredShape) overlay = session.buildOverlay(&coloredFaceCount);

    BOOST_TEST(!overlay.IsNull());
    BOOST_TEST(coloredFaceCount == 1);
}

BOOST_AUTO_TEST_CASE(processable_report_builds_green_overlay_without_explicit_face_colors)
{
    OcctDfmSession session;
    const TopoDS_Shape box = BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape();
    BOOST_REQUIRE(session.loadTargetShape(box));
    BOOST_REQUIRE(session.loadDfmReportFromJson("{\"face_highlight_map\":{},\"violations\":[]}"));

    int coloredFaceCount = -1;
    const Handle(AIS_ColoredShape) overlay = session.buildOverlay(&coloredFaceCount);

    BOOST_TEST(!overlay.IsNull());
    BOOST_TEST(session.isDfmProcessable());
    BOOST_TEST(coloredFaceCount == 0);
}
