/**
 * @file DfmOverlayBuilder.cpp
 * @brief Implementation of OCCT colored overlay construction for DFM visualization.
 */

#include "DfmOverlayBuilder.h"

#include "model/FeatureRecognitionModel.h"

#include <Graphic3d_NameOfMaterial.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Quantity_Color.hxx>
#include <TopAbs.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>

#include <unordered_set>

namespace DfmOverlayBuilder
{
Handle(AIS_ColoredShape) buildOverlay(const FeatureRecognitionModel& model, int* coloredFaceCount)
{
    if (coloredFaceCount != nullptr)
    {
        *coloredFaceCount = 0;
    }

    const bool hasFeatureResults = model.hasResults();
    const bool hasDfmReport = model.hasDfmReport();
    if (!hasFeatureResults && !hasDfmReport)
    {
        return Handle(AIS_ColoredShape)();
    }

    const TopoDS_Shape& originalShape = model.getOriginalShape();
    if (originalShape.IsNull())
    {
        return Handle(AIS_ColoredShape)();
    }

    Handle(AIS_ColoredShape) overview = new AIS_ColoredShape(originalShape);
    overview->SetDisplayMode(AIS_Shaded);
    overview->SetMaterial(Graphic3d_NOM_PLASTIC);
    overview->SetTransparency(0.65f);
    overview->SetColor(Quantity_Color(0.32, 0.36, 0.42, Quantity_TOC_RGB));
    overview->Attributes()->SetFaceBoundaryDraw(true);
    overview->Attributes()->SetFaceBoundaryAspect(
      new Prs3d_LineAspect(Quantity_Color(0.25, 0.25, 0.25, Quantity_TOC_RGB),
                           Aspect_TOL_SOLID,
                           1.0f));

    std::unordered_set<std::string> coloredFaceIds;
    if (hasFeatureResults)
    {
        const auto& groups = model.getFeatureGroups();
        for (int groupIdx = 0; groupIdx < static_cast<int>(groups.size()); ++groupIdx)
        {
            const auto& group = groups[groupIdx];
            if (!model.isGroupVisible(groupIdx))
            {
                continue;
            }

            const auto faceIDs = model.getFaceIDsForFeature(groupIdx, -1, -1);
            if (faceIDs.empty())
            {
                continue;
            }

            for (const auto& faceID : faceIDs)
            {
                const TopoDS_Face face = model.getFaceByID(faceID);
                if (face.IsNull())
                {
                    continue;
                }

                overview->SetCustomColor(face, group.baseColor);
                overview->SetCustomTransparency(face, 0.1f);
                coloredFaceIds.insert(faceID);
            }
        }
    }

    if (hasDfmReport)
    {
        if (model.isDfmProcessable())
        {
            if (!hasFeatureResults)
            {
                overview->SetColor(Quantity_Color(0.20, 0.75, 0.30, Quantity_TOC_RGB));
                overview->SetTransparency(0.15f);
            }
        }
        else
        {
            for (TopExp_Explorer exp(originalShape, TopAbs_FACE); exp.More(); exp.Next())
            {
                const TopoDS_Face face = TopoDS::Face(exp.Current());
                const std::string faceId = model.getFaceId(face);
                if (faceId.empty())
                {
                    continue;
                }

                if (model.getFaceSeverity(faceId) == FeatureRecognitionModel::DfmSeverity::None)
                {
                    continue;
                }

                overview->SetCustomColor(face,
                                         model.getDisplayColorForFace(
                                           faceId,
                                           Quantity_Color(0.5, 0.5, 0.5, Quantity_TOC_RGB)));
                overview->SetCustomTransparency(face, 0.1f);
                coloredFaceIds.insert(faceId);
            }
        }
    }

    if (coloredFaceCount != nullptr)
    {
        *coloredFaceCount = static_cast<int>(coloredFaceIds.size());
    }
    return overview;
}
} // namespace DfmOverlayBuilder
