/**
 * @file OcctDfmSdk.h
 * @brief Public DLL interface for loading DFM reports and building OCCT overlays.
 */
#pragma once

#include <AIS_ColoredShape.hxx>
#include <AIS_InteractiveContext.hxx>
#include <Quantity_Color.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <V3d_View.hxx>

#include <memory>
#include <string>
#include <vector>

#ifdef _WIN32
#  ifdef OCCTDFM_SDK_EXPORTS
#    define OCCTDFM_API __declspec(dllexport)
#  else
#    define OCCTDFM_API __declspec(dllimport)
#  endif
#else
#  define OCCTDFM_API
#endif

/**
 * @class OcctDfmSdkSession
 * @brief Public facade exported from the DFM DLL for external OCCT-based integrations.
 */
class OCCTDFM_API OcctDfmSdkSession
{
public:
    /**
     * @brief DFM severity level exposed to external callers.
     */
    enum class Severity
    {
        None = 0,
        Yellow = 1,
        Red = 2
    };

    /**
     * @brief DFM violation payload exposed to external callers.
     */
    struct Violation
    {
        Severity severity = Severity::None;
        std::string message;
        std::vector<std::string> suggestions;
    };

    /**
     * @brief Constructs a new exported DFM session.
     */
    OcctDfmSdkSession();

    /**
     * @brief Destroys the exported DFM session.
     */
    ~OcctDfmSdkSession();

    OcctDfmSdkSession(OcctDfmSdkSession&& other) noexcept;
    OcctDfmSdkSession& operator=(OcctDfmSdkSession&& other) noexcept;

    OcctDfmSdkSession(const OcctDfmSdkSession&) = delete;
    OcctDfmSdkSession& operator=(const OcctDfmSdkSession&) = delete;

    /**
     * @brief Initializes SDK logging.
     * @param logDirPath Directory for generated logs.
     * @return true when logging initialization succeeds.
     */
    bool initializeLogging(const std::string& logDirPath = "logs");

    /**
     * @brief Loads a target OCCT shape for face-id mapping.
     * @param shape Target shape.
     * @return true when the shape is valid.
     */
    bool loadTargetShape(const TopoDS_Shape& shape);

    /**
     * @brief Loads a target shape directly from a STEP file.
     * @param stepFilePath STEP file path.
     * @return true when the file is loaded successfully.
     */
    bool loadTargetStepFile(const std::string& stepFilePath);

    /**
     * @brief Loads a DFM report from UTF-8 JSON text.
     * @param jsonString UTF-8 JSON content.
     * @return true when parsing succeeds.
     */
    bool loadDfmReportFromJson(const std::string& jsonString);

    /**
     * @brief Loads a DFM report from a JSON file.
     * @param reportFilePath JSON file path.
     * @return true when the file is read and parsed successfully.
     */
    bool loadDfmReportFromFile(const std::string& reportFilePath);

    /**
     * @brief Clears loaded shape/report state and any displayed overlay handle.
     */
    void clear();

    /**
     * @brief Returns whether a DFM report is loaded.
     */
    bool hasDfmReport() const;

    /**
     * @brief Returns whether overlay data is currently available.
     */
    bool hasOverlayData() const;

    /**
     * @brief Returns whether the DFM report marks the part as machinable.
     */
    bool isDfmProcessable() const;

    /**
     * @brief Returns the last error text.
     */
    std::string getLastError() const;

    /**
     * @brief Returns the mapped face-id for an OCCT face.
     * @param face Target face.
     * @return Face-id string, or empty when unmapped.
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
     * @brief Returns the violation list for a face-id.
     * @param faceId Face-id string.
     * @return Violation list.
     */
    std::vector<Violation> getViolationsForFace(const std::string& faceId) const;

    /**
     * @brief Builds an OCCT overlay for the current state.
     * @return New overlay handle, or null when nothing is available.
     */
    Handle(AIS_ColoredShape) buildOverlay() const;

    /**
     * @brief Displays the current overlay into an external OCCT interactive context.
     * @param context External interactive context.
     * @param view Optional OCCT view used for redraw.
     * @return true when the overlay is displayed successfully.
     */
    bool displayOverlay(const Handle(AIS_InteractiveContext)& context,
                        const Handle(V3d_View)& view = Handle(V3d_View)());

    /**
     * @brief Removes the current overlay from an external OCCT interactive context.
     * @param context External interactive context.
     * @param view Optional OCCT view used for redraw.
     */
    void clearOverlay(const Handle(AIS_InteractiveContext)& context,
                      const Handle(V3d_View)& view = Handle(V3d_View)());

private:
    class Impl;
    std::unique_ptr<Impl> myImpl;
};
