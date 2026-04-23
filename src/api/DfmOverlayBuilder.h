/**
 * @file DfmOverlayBuilder.h
 * @brief Builds OCCT colored overlays from feature/DFM model state.
 */
#pragma once

#include <AIS_ColoredShape.hxx>

class FeatureRecognitionModel;

namespace DfmOverlayBuilder
{
/**
 * @brief Builds an OCCT colored overlay for current feature/DFM visualization state.
 * @param model Feature/DFM model containing face map, colors and report state.
 * @param coloredFaceCount Optional output for the number of explicitly colored faces.
 * @return Colored overlay handle, or null when there is nothing to visualize.
 */
Handle(AIS_ColoredShape) buildOverlay(const FeatureRecognitionModel& model,
                                      int* coloredFaceCount = nullptr);
} // namespace DfmOverlayBuilder
