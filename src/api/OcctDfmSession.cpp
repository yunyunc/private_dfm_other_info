/**
 * @file OcctDfmSession.cpp
 * @brief Implementation of the reusable standalone DFM session.
 */

#include "OcctDfmSession.h"

#include "DfmOverlayBuilder.h"
#include "model/FeatureRecognitionModel.h"
#include "utils/Logger.h"

#include <IFSelect_ReturnStatus.hxx>
#include <STEPControl_Reader.hxx>
#include <TopAbs.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>
#include <sstream>

namespace
{
using json = nlohmann::json;

bool readBinaryTextFile(const std::filesystem::path& filePath, std::string& contents)
{
    std::ifstream inputFile(filePath, std::ios::in | std::ios::binary);
    if (!inputFile.is_open())
    {
        return false;
    }

    std::ostringstream buffer;
    buffer << inputFile.rdbuf();
    contents = buffer.str();
    return true;
}

OcctDfmSession::Severity toSessionSeverity(FeatureRecognitionModel::DfmSeverity severity)
{
    switch (severity)
    {
        case FeatureRecognitionModel::DfmSeverity::Red:
            return OcctDfmSession::Severity::Red;
        case FeatureRecognitionModel::DfmSeverity::Yellow:
            return OcctDfmSession::Severity::Yellow;
        case FeatureRecognitionModel::DfmSeverity::None:
        default:
            return OcctDfmSession::Severity::None;
    }
}

const char* toSeverityText(FeatureRecognitionModel::DfmSeverity severity)
{
    switch (severity)
    {
        case FeatureRecognitionModel::DfmSeverity::Red:
            return "red";
        case FeatureRecognitionModel::DfmSeverity::Yellow:
            return "yellow";
        case FeatureRecognitionModel::DfmSeverity::None:
        default:
            return "none";
    }
}

json colorToJson(const Quantity_Color& color)
{
    return json{{"r", color.Red()}, {"g", color.Green()}, {"b", color.Blue()}};
}
} // namespace

OcctDfmSession::OcctDfmSession()
    : myModel(std::make_shared<FeatureRecognitionModel>())
{
}

bool OcctDfmSession::initializeLogging(const std::string& logDirPath)
{
    return Utils::Logger::initialize(logDirPath);
}

bool OcctDfmSession::loadTargetShape(const TopoDS_Shape& shape)
{
    myLastError.clear();
    if (myModel->setDfmTargetShape(shape))
    {
        return true;
    }

    myLastError = myModel->getLastError();
    if (myLastError.empty())
    {
        myLastError = "failed to set target shape";
    }
    return false;
}

bool OcctDfmSession::loadTargetStepFile(const std::string& stepFilePath)
{
    myLastError.clear();
    STEPControl_Reader reader;
    const IFSelect_ReturnStatus status = reader.ReadFile(stepFilePath.c_str());
    if (status != IFSelect_RetDone)
    {
        myModel->clear();
        myLastError = "failed to read STEP file";
        return false;
    }

    reader.TransferRoots();
    const TopoDS_Shape shape = reader.OneShape();
    if (shape.IsNull())
    {
        myModel->clear();
        myLastError = "loaded STEP shape is null";
        return false;
    }

    return loadTargetShape(shape);
}

bool OcctDfmSession::loadDfmReportFromJson(const std::string& jsonString)
{
    myLastError.clear();
    if (myModel->applyDfmReportFromJson(jsonString))
    {
        return true;
    }

    myLastError = myModel->getLastError();
    if (myLastError.empty())
    {
        myLastError = "failed to parse DFM report JSON";
    }
    return false;
}

bool OcctDfmSession::loadDfmReportFromFile(const std::string& reportFilePath)
{
    myLastError.clear();
    std::string reportJson;
    if (!readBinaryTextFile(std::filesystem::u8path(reportFilePath), reportJson))
    {
        myLastError = "failed to read DFM report file";
        return false;
    }

    return loadDfmReportFromJson(reportJson);
}

