#define BOOST_TEST_MODULE OcctShapeOwnerUtilsTest
#include <boost/test/unit_test.hpp>

#include "view/OcctShapeOwnerUtils.h"

#include <BRepPrimAPI_MakeBox.hxx>
#include <SelectMgr_EntityOwner.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>

BOOST_AUTO_TEST_CASE(extract_face_from_null_owner_returns_null_face)
{
    const TopoDS_Face face =
        OcctShapeOwnerUtils::extractFaceFromOwner(Handle(SelectMgr_EntityOwner)());
    BOOST_TEST(face.IsNull());
}

BOOST_AUTO_TEST_CASE(extract_face_from_non_brep_owner_returns_null_face)
{
    Handle(SelectMgr_EntityOwner) owner = new SelectMgr_EntityOwner();
    const TopoDS_Face face = OcctShapeOwnerUtils::extractFaceFromOwner(owner);
    BOOST_TEST(face.IsNull());
}

BOOST_AUTO_TEST_CASE(extract_face_from_solid_owner_returns_null_face)
{
    const TopoDS_Shape box = BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape();
    Handle(StdSelect_BRepOwner) owner = new StdSelect_BRepOwner(box);
    const TopoDS_Face face = OcctShapeOwnerUtils::extractFaceFromOwner(owner);
    BOOST_TEST(face.IsNull());
}

BOOST_AUTO_TEST_CASE(extract_face_from_face_owner_returns_same_face)
{
    const TopoDS_Shape box = BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape();
    TopoDS_Face sourceFace;
    for (TopExp_Explorer exp(box, TopAbs_FACE); exp.More(); exp.Next())
    {
        sourceFace = TopoDS::Face(exp.Current());
        break;
    }

    BOOST_REQUIRE(!sourceFace.IsNull());

    Handle(StdSelect_BRepOwner) owner = new StdSelect_BRepOwner(sourceFace);
    const TopoDS_Face extractedFace = OcctShapeOwnerUtils::extractFaceFromOwner(owner);

    BOOST_TEST(!extractedFace.IsNull());
    BOOST_TEST(extractedFace.IsSame(sourceFace));
}
