#define BOOST_TEST_MODULE ShapeOrientationUtilsTest
#include <boost/test/unit_test.hpp>

#include "model/ShapeOrientationUtils.h"

#include <BRepAdaptor_Surface.hxx>
#include <BRepBuilderAPI_Transform.hxx>
#include <BRepGProp.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakeSphere.hxx>
#include <GProp_GProps.hxx>
#include <GeomAbs_SurfaceType.hxx>
#include <TopAbs.hxx>
#include <TopAbs_Orientation.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <gp_Ax1.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>

#include <cmath>

namespace
{
int faceCountOf(const TopoDS_Shape& shape)
{
    int count = 0;
    for (TopExp_Explorer explorer(shape, TopAbs_FACE); explorer.More(); explorer.Next())
    {
        ++count;
    }
    return count;
}

double planarFaceArea(const TopoDS_Face& face)
{
    GProp_GProps properties;
    BRepGProp::SurfaceProperties(face, properties);
    return properties.Mass();
}

gp_Dir planarFaceNormal(const TopoDS_Face& face)
{
    BRepAdaptor_Surface surface(face, Standard_True);
    gp_Dir normal = surface.Plane().Axis().Direction();
    if (face.Orientation() == TopAbs_REVERSED)
    {
        normal.Reverse();
    }
    return normal;
}

bool hasDownwardLargestPlanarFace(const TopoDS_Shape& shape)
{
    double maxArea = -1.0;
    for (TopExp_Explorer explorer(shape, TopAbs_FACE); explorer.More(); explorer.Next())
    {
        const TopoDS_Face face = TopoDS::Face(explorer.Current());
        BRepAdaptor_Surface surface(face, Standard_True);
        if (surface.GetType() != GeomAbs_Plane)
        {
            continue;
        }

        maxArea = std::max(maxArea, planarFaceArea(face));
    }

    if (maxArea <= 0.0)
    {
        return false;
    }

    constexpr double areaTolerance = 1.0e-7;
    for (TopExp_Explorer explorer(shape, TopAbs_FACE); explorer.More(); explorer.Next())
    {
        const TopoDS_Face face = TopoDS::Face(explorer.Current());
        BRepAdaptor_Surface surface(face, Standard_True);
        if (surface.GetType() != GeomAbs_Plane)
        {
            continue;
        }

        if (std::abs(planarFaceArea(face) - maxArea) > areaTolerance)
        {
            continue;
        }

        const gp_Dir normal = planarFaceNormal(face);
        if (normal.Z() < -0.999)
        {
            return true;
        }
    }

    return false;
}
} // namespace

BOOST_AUTO_TEST_CASE(rotates_largest_planar_face_to_negative_z)
{
    const TopoDS_Shape box = BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape();

    gp_Trsf rotate;
    rotate.SetRotation(gp_Ax1(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 1.0, 0.0)), M_PI_2);
    const TopoDS_Shape rotatedBox = BRepBuilderAPI_Transform(box, rotate, Standard_True).Shape();

    const TopoDS_Shape orientedBox = ShapeOrientationUtils::orientLargestPlanarFaceDown(rotatedBox);

    BOOST_TEST(!orientedBox.IsNull());
    BOOST_CHECK_EQUAL(faceCountOf(rotatedBox), faceCountOf(orientedBox));
    BOOST_CHECK(hasDownwardLargestPlanarFace(orientedBox));
}

BOOST_AUTO_TEST_CASE(non_planar_shape_is_left_usable)
{
    const TopoDS_Shape sphere = BRepPrimAPI_MakeSphere(15.0).Shape();

    const TopoDS_Shape orientedSphere = ShapeOrientationUtils::orientLargestPlanarFaceDown(sphere);

    BOOST_TEST(!orientedSphere.IsNull());
    BOOST_CHECK_EQUAL(faceCountOf(sphere), faceCountOf(orientedSphere));
}
