/**
 * @file FeatureTreeNodeIds.h
 * @brief Stable ImGui identifier helpers for the feature tree.
 */
#pragma once

#include <string>

namespace FeatureTreeNodeIds
{
/**
 * @brief Builds the ImGui ID for a feature group node.
 * @param groupIdx Zero-based group index.
 * @return Stable string ID for the group node.
 */
inline std::string group(int groupIdx)
{
    return "feature_group:" + std::to_string(groupIdx);
}

/**
 * @brief Builds the ImGui ID for a sub-group node.
 * @param groupIdx Zero-based group index.
 * @param subGroupIdx Zero-based sub-group index.
 * @return Stable string ID for the sub-group node.
 */
inline std::string subGroup(int groupIdx, int subGroupIdx)
{
    return group(groupIdx) + "/sub_group:" + std::to_string(subGroupIdx);
}

/**
 * @brief Builds the ImGui ID for a feature node.
 * @param groupIdx Zero-based group index.
 * @param subGroupIdx Zero-based sub-group index, or -1 for direct features.
 * @param featureIdx Zero-based feature index.
 * @return Stable string ID for the feature node.
 */
inline std::string feature(int groupIdx, int subGroupIdx, int featureIdx)
{
    return group(groupIdx) + "/sub_group:" + std::to_string(subGroupIdx)
           + "/feature:" + std::to_string(featureIdx);
}

/**
 * @brief Builds the ImGui ID for a group visibility toggle button.
 * @param groupIdx Zero-based group index.
 * @return Stable string ID for the visibility button.
 */
inline std::string visibilityButton(int groupIdx)
{
    return group(groupIdx) + "/visibility";
}
} // namespace FeatureTreeNodeIds
