#define BOOST_TEST_MODULE FeatureRecognitionModelTests
#include <boost/test/unit_test.hpp>

#include "api/DfmOverlayBuilder.h"
#include "model/FeatureRecognitionModel.h"

#include <AIS_ColoredDrawer.hxx>
#include <Prs3d_ShadingAspect.hxx>
#include <nlohmann/json.hpp>
#include <BRepPrimAPI_MakeBox.hxx>

#include <cmath>
#include <string>
#include <utility>
#include <vector>

namespace
{
using json = nlohmann::json;

constexpr double kColorTol = 1e-6;

std::string buildFeatureRecognitionJson()
{
    json root;
    json part;
    json featureRec;
    json groups = json::array();

    json holes;
    holes["name"] = "Through Hole(s)";
    holes["color"] = "(10, 20, 30)";
    holes["totalGroupFeatureCount"] = 2;
    holes["featureCount"] = 2;
    holes["features"] = json::array({
      {{"shapeIDCount", 1}, {"shapeIDs", json::array({{{"id", "1"}}})}},
      {{"shapeIDCount", 1}, {"shapeIDs", json::array({{{"id", "2"}}})}}
    });
    groups.push_back(holes);

    json pockets;
    pockets["name"] = "Closed Pocket(s)";
    pockets["color"] = "(100, 110, 120)";
    pockets["totalGroupFeatureCount"] = 1;
    pockets["subGroupCount"] = 1;
    pockets["subGroups"] = json::array({
      {
        {"parametersCount", 1},
        {"parameters", json::array({{{"name", "Depth"}, {"units", "mm"}, {"value", "5"}}})},
        {"featureCount", 1},
        {"features",
         json::array({{{"shapeIDCount", 2},
                       {"shapeIDs", json::array({{{"id", "3"}}, {{"id", "4"}}})}}})}
      }
    });
    groups.push_back(pockets);

    featureRec["featureGroups"] = groups;
    part["featureRecognition"] = featureRec;
    root["parts"] = json::array({part});
    return root.dump();
}

std::string buildDfmReport(const std::vector<std::string>& redFaces,
                           const std::vector<std::string>& yellowFaces,
                           const std::vector<std::pair<std::string, std::vector<std::string>>>& violations = {})
{
    json report;
    report["report_version"] = "2.0";
    report["face_highlight_map"] = {
      {"red", redFaces},
      {"yellow", yellowFaces}
    };

    json violationArray = json::array();
    for (const auto& violation : violations)
    {
        violationArray.push_back({
          {"severity", violation.first},
          {"affected_face_ids", violation.second}
        });
    }
    report["violations"] = violationArray;
    return report.dump();
}

std::string buildDfmReportWithViolationsOnly(
  const std::vector<std::pair<std::string, std::vector<std::string>>>& violations)
{
    json report;
    report["report_version"] = "2.0";
    report["violations"] = json::array();
    for (const auto& violation : violations)
    {
        report["violations"].push_back({
          {"severity", violation.first},
          {"affected_face_ids", violation.second}
        });
    }
    return report.dump();
}

std::string buildDfmReportWithDetailedViolations()
{
    json report;
    report["report_version"] = "2.0";
    report["violations"] = json::array({
      {
        {"severity", "yellow"},
        {"affected_face_ids", json::array({"1", "1", "2"})},
        {"message", "yellow-warning"},
        {"suggestions", json::array({"suggest-a", "suggest-b"})}
      },
      {
        {"severity", "red"},
        {"affected_face_ids", json::array({"1"})},
        {"message", "red-error"},
        {"suggestions", json::array({"suggest-c"})}
      }
    });
    return report.dump();
}

void checkColor(const Quantity_Color& color, double r, double g, double b)
{
    BOOST_CHECK_SMALL(std::abs(color.Red() - r), kColorTol);
    BOOST_CHECK_SMALL(std::abs(color.Green() - g), kColorTol);
    BOOST_CHECK_SMALL(std::abs(color.Blue() - b), kColorTol);
}

void checkOverlayFaceColor(const Handle(AIS_ColoredShape)& overlay,
                           const TopoDS_Face& face,
                           double r,
                           double g,
                           double b)
{
    const auto& customAspects = overlay->CustomAspectsMap();
    BOOST_REQUIRE(customAspects.IsBound(face));

    const Handle(AIS_ColoredDrawer) drawer = customAspects.Find(face);
    BOOST_REQUIRE(!drawer.IsNull());
    BOOST_REQUIRE(!drawer->ShadingAspect().IsNull());

    checkColor(drawer->ShadingAspect()->Color(), r, g, b);
}

} // namespace

