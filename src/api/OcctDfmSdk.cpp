/**
 * @file OcctDfmSdk.cpp
 * @brief Exported DLL facade for standalone DFM integration.
 */

#include "OcctDfmSdk.h"

#include "OcctDfmSession.h"

class OcctDfmSdkSession::Impl
{
public:
    OcctDfmSession session;
    Handle(AIS_ColoredShape) displayedOverlay;
};

namespace
{
OcctDfmSdkSession::Severity toSdkSeverity(OcctDfmSession::Severity severity)
{
    switch (severity)
    {
        case OcctDfmSession::Severity::Red:
            return OcctDfmSdkSession::Severity::Red;
        case OcctDfmSession::Severity::Yellow:
            return OcctDfmSdkSession::Severity::Yellow;
        case OcctDfmSession::Severity::None:
        default:
            return OcctDfmSdkSession::Severity::None;
    }
}
} // namespace

OcctDfmSdkSession::OcctDfmSdkSession()
    : myImpl(std::make_unique<Impl>())
{
}

OcctDfmSdkSession::~OcctDfmSdkSession() = default;

OcctDfmSdkSession::OcctDfmSdkSession(OcctDfmSdkSession&& other) noexcept = default;

OcctDfmSdkSession& OcctDfmSdkSession::operator=(OcctDfmSdkSession&& other) noexcept = default;

bool OcctDfmSdkSession::initializeLogging(const std::string& logDirPath)
{
    return myImpl->session.initializeLogging(logDirPath);
}

bool OcctDfmSdkSession::loadTargetShape(const TopoDS_Shape& shape)
{
    return myImpl->session.loadTargetShape(shape);
}

bool OcctDfmSdkSession::loadTargetStepFile(const std::string& stepFilePath)
{
    return myImpl->session.loadTargetStepFile(stepFilePath);
}

bool OcctDfmSdkSession::loadDfmReportFromJson(const std::string& jsonString)
{
    return myImpl->session.loadDfmReportFromJson(jsonString);
}

bool OcctDfmSdkSession::loadDfmReportFromFile(const std::string& reportFilePath)
{
    return myImpl->session.loadDfmReportFromFile(reportFilePath);
}

void OcctDfmSdkSession::clear()
{
    myImpl->displayedOverlay.Nullify();
    myImpl->session.clear();
}

bool OcctDfmSdkSession::hasDfmReport() const
{
    return myImpl->session.hasDfmReport();
}

bool OcctDfmSdkSession::hasOverlayData() const
{
    return myImpl->session.hasOverlayData();
}

bool OcctDfmSdkSession::isDfmProcessable() const
{
    return myImpl->session.isDfmProcessable();
}

std::string OcctDfmSdkSession::getLastError() const
{
    return myImpl->session.getLastError();
}

std::string OcctDfmSdkSession::getFaceId(const TopoDS_Face& face) const
{
    return myImpl->session.getFaceId(face);
}

OcctDfmSdkSession::Severity OcctDfmSdkSession::getFaceSeverity(const std::string& faceId) const
{
    return toSdkSeverity(myImpl->session.getFaceSeverity(faceId));
}

Quantity_Color OcctDfmSdkSession::getFaceDisplayColor(const std::string& faceId) const
{
    return myImpl->session.getFaceDisplayColor(faceId);
}

std::vector<OcctDfmSdkSession::Violation>
OcctDfmSdkSession::getViolationsForFace(const std::string& faceId) const
{
    std::vector<Violation> result;
    const auto violations = myImpl->session.getViolationsForFace(faceId);
    result.reserve(violations.size());
    for (const auto& violation : violations)
    {
        result.push_back(
          Violation{toSdkSeverity(violation.severity), violation.message, violation.suggestions});
    }
    return result;
}

Handle(AIS_ColoredShape) OcctDfmSdkSession::buildOverlay() const
{
    return myImpl->session.buildOverlay();
}

bool OcctDfmSdkSession::displayOverlay(const Handle(AIS_InteractiveContext)& context,
                                       const Handle(V3d_View)& view)
{
    if (context.IsNull())
    {
        return false;
    }

    const Handle(AIS_ColoredShape) overlay = myImpl->session.buildOverlay();
    if (overlay.IsNull())
    {
        return false;
    }

    if (!myImpl->displayedOverlay.IsNull())
    {
        context->Remove(myImpl->displayedOverlay, false);
    }

    myImpl->displayedOverlay = overlay;
    context->Display(myImpl->displayedOverlay, AIS_Shaded, 0, false);
    context->Deactivate(myImpl->displayedOverlay);

    if (!view.IsNull())
    {
        view->Redraw();
    }
    else
    {
        context->UpdateCurrentViewer();
    }

    return true;
}

void OcctDfmSdkSession::clearOverlay(const Handle(AIS_InteractiveContext)& context,
                                     const Handle(V3d_View)& view)
{
    if (!context.IsNull() && !myImpl->displayedOverlay.IsNull())
    {
        context->Remove(myImpl->displayedOverlay, false);
        if (!view.IsNull())
        {
            view->Redraw();
        }
        else
        {
            context->UpdateCurrentViewer();
        }
    }

    myImpl->displayedOverlay.Nullify();
}
