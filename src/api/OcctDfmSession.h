/**
 * @file OcctDfmSession.h
 * @brief Internal reusable DFM session for loading reports and building overlays.
 */
#pragma once

#include <AIS_ColoredShape.hxx>
#include <Quantity_Color.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>

#include <memory>
#include <string>
#include <vector>

class FeatureRecognitionModel;

/**
 * @class OcctDfmSession
 * @brief Reusable DFM session around FeatureRecognitionModel for non-UI integration.
 */
class OcctDfmSession
{
public:
    /**
     * @brief DFM severity exposed by the session.
     */
    enum class Severity
    {
        None = 0,
        Yellow = 1,
        Red = 2
    };

    /**
     * @brief Serializable DFM violation payload for external callers.
     */
    struct Violation
    {
        Severity severity = Severity::None;
        std::string message;
        std::vector<std::string> suggestions;
    };

    /**
     * @brief Constructs a new DFM session.
     */
    OcctDfmSession();

    /**
     * @brief Initializes logging for standalone SDK usage.
     * @param logDirPath Log directory path.
     * @return true when logging is ready.
     */
    bool initializeLogging(const std::string& logDirPath = "logs");

    /**
     * @brief Sets the target CAD shape used for face-id mapping and overlay generation.
     * @param shape Target OCCT shape.
     * @return true when the shape is valid.
     */
    bool loadTargetShape(const TopoDS_Shape& shape);

    /**
     * @brief Loads the target CAD shape from a STEP file.
     * @param stepFilePath STEP file path.
     * @return true when the file is loaded successfully.
     */
    bool loadTargetStepFile(const std::string& stepFilePath);

    /**
     * @brief Loads a DFM report from a JSON string.
     * @param jsonString UTF-8 JSON content.
     * @return true when parsing succeeds.
     */
    bool loadDfmReportFromJson(const std::string& jsonString);

    /**
     * @brief Loads a DFM report from a JSON file.
     * @param reportFilePath JSON report file path.
     * @return true when the file is read and parsed successfully.
     */
    bool loadDfmReportFromFile(const std::string& reportFilePath);

    /**
     * @brief Clears target shape, DFM report and cached model state.
     */
    void clear();

    /**
     * @brief Returns whether a DFM report is loaded.
     */
    bool hasDfmReport() const;

    /**
     * @brief Returns whether current session has enough data to build a visualization overlay.
     */
    bool hasOverlayData() const;

    /**
     * @brief Returns whether current DFM report indicates machinable.
     */
    bool isDfmProcessable() const;

    /**
     * @brief Returns the last error message from the underlying model/session.
     */
    std::string getLastError() const;

    /**
     * @brief Returns the model face-id for an OCCT face.
     * @param face Target face.
     * @return Face-id string, or empty string when unmapped.
     */
    std::string getFaceId(const TopoDS_Face& face) const;

    /**
     * @brief Returns the DFM severity for a face-id.
     * @param faceId Face-id string.
     * @return Face severity.
     */
    Severity getFaceSeverity(const std::string& faceId) const;

    /**
     * @brief Returns the display color for a face-id.
     * @param faceId Face-id string.
     * @return OCCT RGB color.
     */
    Quantity_Color getFaceDisplayColor(const std::string& faceId) const;

    /**
     * @brief Returns all DFM violations for a face-id.
     * @param faceId Face-id string.
     * @return Violation list.
     */
    std::vector<Violation> getViolationsForFace(const std::string& faceId) const;

    /**
     * @brief Builds an overlay for the currently loaded shape/report state.
     * @param coloredFaceCount Optional number of explicitly colored faces.
     * @return OCCT colored overlay handle, or null when nothing is available.
     */
    Handle(AIS_ColoredShape) buildOverlay(int* coloredFaceCount = nullptr) const;

    /**
     * @brief Builds a JSON summary for external non-OCCT callers.
     * @return UTF-8 JSON string containing highlighted face colors and violations.
     */
    std::string buildVisualizationJson() const;

private:
    std::shared_ptr<FeatureRecognitionModel> myModel;
    std::string myLastError;
};