BOOST_AUTO_TEST_CASE(load_feature_result_success)
{
    FeatureRecognitionModel model;
    BOOST_REQUIRE(model.loadResultFromJson(buildFeatureRecognitionJson()));

    const auto& groups = model.getFeatureGroups();
    BOOST_REQUIRE_EQUAL(groups.size(), 2);
    BOOST_CHECK_EQUAL(groups[0].name, "Through Hole(s)");
    BOOST_CHECK_EQUAL(groups[1].name, "Closed Pocket(s)");
}

BOOST_AUTO_TEST_CASE(apply_dfm_red_face_map_sets_group_red)
{
    FeatureRecognitionModel model;
    BOOST_REQUIRE(model.loadResultFromJson(buildFeatureRecognitionJson()));
    BOOST_REQUIRE(model.applyDfmReportFromJson(buildDfmReport({"1"}, {})));

    const auto& groups = model.getFeatureGroups();
    checkColor(groups[0].color, 0.92, 0.20, 0.20);
    checkColor(groups[1].color, 100.0 / 255.0, 110.0 / 255.0, 120.0 / 255.0);
}

BOOST_AUTO_TEST_CASE(apply_dfm_yellow_face_map_sets_group_yellow)
{
    FeatureRecognitionModel model;
    BOOST_REQUIRE(model.loadResultFromJson(buildFeatureRecognitionJson()));
    BOOST_REQUIRE(model.applyDfmReportFromJson(buildDfmReport({}, {"3"})));

    const auto& groups = model.getFeatureGroups();
    checkColor(groups[0].color, 10.0 / 255.0, 20.0 / 255.0, 30.0 / 255.0);
    checkColor(groups[1].color, 0.95, 0.78, 0.18);
}

BOOST_AUTO_TEST_CASE(red_overrides_yellow_in_same_group)
{
    FeatureRecognitionModel model;
    BOOST_REQUIRE(model.loadResultFromJson(buildFeatureRecognitionJson()));
    BOOST_REQUIRE(model.applyDfmReportFromJson(buildDfmReport({"1"}, {"2"})));

    const auto& groups = model.getFeatureGroups();
    checkColor(groups[0].color, 0.92, 0.20, 0.20);
}

BOOST_AUTO_TEST_CASE(non_matching_faces_keep_base_color)
{
    FeatureRecognitionModel model;
    BOOST_REQUIRE(model.loadResultFromJson(buildFeatureRecognitionJson()));
    BOOST_REQUIRE(model.applyDfmReportFromJson(buildDfmReport({"99"}, {"100"})));

    const auto& groups = model.getFeatureGroups();
    checkColor(groups[0].color, 10.0 / 255.0, 20.0 / 255.0, 30.0 / 255.0);
    checkColor(groups[1].color, 100.0 / 255.0, 110.0 / 255.0, 120.0 / 255.0);
}

BOOST_AUTO_TEST_CASE(violations_array_supported_without_face_map)
{
    FeatureRecognitionModel model;
    BOOST_REQUIRE(model.loadResultFromJson(buildFeatureRecognitionJson()));
    BOOST_REQUIRE(model.applyDfmReportFromJson(
      buildDfmReportWithViolationsOnly({{"yellow", {"3", "4"}}})));

    const auto& groups = model.getFeatureGroups();
    checkColor(groups[1].color, 0.95, 0.78, 0.18);
}

