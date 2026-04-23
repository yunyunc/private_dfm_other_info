/**
 * @file ShapeOrientationUtils.h
 * @brief Utilities for normalizing imported CAD shape orientation.
 */
#pragma once

#include <TopoDS_Shape.hxx>

namespace ShapeOrientationUtils
{
/**
 * @brief Rotates a shape so that its largest planar face points downward along `-Z`.
 *
 * If no planar face is found, the original shape is returned unchanged.
 *
 * @param shape Input shape.
 * @return Re-oriented shape, or the original shape when no re-orientation is possible.
 */
TopoDS_Shape orientLargestPlanarFaceDown(const TopoDS_Shape& shape);
} // namespace ShapeOrientationUtils
