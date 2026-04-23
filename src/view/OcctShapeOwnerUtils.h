/**
 * @file OcctShapeOwnerUtils.h
 * @brief Helpers for safely extracting B-Rep sub-shapes from selection owners.
 */
#pragma once

#include <SelectMgr_EntityOwner.hxx>
#include <StdSelect_BRepOwner.hxx>
#include <TopAbs.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>

namespace OcctShapeOwnerUtils
{
/**
 * @brief Extracts a face from a detected/selected owner when the owner really carries a face.
 * @param owner Detected or selected owner from AIS selection context.
 * @return Extracted face, or a null face when the owner is not a face-bearing B-Rep owner.
 */
inline TopoDS_Face extractFaceFromOwner(const Handle(SelectMgr_EntityOwner)& owner)
{
    Handle(StdSelect_BRepOwner) brepOwner = Handle(StdSelect_BRepOwner)::DownCast(owner);
    if (brepOwner.IsNull() || !brepOwner->HasShape())
    {
        return TopoDS_Face();
    }

    const TopoDS_Shape& shape = brepOwner->Shape();
    if (shape.IsNull() || shape.ShapeType() != TopAbs_FACE)
    {
        return TopoDS_Face();
    }

    return TopoDS::Face(shape);
}
} // namespace OcctShapeOwnerUtils