BOOST_AUTO_TEST_CASE(conflicting_sources_prefer_red)
{
    FeatureRecognitionModel model;
    BOOST_REQUIRE(model.loadResultFromJson(buildFeatureRecognitionJson()));
    BOOST_REQUIRE(model.applyDfmReportFromJson(
      buildDfmReport({}, {"1"}, {{"red", {"1"}}})));

    BOOST_CHECK(model.getFaceSeverity("1") == FeatureRecognitionModel::DfmSeverity::Red);
    checkColor(model.getDisplayColorForFace("1", Quantity_Color(0.1, 0.1, 0.1, Quantity_TOC_RGB)),
               0.92,
               0.20,
               0.20);
}

BOOST_AUTO_TEST_CASE(applying_empty_dfm_report_resets_to_base_colors)
{
    FeatureRecognitionModel model;
    BOOST_REQUIRE(model.loadResultFromJson(buildFeatureRecognitionJson()));
    BOOST_REQUIRE(model.applyDfmReportFromJson(buildDfmReport({"1"}, {})));
    BOOST_REQUIRE(model.applyDfmReportFromJson(buildDfmReport({}, {})));

    const auto& groups = model.getFeatureGroups();
    checkColor(groups[0].color, 10.0 / 255.0, 20.0 / 255.0, 30.0 / 255.0);
    checkColor(groups[1].color, 100.0 / 255.0, 110.0 / 255.0, 120.0 / 255.0);
}

BOOST_AUTO_TEST_CASE(invalid_dfm_json_returns_false)
{
    FeatureRecognitionModel model;
    BOOST_REQUIRE(model.loadResultFromJson(buildFeatureRecognitionJson()));
    BOOST_CHECK(!model.applyDfmReportFromJson("{invalid json"));
}

BOOST_AUTO_TEST_CASE(non_dfm_json_returns_false)
{
    FeatureRecognitionModel model;
    BOOST_REQUIRE(model.loadResultFromJson(buildFeatureRecognitionJson()));
    BOOST_CHECK(!model.applyDfmReportFromJson(R"({"foo":"bar"})"));
}

BOOST_AUTO_TEST_CASE(dfm_violation_details_can_be_queried_by_face)
{
    FeatureRecognitionModel model;
    BOOST_REQUIRE(model.loadResultFromJson(buildFeatureRecognitionJson()));
    BOOST_REQUIRE(model.applyDfmReportFromJson(buildDfmReportWithDetailedViolations()));

    const auto face1Violations = model.getDfmViolationsForFace("1");
    BOOST_REQUIRE_EQUAL(face1Violations.size(), 2);
    BOOST_CHECK(face1Violations[0].severity == FeatureRecognitionModel::DfmSeverity::Yellow);
    BOOST_CHECK_EQUAL(face1Violations[0].message, "yellow-warning");
    BOOST_CHECK_EQUAL(face1Violations[0].suggestions.size(), 2);
    BOOST_CHECK_EQUAL(face1Violations[0].suggestions[0], "suggest-a");
    BOOST_CHECK(face1Violations[1].severity == FeatureRecognitionModel::DfmSeverity::Red);
    BOOST_CHECK_EQUAL(face1Violations[1].message, "red-error");

    const auto face2Violations = model.getDfmViolationsForFace("2");
    BOOST_REQUIRE_EQUAL(face2Violations.size(), 1);
    BOOST_CHECK(face2Violations[0].severity == FeatureRecognitionModel::DfmSeverity::Yellow);
    BOOST_CHECK_EQUAL(face2Violations[0].message, "yellow-warning");
}

BOOST_AUTO_TEST_CASE(dfm_violation_query_returns_empty_for_unknown_face)
{
    FeatureRecognitionModel model;
    BOOST_REQUIRE(model.loadResultFromJson(buildFeatureRecognitionJson()));
    BOOST_REQUIRE(model.applyDfmReportFromJson(buildDfmReportWithDetailedViolations()));

    const auto unknownViolations = model.getDfmViolationsForFace("999");
    BOOST_CHECK(unknownViolations.empty());
}

