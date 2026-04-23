/**
 * @file ShapeOrientationUtils.cpp
 * @brief Implementation of CAD shape orientation normalization helpers.
 */

#include "ShapeOrientationUtils.h"

#include <BRepAdaptor_Surface.hxx>
#include <BRepBndLib.hxx>
#include <BRepBuilderAPI_Transform.hxx>
#include <BRepGProp.hxx>
#include <Bnd_Box.hxx>
#include <GProp_GProps.hxx>
#include <GeomAbs_SurfaceType.hxx>
#include <Precision.hxx>
#include <TopAbs_Orientation.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <gp_Pnt.hxx>
#include <gp_Quaternion.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>

#include <cmath>

namespace
{
struct LargestPlanarFaceInfo
{
    TopoDS_Face face;
    gp_Dir normal;
    double area = -1.0;
    bool found = false;
};

LargestPlanarFaceInfo findLargestPlanarFace(const TopoDS_Shape& shape)
{
    LargestPlanarFaceInfo bestFace;
    constexpr double areaTolerance = 1.0e-7;

    for (TopExp_Explorer explorer(shape, TopAbs_FACE); explorer.More(); explorer.Next())
    {
        const TopoDS_Face face = TopoDS::Face(explorer.Current());
        if (face.IsNull())
        {
            continue;
        }

        BRepAdaptor_Surface surface(face, Standard_True);
        if (surface.GetType() != GeomAbs_Plane)
        {
            continue;
        }

        GProp_GProps properties;
        BRepGProp::SurfaceProperties(face, properties);
        const double area = properties.Mass();
        if (area <= areaTolerance)
        {
            continue;
        }

        gp_Dir normal = surface.Plane().Axis().Direction();
        if (face.Orientation() == TopAbs_REVERSED)
        {
            normal.Reverse();
        }

        const bool largerArea = area > bestFace.area + areaTolerance;
        const bool sameAreaBetterDownward =
          std::abs(area - bestFace.area) <= areaTolerance
          && (!bestFace.found || normal.Z() < bestFace.normal.Z());

        if (!bestFace.found || largerArea || sameAreaBetterDownward)
        {
            bestFace.face = face;
            bestFace.normal = normal;
            bestFace.area = area;
            bestFace.found = true;
        }
    }

    return bestFace;
}

gp_Pnt boundingBoxCenterOf(const TopoDS_Shape& shape)
{
    Bnd_Box bounds;
    BRepBndLib::Add(shape, bounds);
    if (bounds.IsVoid())
    {
        return gp_Pnt(0.0, 0.0, 0.0);
    }

    Standard_Real xmin = 0.0;
    Standard_Real ymin = 0.0;
    Standard_Real zmin = 0.0;
    Standard_Real xmax = 0.0;
    Standard_Real ymax = 0.0;
    Standard_Real zmax = 0.0;
    bounds.Get(xmin, ymin, zmin, xmax, ymax, zmax);
    return gp_Pnt((xmin + xmax) * 0.5, (ymin + ymax) * 0.5, (zmin + zmax) * 0.5);
}
} // namespace

namespace ShapeOrientationUtils
{
TopoDS_Shape orientLargestPlanarFaceDown(const TopoDS_Shape& shape)
{
    if (shape.IsNull())
    {
        return shape;
    }

    const LargestPlanarFaceInfo largestFace = findLargestPlanarFace(shape);
    if (!largestFace.found)
    {
        return shape;
    }

    const gp_Vec currentNormal(largestFace.normal.X(), largestFace.normal.Y(), largestFace.normal.Z());
    const gp_Vec targetNormal(0.0, 0.0, -1.0);
    const double normalizedDot =
      currentNormal.Dot(targetNormal) / (currentNormal.Magnitude() * targetNormal.Magnitude());
    if (normalizedDot >= 1.0 - Precision::Angular())
    {
        return shape;
    }

    const gp_Pnt center = boundingBoxCenterOf(shape);

    gp_Trsf moveToOrigin;
    moveToOrigin.SetTranslation(gp_Vec(center, gp_Pnt(0.0, 0.0, 0.0)));

    gp_Trsf rotation;
    rotation.SetRotation(gp_Quaternion(currentNormal, targetNormal));

    gp_Trsf moveBack;
    moveBack.SetTranslation(gp_Vec(gp_Pnt(0.0, 0.0, 0.0), center));

    TopoDS_Shape orientedShape = BRepBuilderAPI_Transform(shape, moveToOrigin, Standard_True).Shape();
    orientedShape = BRepBuilderAPI_Transform(orientedShape, rotation, Standard_True).Shape();
    orientedShape = BRepBuilderAPI_Transform(orientedShape, moveBack, Standard_True).Shape();

    return orientedShape.IsNull() ? shape : orientedShape;
}
} // namespace ShapeOrientationUtils