void OcctDfmSession::clear()
{
    myLastError.clear();
    myModel->clear();
}

bool OcctDfmSession::hasDfmReport() const
{
    return myModel->hasDfmReport();
}

bool OcctDfmSession::hasOverlayData() const
{
    return myModel->hasResults() || myModel->hasDfmReport();
}

bool OcctDfmSession::isDfmProcessable() const
{
    return myModel->isDfmProcessable();
}

std::string OcctDfmSession::getLastError() const
{
    if (!myLastError.empty())
    {
        return myLastError;
    }

    return myModel->getLastError();
}

std::string OcctDfmSession::getFaceId(const TopoDS_Face& face) const
{
    return myModel->getFaceId(face);
}

OcctDfmSession::Severity OcctDfmSession::getFaceSeverity(const std::string& faceId) const
{
    return ::toSessionSeverity(myModel->getFaceSeverity(faceId));
}

Quantity_Color OcctDfmSession::getFaceDisplayColor(const std::string& faceId) const
{
    return myModel->getDisplayColorForFace(faceId, Quantity_Color(0.5, 0.5, 0.5, Quantity_TOC_RGB));
}

std::vector<OcctDfmSession::Violation> OcctDfmSession::getViolationsForFace(const std::string& faceId) const
{
    std::vector<Violation> result;
    const auto violations = myModel->getDfmViolationsForFace(faceId);
    result.reserve(violations.size());
    for (const auto& violation : violations)
    {
        result.push_back(Violation{::toSessionSeverity(violation.severity),
                                   violation.message,
                                   violation.suggestions});
    }
    return result;
}

Handle(AIS_ColoredShape) OcctDfmSession::buildOverlay(int* coloredFaceCount) const
{
    return DfmOverlayBuilder::buildOverlay(*myModel, coloredFaceCount);
}

std::string OcctDfmSession::buildVisualizationJson() const
{
    json root;
    root["has_dfm_report"] = hasDfmReport();
    root["processable"] = isDfmProcessable();
    root["step_face_index_scheme"] = "TopExp_Explorer face order, 1-based";
    root["highlighted_face_count"] = 0;
    root["faces"] = json::array();

    const TopoDS_Shape& originalShape = myModel->getOriginalShape();
    if (!hasDfmReport() || originalShape.IsNull())
    {
        return root.dump();
    }

    int totalFaceCount = 0;
    json faces = json::array();
    for (TopExp_Explorer exp(originalShape, TopAbs_FACE); exp.More(); exp.Next())
    {
        ++totalFaceCount;
        const TopoDS_Face face = TopoDS::Face(exp.Current());
        const std::string faceId = myModel->getFaceId(face);
        if (faceId.empty())
        {
            continue;
        }

        const FeatureRecognitionModel::DfmSeverity severity = myModel->getFaceSeverity(faceId);
        if (severity == FeatureRecognitionModel::DfmSeverity::None)
        {
            continue;
        }

        json violationArray = json::array();
        for (const auto& violation : myModel->getDfmViolationsForFace(faceId))
        {
            violationArray.push_back(
              {{"severity", toSeverityText(violation.severity)},
               {"message", violation.message},
               {"suggestions", violation.suggestions}});
        }

        faces.push_back({{"face_id", faceId},
                         {"severity", toSeverityText(severity)},
                         {"color",
                          colorToJson(myModel->getDisplayColorForFace(
                            faceId,
                            Quantity_Color(0.5, 0.5, 0.5, Quantity_TOC_RGB)))},
                         {"violations", violationArray}});
    }

    root["total_face_count"] = totalFaceCount;
    root["highlighted_face_count"] = faces.size();
    root["faces"] = std::move(faces);

    if (isDfmProcessable())
    {
        root["part_color"] = colorToJson(Quantity_Color(0.20, 0.75, 0.30, Quantity_TOC_RGB));
    }

    return root.dump();
}
