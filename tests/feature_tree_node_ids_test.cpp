#define BOOST_TEST_MODULE FeatureTreeNodeIdsTest
#include <boost/test/unit_test.hpp>

#include "view/FeatureTreeNodeIds.h"

#include <unordered_set>

BOOST_AUTO_TEST_CASE(group_ids_are_unique_for_multiple_groups)
{
    std::unordered_set<std::string> ids;
    for (int groupIdx = 0; groupIdx < 10; ++groupIdx)
    {
        BOOST_TEST(ids.insert(FeatureTreeNodeIds::group(groupIdx)).second);
    }
}

BOOST_AUTO_TEST_CASE(subgroup_ids_are_unique_for_multiple_paths)
{
    std::unordered_set<std::string> ids;
    for (int groupIdx = 0; groupIdx < 4; ++groupIdx)
    {
        for (int subGroupIdx = 0; subGroupIdx < 4; ++subGroupIdx)
        {
            BOOST_TEST(ids.insert(FeatureTreeNodeIds::subGroup(groupIdx, subGroupIdx)).second);
        }
    }
}

BOOST_AUTO_TEST_CASE(feature_ids_are_unique_for_direct_features)
{
    std::unordered_set<std::string> ids;
    for (int featureIdx = 0; featureIdx < 60; ++featureIdx)
    {
        BOOST_TEST(ids.insert(FeatureTreeNodeIds::feature(0, -1, featureIdx)).second);
    }
}

BOOST_AUTO_TEST_CASE(group_and_direct_feature_ids_do_not_collide)
{
    BOOST_TEST(FeatureTreeNodeIds::group(0) != FeatureTreeNodeIds::feature(0, -1, 0));
    BOOST_TEST(FeatureTreeNodeIds::group(1) != FeatureTreeNodeIds::feature(0, -1, 1));
    BOOST_TEST(FeatureTreeNodeIds::group(9) != FeatureTreeNodeIds::feature(0, -1, 9));
}

BOOST_AUTO_TEST_CASE(subgroup_and_feature_ids_do_not_collide)
{
    BOOST_TEST(FeatureTreeNodeIds::subGroup(0, 0) != FeatureTreeNodeIds::feature(0, 0, 0));
    BOOST_TEST(FeatureTreeNodeIds::subGroup(0, 1) != FeatureTreeNodeIds::feature(0, 1, 0));
    BOOST_TEST(FeatureTreeNodeIds::subGroup(3, 2) != FeatureTreeNodeIds::feature(3, 2, 5));
}

BOOST_AUTO_TEST_CASE(visibility_button_ids_are_distinct_from_tree_nodes)
{
    for (int groupIdx = 0; groupIdx < 10; ++groupIdx)
    {
        const std::string visibilityId = FeatureTreeNodeIds::visibilityButton(groupIdx);
        BOOST_TEST(visibilityId != FeatureTreeNodeIds::group(groupIdx));
        BOOST_TEST(visibilityId != FeatureTreeNodeIds::subGroup(groupIdx, 0));
        BOOST_TEST(visibilityId != FeatureTreeNodeIds::feature(groupIdx, -1, 0));
    }
}