BOOST_AUTO_TEST_CASE(dfm_processable_true_when_no_highlight_faces)
{
    FeatureRecognitionModel model;
    BOOST_REQUIRE(model.loadResultFromJson(buildFeatureRecognitionJson()));
    BOOST_REQUIRE(model.applyDfmReportFromJson(buildDfmReport({}, {})));

    BOOST_CHECK(model.hasDfmReport());
    BOOST_CHECK(model.isDfmProcessable());
}

BOOST_AUTO_TEST_CASE(dfm_processable_false_when_highlight_faces_exist)
{
    FeatureRecognitionModel model;
    BOOST_REQUIRE(model.loadResultFromJson(buildFeatureRecognitionJson()));
    BOOST_REQUIRE(model.applyDfmReportFromJson(buildDfmReport({}, {"3"})));

    BOOST_CHECK(model.hasDfmReport());
    BOOST_CHECK(!model.isDfmProcessable());
}

BOOST_AUTO_TEST_CASE(standalone_dfm_target_shape_supports_direct_face_highlight)
{
    FeatureRecognitionModel model;
    const TopoDS_Shape box = BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape();

    BOOST_REQUIRE(model.setDfmTargetShape(box));
    BOOST_REQUIRE(model.applyDfmReportFromJson(buildDfmReport({"1"}, {})));

    BOOST_CHECK(model.hasDfmReport());
    BOOST_CHECK(!model.isDfmProcessable());
    BOOST_CHECK(model.getFaceSeverity("1") == FeatureRecognitionModel::DfmSeverity::Red);
    BOOST_CHECK(!model.getFaceByID("1").IsNull());
}

BOOST_AUTO_TEST_CASE(dfm_overlay_only_overrides_violating_faces_within_feature_group)
{
    FeatureRecognitionModel model;
    const TopoDS_Shape box = BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape();

    BOOST_REQUIRE(model.setDfmTargetShape(box));
    BOOST_REQUIRE(model.loadResultFromJson(buildFeatureRecognitionJson()));
    BOOST_REQUIRE(model.applyDfmReportFromJson(buildDfmReport({"1"}, {})));

    int coloredFaceCount = 0;
    const Handle(AIS_ColoredShape) overlay = DfmOverlayBuilder::buildOverlay(model,
                                                                             &coloredFaceCount);

    BOOST_REQUIRE(!overlay.IsNull());
    BOOST_CHECK_EQUAL(coloredFaceCount, 4);

    checkOverlayFaceColor(overlay, model.getFaceByID("1"), 0.92, 0.20, 0.20);
    checkOverlayFaceColor(overlay,
                          model.getFaceByID("2"),
                          10.0 / 255.0,
                          20.0 / 255.0,
                          30.0 / 255.0);
    checkOverlayFaceColor(overlay,
                          model.getFaceByID("3"),
                          100.0 / 255.0,
                          110.0 / 255.0,
                          120.0 / 255.0);
}

BOOST_AUTO_TEST_CASE(recognize_shape_rejects_null_shape)
{
    FeatureRecognitionModel model;

    BOOST_CHECK(!model.recognizeShape(TopoDS_Shape()));
    BOOST_CHECK(!model.getLastError().empty());
    BOOST_CHECK(!model.hasResults());
}

BOOST_AUTO_TEST_CASE(recognize_shape_returns_without_process_crash)
{
    FeatureRecognitionModel model;
    const TopoDS_Shape box = BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape();
    const std::string params = R"({
        "recognitionStrategy": "RuleBasedOnly",
        "operationType": "Milling",
        "linearTolerance": 0.01,
        "recognizeHoles": true,
        "recognizePockets": true,
        "recognizeSlots": true
    })";

    const bool success = model.recognizeShape(box, params);

    BOOST_CHECK(success || !model.getLastError().empty());
    if (success)
    {
        BOOST_CHECK(model.hasResults());
        BOOST_CHECK(!model.getFeatureGroups().empty());
    }
}
