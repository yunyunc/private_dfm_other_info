# IFR特征识别结果JSON格式解析指南

**版本**: v0.7.3
**创建日期**: 2025-10-28
**最后更新**: 2025-12-01

---

## 目录

- [1. 概述](#1-概述)
- [2. 顶层结构](#2-顶层结构)
- [3. 零件信息 (parts)](#3-零件信息-parts)
- [4. 特征识别部分 (featureRecognition)](#4-特征识别部分-featurerecognition)
- [5. 特征组类型详解 (featureGroups)](#5-特征组类型详解-featuregroups)
    - [5.1 加工面特征 (Machining Face Features)](#51-加工面特征-machining-face-features)
    - [5.2 孔特征 (Hole Features)](#52-孔特征-hole-features)
    - [5.3 型腔特征 (Pocket Features)](#53-型腔特征-pocket-features)
    - [5.4 外轮廓特征 (Profile Features)](#54-外轮廓特征-profile-features)
    - [5.5 倒角特征 (Chamfer Features)](#55-倒角特征-chamfer-features)
    - [5.6 圆角特征 (Fillet Features)](#56-圆角特征-fillet-features)
- [6. 参数系统说明](#6-参数系统说明)
- [7. 面ID系统 (shapeIDs)](#7-面id系统-shapeids)
- [8. 颜色编码规范](#8-颜色编码规范)
- [9. 解析示例代码](#9-解析示例代码)
- [10. 最佳实践](#10-最佳实践)
- [11. 常见问题 (FAQ)](#11-常见问题-faq)
- [12. 附录](#12-附录)

---

## 1. 概述

### 1.1 用途和目标

IFR (Interactive Feature Recognition) 特征识别结果JSON格式是一种标准化的数据交换格式，用于表示CNC机械加工零件的特征识别结果。该格式设计用于：

- **特征可视化**: 为CAM软件、CAD软件和可视化工具提供特征数据
- **工艺规划**: 支持自动化工艺路线生成和刀具路径规划
- **数据交换**: 在不同系统之间传递特征识别结果
- **结果验证**: 便于人工审核和验证识别结果的正确性

### 1.2 适用场景

- CNC铣削加工特征识别
- 机械零件的特征提取和分析
- CAM工艺规划的前端输入
- 特征库构建和机器学习训练数据

### 1.3 版本信息

当前文档基于 JSON 格式版本 `"1"`，对应：

- 序列化器: `CNC_MachiningFaceJsonSerializer`
- 特征类型系统: `CNC_FeatureTypes.h`

---

## 2. 顶层结构

### 2.1 完整的JSON Schema

```json
{
  "version": "string",
  // 格式版本号
  "parts": [
    // 零件数组
    {
      "partId": "string",
      // 零件唯一标识符
      "partName": "string",
      // 零件名称
      "geometricProperties": {
        ...
      },
      // 几何属性
      "process": "string",
      // 加工工艺类型
      "featureRecognition": {
        ...
      }
      // 特征识别结果
    }
  ]
}
```

### 2.2 各字段说明

| 字段        | 类型     | 必需 | 说明                |
|-----------|--------|----|-------------------|
| `version` | string | 是  | JSON格式版本号，当前为 "1" |
| `parts`   | array  | 是  | 零件数组，通常包含一个零件对象   |

### 2.3 数据类型定义

- **字符串数值**: 所有数值（体积、面积、尺寸等）均以字符串形式存储，保留2位小数
- **颜色格式**: RGB颜色使用字符串格式 `"(R, G, B)"`，值域为 0-255
- **坐标格式**: 三维坐标使用对象形式 `{"x": "value", "y": "value", "z": "value"}`

---

## 3. 零件信息 (parts)

### 3.1 零件基本信息

```json
{
  "partId": "part-001",
  // 零件ID
  "partName": "CNC_Part",
  // 零件名称
  "process": "CNC Machining Milling"
  // 加工工艺
}
```

**字段说明**:

- `partId`: 自动生成或用户指定的唯一标识符，格式为 `"part-XXXX"`
- `partName`: 零件名称，默认为 `"CNC_Part"`
- `process`: 固定值 `"CNC Machining Milling"`，表示CNC铣削加工

### 3.2 几何属性详解 (geometricProperties)

#### 3.2.1 体积和表面积

```json
"geometricProperties": {
"volume": "424936.19", // 零件体积 (mm³)
"surfaceArea": "133104.95"    // 零件表面积 (mm²)
}
```

- **volume**: 通过OpenCASCADE的 `BRepGProp::VolumeProperties()` 计算
- **surfaceArea**: 通过 `BRepGProp::SurfaceProperties()` 计算

#### 3.2.2 AABB包围盒 (Axis-Aligned Bounding Box)

```json
"AABB": {
"dimensions": {// 包围盒尺寸
"x": "318.00", // X方向长度 (mm)
"y": "148.00", // Y方向长度 (mm)
"z": "30.00"           // Z方向长度 (mm)
},
"minCorner": {           // 最小角点坐标
"x": "-115.00",
"y": "-74.00",
"z": "-15.00"
},
"maxCorner": {// 最大角点坐标
"x": "203.00",
"y": "74.00",
"z": "15.00"
}
}
```

**计算公式**:

```
dimensions.x = maxCorner.x - minCorner.x
dimensions.y = maxCorner.y - minCorner.y
dimensions.z = maxCorner.z - minCorner.z
```

**用途**:

- 零件尺寸估算
- 工件夹持和定位规划
- 机床行程验证

---

## 4. 特征识别部分 (featureRecognition)

### 4.1 结构概览

```json
"featureRecognition": {
"name": "Feature Recognition", // 固定名称
"totalFeatureCount": "23", // 总特征数量
"featureGroups": [...]           // 特征组数组
}
```

### 4.2 字段说明

- **name**: 固定值 `"Feature Recognition"`
- **totalFeatureCount**: 所有特征组中的特征总数（字符串格式）
- **featureGroups**: 特征组数组，每个组包含同类型的特征

### 4.3 totalFeatureCount 计算

```cpp
// 伪代码示例
int totalFeatures = 0;
for (const auto& group : featureGroups) {
  totalFeatures += group.totalGroupFeatureCount;
}
```

---

## 5. 特征组类型详解 (featureGroups)

### 5.1 加工面特征 (Machining Face Features)

加工面特征表示通过铣削加工产生的各种表面类型。

#### 5.1.1 支持的加工面类型

| 特征名称  | 英文名称                     | 颜色 (RGB)        | 说明       |
|-------|--------------------------|-----------------|----------|
| 平面铣削面 | Flat Face Milled Face(s) | (115, 251, 253) | 平坦的加工表面  |
| 侧面平铣面 | Flat Side Milled Face(s) | (0, 35, 245)    | 侧面的平坦加工面 |
| 曲面铣削面 | Curved Milled Face(s)    | (22, 65, 124)   | 自由曲面加工   |
| 凹圆角边  | Concave Fillet Edge(s)   | (129, 127, 38)  | 凹圆角过渡面   |
| 凸轮廓边  | Convex Profile Edge(s)   | (240, 155, 89)  | 凸轮廓过渡面   |
| 圆形铣削面 | Circular Milled Face(s)  | (150, 150, 150) | 圆柱形加工表面  |

#### 5.1.2 数据结构示例

```json
{
  "name": "Flat Face Milled Face(s)",
  "color": "(115, 251, 253)",
  "totalGroupFeatureCount": "2",
  "featureCount": "2",
  "features": [
    {
      "shapeIDCount": "1",
      "shapeIDs": [
        {
          "id": "138"
        }
      ]
    },
    {
      "shapeIDCount": "1",
      "shapeIDs": [
        {
          "id": "174"
        }
      ]
    }
  ]
}
```

#### 5.1.3 特点

- **无参数模板**: 加工面特征不包含参数信息
- **单面特征**: 每个特征通常对应单个面（shapeIDCount = 1）
- **按类型分组**: 相同类型的加工面归为一组

---

### 5.2 孔特征 (Hole Features)

孔特征是CNC加工中最常见的特征类型，系统采用**二级分组**策略进行组织。

#### 5.2.1 孔特征二级分组逻辑

```
一级分组: 按孔的结构类型分类
  ├─ 圆孔 (Hole_Round)
  ├─ 阶梯孔 (Hole_Stepped)
  └─ 沉孔 (Hole_Countersink)

二级分组: 按孔的贯穿性分类
  ├─ 通孔 (Through)      // IsThrough() == true
  └─ 盲孔 (Blind)        // IsThrough() == false
```

#### 5.2.2 支持的孔类型

| 一级分类 | 二级分类 | 特征名称                        | 颜色 (RGB)        | 说明       |
|------|------|-----------------------------|-----------------|----------|
| 圆孔   | 盲孔   | Blind Hole(s)               | (200, 100, 100) | 不贯穿的圆柱孔  |
| 圆孔   | 通孔   | Through Hole(s)             | (240, 135, 132) | 完全贯穿的圆柱孔 |
| 阶梯孔  | 盲孔   | Stepped Blind Hole(s)       | (150, 0, 90)    | 不贯穿的阶梯孔  |
| 阶梯孔  | 通孔   | Stepped Through Hole(s)     | (204, 0, 125)   | 贯穿的阶梯孔   |
| 沉孔   | 盲孔   | Countersink Blind Hole(s)   | (150, 0, 90)    | 不贯穿的沉孔   |
| 沉孔   | 通孔   | Countersink Through Hole(s) | (204, 0, 125)   | 贯穿的沉孔    |

#### 5.2.3 IsThrough() 判断机制

`determineIsThrough()` 函数通过以下方式判断孔是否为通孔：

```cpp
bool determineIsThrough(const Handle(CNC_Feature)& feature) const
{
  // 1. 尝试转换为 CNC_HoleFeature (基础圆孔)
  Handle(CNC_HoleFeature) holeFeature = Handle(CNC_HoleFeature)::DownCast(feature);
  if (!holeFeature.IsNull())
    return holeFeature->IsThrough();

  // 2. 尝试转换为 CNC_SteppedHoleFeature (阶梯孔)
  Handle(CNC_SteppedHoleFeature) steppedHole = Handle(CNC_SteppedHoleFeature)::DownCast(feature);
  if (!steppedHole.IsNull())
    return steppedHole->IsThrough();

  // 3. 尝试转换为 CNC_CountersinkHoleFeature (沉孔)
  Handle(CNC_CountersinkHoleFeature) countersinkHole = Handle(CNC_CountersinkHoleFeature)::DownCast(feature);
  if (!countersinkHole.IsNull())
    return countersinkHole->IsThrough();

  // 4. 默认返回 false (保守策略)
  return false;
}
```

**判断标准**:

- 通孔: 孔的特征从零件一侧完全贯穿到另一侧
- 盲孔: 孔的特征有底面，未完全贯穿零件

#### 5.2.4 数据结构示例

**盲孔示例** (带参数的subGroup结构):

```json
{
  "name": "Blind Hole(s)",
  "color": "(200, 100, 100)",
  "totalGroupFeatureCount": "7",
  "subGroupCount": "1",
  "subGroups": [
    {
      "parametersCount": "3",
      "parameters": [
        {
          "name": "Radius",
          "units": "mm",
          "value": "5.000000"
        },
        {
          "name": "Depth",
          "units": "mm",
          "value": "18.000000"
        },
        {
          "name": "Axis",
          "units": "",
          "value": "(0.00, 0.00, 1.00)"
        }
      ],
      "featureCount": "7",
      "features": [
        {
          "shapeIDCount": "4",
          "shapeIDs": [
            {
              "id": "8"
            },
            {
              "id": "4"
            },
            {
              "id": "24"
            },
            {
              "id": "25"
            }
          ]
        }
        // ... 其他6个孔特征
      ]
    }
  ]
}
```

#### 5.2.5 孔特征参数说明

| 参数名    | 单位 | 说明   | 计算方式               |
|--------|----|------|--------------------|
| Radius | mm | 孔的半径 | `Diameter / 2.0`   |
| Depth  | mm | 孔的深度 | 从孔开口到底面的距离         |
| Axis   | 无  | 孔的轴向 | 单位方向向量 `(X, Y, Z)` |

**注意事项**:

1. 参数采用**模板机制**: 取第一个孔的参数作为整组的参数模板
2. 同组内的孔应具有相同的参数（半径、深度等）
3. Axis表示孔的方向，通常垂直于孔开口面

#### 5.2.6 孔特征的面数量

孔特征通常包含多个面：

- **圆柱盲孔**: 3-4个面（圆柱面 + 底面 + 可能的倒角面）
- **圆柱通孔**: 1-2个面（圆柱面 + 可能的倒角面）
- **阶梯孔**: 5个以上面（多段圆柱面 + 底面/过渡面）
- **沉孔**: 4-5个面（圆柱面 + 锥面/沉孔台阶面 + 底面等）

#### 5.2.7 沉头孔的特殊数据结构

沉头孔(Countersink Hole)可能采用不带参数模板的简化结构：

```json
{
  "name": "Countersink Blind Hole(s)",
  "color": "(150, 0, 90)",
  "totalGroupFeatureCount": "3",
  "featureCount": "3",
  "features": [
    {
      "shapeIDCount": "4",
      "shapeIDs": [
        {"id": "69"},
        {"id": "68"},
        {"id": "70"},
        {"id": "71"}
      ]
    },
    {
      "shapeIDCount": "4",
      "shapeIDs": [
        {"id": "73"},
        {"id": "72"},
        {"id": "74"},
        {"id": "75"}
      ]
    },
    {
      "shapeIDCount": "4",
      "shapeIDs": [
        {"id": "77"},
        {"id": "76"},
        {"id": "78"},
        {"id": "79"}
      ]
    }
  ]
}
```

**注意事项**:

- 当沉头孔参数各异时，可能不使用 `subGroups` 结构
- 此时 `featureCount` 和 `features` 直接位于组级别
- 不包含统一的参数模板（因为各孔参数可能不同）

---

### 5.3 型腔特征 (Pocket Features)

型腔特征表示通过铣削加工产生的凹陷区域。

#### 5.3.1 型腔类型分类

| 类型      | 特征名称              | 颜色 (RGB)       | 说明          |
|---------|-------------------|----------------|-------------|
| Closed  | Closed Pocket(s)  | (81, 20, 0)    | 封闭型腔，四周有壁   |
| Open    | Open Pocket(s)    | (120, 60, 30)  | 开口型腔，至少一侧开口 |
| Through | Through Pocket(s) | (160, 100, 60) | 通槽，完全贯穿零件   |

#### 5.3.2 数据结构示例

```json
{
  "name": "Closed Pocket(s)",
  "color": "(81, 20, 0)",
  "totalGroupFeatureCount": "6",
  "subGroupCount": "1",
  "subGroups": [
    {
      "parametersCount": "4",
      "parameters": [
        {
          "name": "Length",
          "units": "mm",
          "value": "92.139864"
        },
        {
          "name": "Width",
          "units": "mm",
          "value": "137.000000"
        },
        {
          "name": "Depth",
          "units": "mm",
          "value": "20.000000"
        },
        {
          "name": "Axis",
          "units": "",
          "value": "(0.00, 0.00, -1.00)"
        }
      ],
      "featureCount": "6",
      "features": [
        {
          "shapeIDCount": "23",
          "shapeIDs": [
            ...
          ]
        }
        // ... 其他型腔
      ]
    }
  ]
}
```

#### 5.3.3 型腔参数说明

| 参数名    | 单位 | 说明      | 适用类型         |
|--------|----|---------|--------------|
| Length | mm | 型腔Y方向长度 | All          |
| Width  | mm | 型腔X方向宽度 | All          |
| Depth  | mm | 型腔深度    | Closed, Open |
| Axis   | 无  | 型腔加工方向  | All          |

**特殊规则**:

- **Through Pocket** 不显示 `Depth` 参数（因为贯穿整个零件）
- Length和Width从包围盒 (Bounding Box) 计算得出

#### 5.3.4 型腔特征的面数量

型腔通常包含大量的面：

- **Closed Pocket**: 12-23个面（底面 + 侧壁面 + 圆角过渡面）
- **Open Pocket**: 2-6个面（部分壁面 + 底面）
- **Through Pocket**: 12个以上面（侧壁面 + 圆角过渡面，无底面）

#### 5.3.5 型腔特征的附加信息字段

型腔特征可能包含额外的附加信息字段，用于标识型腔内部的特殊面：

```json
{
  "shapeIDCount": "27",
  "shapeIDs": [
    {"id": "40"},
    {"id": "33"},
    // ... 其他面ID
  ],
  "innerFilletsIDs": ["20", "21", "22", "23", "24", "25", "26", "27", "28", "29", "30", "31", "32"],
  "bossesIDs": ["42"]
}
```

**字段说明**:

| 字段名            | 类型           | 说明                           |
|----------------|--------------|------------------------------|
| innerFilletsIDs | string array | 型腔内部圆角面的ID列表，这些面属于型腔内部的圆角过渡面 |
| bossesIDs      | string array | 型腔内部凸台面的ID列表，这些面属于型腔底部或侧壁的凸台 |

**用途**:

1. **内部圆角识别**: `innerFilletsIDs` 帮助区分型腔的主体面和圆角过渡面
2. **凸台识别**: `bossesIDs` 标识型腔内部的凸台特征（如定位柱、加强筋等）
3. **工艺规划**: 圆角需要使用圆角刀具，凸台可能需要额外的加工路径
4. **特征分层**: 便于分层加工时识别不同类型的面

**注意事项**:

- 这些字段是可选的，仅在型腔包含相应特征时才会出现
- `innerFilletsIDs` 和 `bossesIDs` 中的面ID同时也包含在 `shapeIDs` 中
- 一个型腔可以同时包含内部圆角和凸台

---

### 5.4 外轮廓特征 (Profile Features)

外轮廓特征表示零件的外部轮廓面。

#### 5.4.1 数据结构

```json
{
  "name": "Profile Feature(s)",
  "color": "(0, 200, 100)",
  "totalGroupFeatureCount": "1",
  "featureCount": "1",
  "features": [
    {
      "shapeIDCount": "18",
      "shapeIDs": [
        {
          "id": "1"
        },
        {
          "id": "2"
        },
        {
          "id": "3"
        }
        // ... 其他面ID
      ]
    }
  ]
}
```

#### 5.4.2 特点

- **无参数**: Profile特征不包含参数信息
- **单组结构**: 所有外轮廓面归为一组，不再细分
- **多面特征**: 通常包含大量面（外表面、倒角、圆角等）
- **固定颜色**: 使用青绿色 `(0, 200, 100)`

---

### 5.5 倒角特征 (Chamfer Features)

倒角特征表示零件边缘的倒角加工面，用于去除锐边、便于装配或改善外观。

#### 5.5.1 数据结构示例

```json
{
  "name": "Chamfer(s)",
  "color": "(128, 0, 128)",
  "totalGroupFeatureCount": "1",
  "subGroupCount": "1",
  "subGroups": [
    {
      "parametersCount": "4",
      "parameters": [
        {
          "name": "Width",
          "units": "mm",
          "value": "1.562482"
        },
        {
          "name": "Angle",
          "units": "deg",
          "value": "45.000000"
        },
        {
          "name": "ChamferType",
          "units": "",
          "value": "Hybrid"
        },
        {
          "name": "Confidence",
          "units": "",
          "value": "0.500000"
        }
      ],
      "featureCount": "1",
      "features": [
        {
          "shapeIDCount": "5",
          "shapeIDs": [
            {"id": "1"},
            {"id": "2"},
            {"id": "3"},
            {"id": "4"},
            {"id": "5"}
          ],
          "supportInfos": [
            {"faceId": "1", "supportFaceId1": "9", "supportFaceId2": "8"},
            {"faceId": "2", "supportFaceId1": "9", "supportFaceId2": "44"},
            {"faceId": "3", "supportFaceId1": "9", "supportFaceId2": "54"},
            {"faceId": "4", "supportFaceId1": "9", "supportFaceId2": "6"},
            {"faceId": "5", "supportFaceId1": "7", "supportFaceId2": "9"}
          ]
        }
      ]
    }
  ]
}
```

#### 5.5.2 倒角参数说明

| 参数名         | 单位  | 说明                            | 取值范围/示例                       |
|-------------|-----|-------------------------------|-------------------------------|
| Width       | mm  | 倒角宽度（斜面在一个支撑面上的投影长度）        | 正数或0，如 `1.562482`             |
| Angle       | deg | 倒角角度（相对于支撑面的夹角）              | 通常为 `45.000000`               |
| ChamferType | 无   | 倒角类型分类                        | `Edge`、`Vertex`、`Hybrid`、`Planar` |
| Confidence  | 无   | 识别置信度（0-1之间，表示识别结果的可信程度）    | `0.0` ~ `1.0`                 |

**倒角类型说明**:

- **Edge**: 边倒角，沿单条边进行的倒角
- **Vertex**: 顶点倒角，在顶点位置形成的倒角
- **Hybrid**: 混合倒角，包含边倒角和顶点倒角的组合
- **Planar**: 平面倒角，形成平面斜切的倒角（通常置信度较高）

#### 5.5.3 supportInfos 字段说明

`supportInfos` 是倒角特征特有的字段，用于记录每个倒角面与其相邻支撑面的关系。

```json
"supportInfos": [
  {
    "faceId": "1",           // 倒角面的ID
    "supportFaceId1": "9",   // 第一个支撑面ID（通常是主平面）
    "supportFaceId2": "8"    // 第二个支撑面ID（相邻面）
  }
]
```

**字段说明**:

| 字段名           | 类型     | 说明                      |
|---------------|--------|-------------------------|
| faceId        | string | 倒角面本身的面ID               |
| supportFaceId1 | string | 第一个支撑面ID（倒角依附的主面）      |
| supportFaceId2 | string | 第二个支撑面ID（倒角连接的相邻面）     |

**用途**:

1. **几何重建**: 根据支撑面关系重建倒角的原始边
2. **工艺规划**: 确定倒角刀具的进刀方向和参考面
3. **特征编辑**: 修改倒角参数时保持与支撑面的几何约束

#### 5.5.4 特点

- **带参数模板**: 使用 `subGroups` 结构包含参数信息
- **支撑面信息**: 独有的 `supportInfos` 字段记录几何关系
- **置信度评分**: 提供识别结果的可信程度
- **固定颜色**: 使用紫色 `(128, 0, 128)`

---

### 5.6 圆角特征 (Fillet Features)

圆角特征表示零件边缘或面之间的圆弧过渡面，用于消除锐边、减少应力集中或改善外观。

#### 5.6.1 数据结构示例

```json
{
  "name": "Fillet(s)",
  "color": "(0, 191, 255)",
  "totalGroupFeatureCount": "1",
  "subGroupCount": "1",
  "subGroups": [
    {
      "parametersCount": "2",
      "parameters": [
        {
          "name": "Radius",
          "units": "mm",
          "value": "1.000000"
        },
        {
          "name": "IsConvex",
          "units": "",
          "value": "true"
        }
      ],
      "featureCount": "1",
      "features": [
        {
          "shapeIDCount": "10",
          "shapeIDs": [
            {"id": "22"},
            {"id": "23"},
            {"id": "25"},
            {"id": "27"},
            {"id": "29"},
            {"id": "31"},
            {"id": "30"},
            {"id": "28"},
            {"id": "26"},
            {"id": "24"}
          ]
        }
      ]
    }
  ]
}
```

#### 5.6.2 圆角参数说明

| 参数名      | 单位 | 说明                          | 取值范围/示例              |
|----------|----|-----------------------------|----------------------|
| Radius   | mm | 圆角半径                        | 正数，如 `1.000000`      |
| IsConvex | 无  | 是否为凸圆角（外圆角）                 | `"true"` 或 `"false"` |

**凸凹圆角区分**:

- **凸圆角 (Convex Fillet)**: `IsConvex = "true"`
  - 外圆角，位于零件外部边缘
  - 圆角面向外凸出
  - 典型应用：零件外边缘的倒圆

- **凹圆角 (Concave Fillet)**: `IsConvex = "false"`
  - 内圆角，位于零件内部角落
  - 圆角面向内凹陷
  - 典型应用：型腔内角的圆角过渡

#### 5.6.3 圆角与倒角的区别

| 特性   | 圆角 (Fillet)        | 倒角 (Chamfer)        |
|------|---------------------|---------------------|
| 几何形状 | 圆弧过渡面               | 平面斜切面               |
| 主要参数 | 半径 (Radius)         | 宽度 (Width)、角度 (Angle) |
| 加工方式 | 圆角刀具                | 倒角刀具或端铣刀            |
| 应用场景 | 减少应力集中、改善流体动力学      | 便于装配、去除毛刺           |
| JSON标识 | `Fillet(s)`         | `Chamfer(s)`        |

#### 5.6.4 特点

- **带参数模板**: 使用 `subGroups` 结构包含参数信息
- **凸凹区分**: 通过 `IsConvex` 参数区分外圆角和内圆角
- **多面特征**: 连续的圆角可能包含多个圆角面
- **固定颜色**: 使用深天蓝色 `(0, 191, 255)`

#### 5.6.5 圆角特征的面数量

圆角特征的面数量取决于圆角的几何复杂度：

- **简单边圆角**: 1个面（单条边的圆角）
- **连续边圆角**: 多个面（多条相连边的圆角）
- **角点圆角**: 包含球面过渡的额外面

**示例**: 一个矩形型腔的四个内角圆角可能包含4-8个面（4个圆柱面 + 可能的球面过渡）

---

## 6. 参数系统说明

### 6.1 参数模板机制

特征组的参数采用**模板机制**：

1. 提取组内第一个特征的参数
2. 将这些参数作为整组的参数模板
3. 假设同组内所有特征具有相同参数

### 6.2 参数结构

```json
"parameters": [
{
"name": "Radius", // 参数名称
"units": "mm", // 参数单位
"value": "5.000000"         // 参数值 (字符串格式)
}
]
```

### 6.3 参数数组对应关系

```json
{
  "parametersCount": "3",
  "parameters": [
    {
      "name": "Radius",
      "units": "mm",
      "value": "5.000000"
    },
    {
      "name": "Depth",
      "units": "mm",
      "value": "18.000000"
    },
    {
      "name": "Axis",
      "units": "",
      "value": "(0.00, 0.00, 1.00)"
    }
  ]
}
```

**注意**:

- `parametersCount` 表示参数数量
- 三个数组（name, units, value）长度必须一致
- 数组索引一一对应

### 6.4 特殊参数格式

#### 6.4.1 Axis (轴向参数)

```json
{
  "name": "Axis",
  "units": "",
  // 无单位
  "value": "(0.00, 0.00, 1.00)"
  // 单位向量字符串
}
```

格式: `(X, Y, Z)`，保留2位小数，表示单位方向向量。

#### 6.4.2 数值参数

```json
{
  "name": "Radius",
  "units": "mm",
  "value": "5.000000"
  // 保留6位小数
}
```

---

## 7. 面ID系统 (shapeIDs)

### 7.1 面ID的含义

面ID (Shape ID) 是CAD模型中拓扑面 (Face) 的唯一整数标识符，用于：

- 将特征映射回原始CAD模型
- 支持特征高亮显示
- 实现特征与几何的关联

### 7.2 数据结构

```json
{
  "shapeIDCount": "4",
  // 该特征包含的面数量
  "shapeIDs": [
    {
      "id": "8"
    },
    {
      "id": "4"
    },
    {
      "id": "24"
    },
    {
      "id": "25"
    }
  ]
}
```

### 7.3 面ID的生成

面ID通过OpenCASCADE的 `TopTools_IndexedMapOfShape` 生成：

```cpp
// 伪代码示例
TopTools_IndexedMapOfShape faceMap;
TopExp::MapShapes(shape, TopAbs_FACE, faceMap);

// 面ID = faceMap中的索引 (从1开始)
int faceId = faceMap.FindIndex(face);
```

### 7.4 与CAD模型的映射

```cpp
// 根据face ID反查face对象
TopoDS_Face face = TopoDS::Face(faceMap(faceId));
```

---

## 8. 颜色编码规范

### 8.1 完整颜色定义表

| 特征类型                     | RGB颜色           | 十六进制    | 说明  |
|--------------------------|-----------------|---------|-----|
| **加工面特征**                |                 |         |     |
| Flat Face Milled         | (115, 251, 253) | #73FBFD | 青色系 |
| Flat Side Milled         | (0, 35, 245)    | #0023F5 | 蓝色系 |
| Curved Milled            | (22, 65, 124)   | #16417C | 深蓝色 |
| Concave Fillet Edge      | (129, 127, 38)  | #817F26 | 黄绿色 |
| Convex Profile Edge      | (240, 155, 89)  | #F09B59 | 橙色系 |
| Circular Milled          | (150, 150, 150) | #969696 | 灰色  |
| **孔特征**                  |                 |         |     |
| Blind Hole               | (200, 100, 100) | #C86464 | 深粉色 |
| Through Hole             | (240, 135, 132) | #F08784 | 浅粉色 |
| Stepped Blind Hole       | (150, 0, 90)    | #96005A | 深品红 |
| Stepped Through Hole     | (204, 0, 125)   | #CC007D | 品红色 |
| Countersink Blind Hole   | (150, 0, 90)    | #96005A | 深品红 |
| Countersink Through Hole | (204, 0, 125)   | #CC007D | 品红色 |
| **型腔特征**                 |                 |         |     |
| Closed Pocket            | (81, 20, 0)     | #511400 | 深棕色 |
| Open Pocket              | (120, 60, 30)   | #783C1E | 棕色  |
| Through Pocket           | (160, 100, 60)  | #A0643C | 浅棕色 |
| **外轮廓特征**                |                 |         |     |
| Profile Feature          | (0, 200, 100)   | #00C864 | 青绿色 |
| **倒角特征**                  |                 |         |     |
| Chamfer                  | (128, 0, 128)   | #800080 | 紫色  |
| **圆角特征**                  |                 |         |     |
| Fillet                   | (0, 191, 255)   | #00BFFF | 深天蓝色 |

### 8.2 颜色编码原则

1. **同类特征色系一致**:
    - 孔特征使用粉红/品红色系
    - 型腔特征使用棕色系
    - 加工面特征使用多样化色系便于区分

2. **通孔vs盲孔**:
    - 通孔颜色更浅、更鲜艳
    - 盲孔颜色更深、更暗

3. **视觉区分度**: 确保相邻特征类型颜色差异明显

### 8.3 自定义颜色映射

```cpp
// C++ 代码示例
CNC_MachiningFaceJsonSerializer serializer;

std::map<CNC_MachiningFaceType, std::string> customColors;
customColors[CNC_MachiningFaceType::FlatFaceMilled] = "(255, 0, 0)";  // 红色

serializer.SetColorMapping(customColors);
```

---

## 9. 解析示例代码

### 9.1 Python 解析示例

```python
import json

def parse_ifr_json(file_path):
    """解析IFR特征识别结果JSON文件"""

    with open(file_path, 'r', encoding='utf-8') as f:
        data = json.load(f)

    # 1. 提取版本信息
    version = data.get('version')
    print(f"JSON版本: {version}")

    # 2. 遍历零件
    for part in data.get('parts', []):
        part_id = part.get('partId')
        part_name = part.get('partName')
        print(f"\n零件: {part_name} (ID: {part_id})")

        # 3. 提取几何属性
        geom_props = part.get('geometricProperties', {})
        volume = float(geom_props.get('volume', 0))
        surface_area = float(geom_props.get('surfaceArea', 0))
        print(f"  体积: {volume:.2f} mm³")
        print(f"  表面积: {surface_area:.2f} mm²")

        # 4. 提取AABB包围盒
        aabb = geom_props.get('AABB', {})
        dimensions = aabb.get('dimensions', {})
        print(f"  尺寸: {dimensions.get('x')} × {dimensions.get('y')} × {dimensions.get('z')} mm")

        # 5. 遍历特征识别结果
        feature_rec = part.get('featureRecognition', {})
        total_features = int(feature_rec.get('totalFeatureCount', 0))
        print(f"\n  总特征数: {total_features}")

        feature_groups = feature_rec.get('featureGroups', [])
        for group in feature_groups:
            parse_feature_group(group)

def parse_feature_group(group):
    """解析特征组"""

    name = group.get('name')
    color = group.get('color')
    total_count = int(group.get('totalGroupFeatureCount', 0))

    print(f"\n  [{name}] - {total_count} 个特征")
    print(f"    颜色: {color}")

    # 检查是否有subGroups
    sub_groups = group.get('subGroups', [])
    if sub_groups:
        for sub_group in sub_groups:
            parse_sub_group(sub_group)
    else:
        # 无subGroups，直接解析features
        features = group.get('features', [])
        for i, feature in enumerate(features):
            face_count = int(feature.get('shapeIDCount', 0))
            shape_ids = [sid.get('id') for sid in feature.get('shapeIDs', [])]
            print(f"    特征 {i+1}: {face_count} 个面, IDs: {shape_ids[:5]}...")

def parse_sub_group(sub_group):
    """解析子组（带参数的组）"""

    parameters = sub_group.get('parameters', [])
    feature_count = int(sub_group.get('featureCount', 0))

    print(f"    参数:")
    for param in parameters:
        name = param.get('name')
        units = param.get('units')
        value = param.get('value')
        unit_str = f" {units}" if units else ""
        print(f"      {name}: {value}{unit_str}")

    print(f"    特征数量: {feature_count}")

    # 解析特征
    features = sub_group.get('features', [])
    for i, feature in enumerate(features):
        face_count = int(feature.get('shapeIDCount', 0))
        shape_ids = [sid.get('id') for sid in feature.get('shapeIDs', [])]
        print(f"      特征 {i+1}: {face_count} 个面, IDs: {shape_ids}")

# 使用示例
if __name__ == "__main__":
    parse_ifr_json("tongtaiEx02_feature_recognition_result.json")
```

### 9.2 C++ 解析示例

```cpp
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>

using json = nlohmann::json;

// 特征组信息结构体
struct FeatureGroupInfo {
    std::string name;
    std::string color;
    int totalGroupFeatureCount;
    std::vector<std::string> parameters;
    std::vector<std::vector<int>> faceIds;
};

// 解析JSON文件
void parseIFRJson(const std::string& filePath) {
    // 1. 读取JSON文件
    std::ifstream file(filePath);
    json data = json::parse(file);

    // 2. 提取版本信息
    std::string version = data["version"];
    std::cout << "JSON版本: " << version << std::endl;

    // 3. 遍历零件
    for (const auto& part : data["parts"]) {
        std::string partId = part["partId"];
        std::string partName = part["partName"];
        std::cout << "\n零件: " << partName << " (ID: " << partId << ")" << std::endl;

        // 4. 提取几何属性
        const auto& geomProps = part["geometricProperties"];
        double volume = std::stod(geomProps["volume"].get<std::string>());
        double surfaceArea = std::stod(geomProps["surfaceArea"].get<std::string>());

        std::cout << "  体积: " << volume << " mm³" << std::endl;
        std::cout << "  表面积: " << surfaceArea << " mm²" << std::endl;

        // 5. 遍历特征组
        const auto& featureRec = part["featureRecognition"];
        int totalFeatures = std::stoi(featureRec["totalFeatureCount"].get<std::string>());
        std::cout << "\n  总特征数: " << totalFeatures << std::endl;

        for (const auto& group : featureRec["featureGroups"]) {
            parseFeatureGroup(group);
        }
    }
}

// 解析特征组
void parseFeatureGroup(const json& group) {
    std::string name = group["name"];
    std::string color = group["color"];
    int totalCount = std::stoi(group["totalGroupFeatureCount"].get<std::string>());

    std::cout << "\n  [" << name << "] - " << totalCount << " 个特征" << std::endl;
    std::cout << "    颜色: " << color << std::endl;

    // 检查是否有subGroups
    if (group.contains("subGroups")) {
        for (const auto& subGroup : group["subGroups"]) {
            // 解析参数
            std::cout << "    参数:" << std::endl;
            for (const auto& param : subGroup["parameters"]) {
                std::string paramName = param["name"];
                std::string units = param["units"];
                std::string value = param["value"];

                std::cout << "      " << paramName << ": " << value;
                if (!units.empty()) {
                    std::cout << " " << units;
                }
                std::cout << std::endl;
            }

            // 解析特征
            parseFeatures(subGroup["features"]);
        }
    } else {
        // 无subGroups，直接解析features
        parseFeatures(group["features"]);
    }
}

// 解析特征列表
void parseFeatures(const json& features) {
    int index = 1;
    for (const auto& feature : features) {
        int faceCount = std::stoi(feature["shapeIDCount"].get<std::string>());

        std::vector<int> faceIds;
        for (const auto& shapeId : feature["shapeIDs"]) {
            faceIds.push_back(std::stoi(shapeId["id"].get<std::string>()));
        }

        std::cout << "      特征 " << index++ << ": " << faceCount << " 个面, IDs: ";
        for (size_t i = 0; i < std::min(faceIds.size(), size_t(5)); ++i) {
            std::cout << faceIds[i] << " ";
        }
        if (faceIds.size() > 5) {
            std::cout << "...";
        }
        std::cout << std::endl;
    }
}
```

### 9.3 JavaScript 解析示例

```javascript
// 读取并解析JSON文件
async function parseIFRJson(filePath) {
    const response = await fetch(filePath);
    const data = await response.json();

    // 提取版本信息
    console.log(`JSON版本: ${data.version}`);

    // 遍历零件
    for (const part of data.parts) {
        console.log(`\n零件: ${part.partName} (ID: ${part.partId})`);

        // 几何属性
        const geomProps = part.geometricProperties;
        console.log(`  体积: ${geomProps.volume} mm³`);
        console.log(`  表面积: ${geomProps.surfaceArea} mm²`);

        // AABB包围盒
        const dimensions = geomProps.AABB.dimensions;
        console.log(`  尺寸: ${dimensions.x} × ${dimensions.y} × ${dimensions.z} mm`);

        // 特征识别
        const featureRec = part.featureRecognition;
        console.log(`\n  总特征数: ${featureRec.totalFeatureCount}`);

        for (const group of featureRec.featureGroups) {
            parseFeatureGroup(group);
        }
    }
}

// 解析特征组
function parseFeatureGroup(group) {
    console.log(`\n  [${group.name}] - ${group.totalGroupFeatureCount} 个特征`);
    console.log(`    颜色: ${group.color}`);

    // 检查subGroups
    if (group.subGroups) {
        for (const subGroup of group.subGroups) {
            console.log('    参数:');
            for (const param of subGroup.parameters) {
                const unit = param.units ? ` ${param.units}` : '';
                console.log(`      ${param.name}: ${param.value}${unit}`);
            }

            parseFeatures(subGroup.features);
        }
    } else {
        parseFeatures(group.features);
    }
}

// 解析特征列表
function parseFeatures(features) {
    features.forEach((feature, index) => {
        const faceIds = feature.shapeIDs.map(sid => sid.id);
        const preview = faceIds.slice(0, 5).join(', ');
        const ellipsis = faceIds.length > 5 ? '...' : '';
        console.log(`      特征 ${index + 1}: ${feature.shapeIDCount} 个面, IDs: ${preview}${ellipsis}`);
    });
}

// 使用示例
parseIFRJson('tongtaiEx02_feature_recognition_result.json');
```

---

## 10. 最佳实践

### 10.1 验证JSON格式

#### 10.1.1 基本结构验证

```python
def validate_ifr_json(data):
    """验证IFR JSON的基本结构"""

    # 检查顶层字段
    assert 'version' in data, "缺少version字段"
    assert 'parts' in data, "缺少parts字段"
    assert isinstance(data['parts'], list), "parts必须是数组"

    for part in data['parts']:
        # 检查零件必需字段
        assert 'partId' in part, "零件缺少partId"
        assert 'partName' in part, "零件缺少partName"
        assert 'geometricProperties' in part, "零件缺少几何属性"
        assert 'featureRecognition' in part, "零件缺少特征识别结果"

        # 验证特征组
        feature_rec = part['featureRecognition']
        assert 'totalFeatureCount' in feature_rec
        assert 'featureGroups' in feature_rec

        # 验证特征数量一致性
        total_from_groups = sum(
            int(group['totalGroupFeatureCount'])
            for group in feature_rec['featureGroups']
        )
        total_declared = int(feature_rec['totalFeatureCount'])
        assert total_from_groups == total_declared, \
            f"特征数量不匹配: {total_from_groups} vs {total_declared}"

    print("✓ JSON格式验证通过")
```

#### 10.1.2 参数完整性验证

```python
def validate_parameters(sub_group):
    """验证参数的完整性"""

    parameters = sub_group.get('parameters', [])
    param_count = int(sub_group.get('parametersCount', 0))

    # 检查参数数量
    assert len(parameters) == param_count, \
        f"参数数量不匹配: {len(parameters)} vs {param_count}"

    # 检查每个参数的字段
    for param in parameters:
        assert 'name' in param, "参数缺少name字段"
        assert 'units' in param, "参数缺少units字段"
        assert 'value' in param, "参数缺少value字段"
```

### 10.2 错误处理建议

#### 10.2.1 缺失字段处理

```python
def safe_get(data, *keys, default=None):
    """安全获取嵌套字典值"""
    result = data
    for key in keys:
        if isinstance(result, dict) and key in result:
            result = result[key]
        else:
            return default
    return result

# 使用示例
volume = safe_get(data, 'parts', 0, 'geometricProperties', 'volume', default='0')
```

#### 10.2.2 类型转换错误处理

```python
def safe_float(value, default=0.0):
    """安全转换为浮点数"""
    try:
        return float(value)
    except (ValueError, TypeError):
        return default

def safe_int(value, default=0):
    """安全转换为整数"""
    try:
        return int(value)
    except (ValueError, TypeError):
        return default

# 使用示例
volume = safe_float(geom_props.get('volume'))
feature_count = safe_int(group.get('totalGroupFeatureCount'))
```

### 10.3 性能优化建议

#### 10.3.1 流式解析大文件

```python
import ijson

def parse_large_json(file_path):
    """流式解析大型JSON文件"""

    with open(file_path, 'rb') as f:
        # 流式解析特征组
        feature_groups = ijson.items(f, 'parts.item.featureRecognition.featureGroups.item')

        for group in feature_groups:
            # 逐个处理特征组，避免一次性加载全部数据
            process_feature_group(group)
```

#### 10.3.2 缓存解析结果

```python
from functools import lru_cache
import hashlib

@lru_cache(maxsize=32)
def parse_ifr_json_cached(file_path):
    """缓存JSON解析结果"""

    # 计算文件哈希作为缓存键
    with open(file_path, 'rb') as f:
        file_hash = hashlib.md5(f.read()).hexdigest()

    # 实际解析逻辑
    return parse_ifr_json(file_path)
```

#### 10.3.3 面ID快速查询

```python
def build_face_id_index(data):
    """构建面ID到特征的索引"""

    face_to_feature = {}

    for part in data['parts']:
        for group in part['featureRecognition']['featureGroups']:
            group_name = group['name']

            features = group.get('features', [])
            if not features and 'subGroups' in group:
                features = group['subGroups'][0].get('features', [])

            for feature_idx, feature in enumerate(features):
                for shape_id_obj in feature.get('shapeIDs', []):
                    face_id = int(shape_id_obj['id'])
                    face_to_feature[face_id] = {
                        'group': group_name,
                        'color': group['color'],
                        'feature_index': feature_idx
                    }

    return face_to_feature

# 使用示例
index = build_face_id_index(data)
feature_info = index.get(138)  # 快速查询面138属于哪个特征
```

### 10.4 数据导出建议

#### 10.4.1 导出为CSV

```python
import csv

def export_features_to_csv(data, output_path):
    """导出特征信息为CSV文件"""

    with open(output_path, 'w', newline='', encoding='utf-8') as csvfile:
        writer = csv.writer(csvfile)

        # 写入表头
        writer.writerow([
            'Part ID', 'Part Name', 'Feature Type', 'Feature Index',
            'Face Count', 'Parameters', 'Face IDs'
        ])

        for part in data['parts']:
            part_id = part['partId']
            part_name = part['partName']

            for group in part['featureRecognition']['featureGroups']:
                group_name = group['name']

                features = extract_features_from_group(group)

                for idx, feature in enumerate(features):
                    face_ids = ','.join(sid['id'] for sid in feature['shapeIDs'])
                    params = extract_parameters(group)

                    writer.writerow([
                        part_id, part_name, group_name, idx + 1,
                        feature['shapeIDCount'], params, face_ids
                    ])
```

#### 10.4.2 导出为数据库

```python
import sqlite3

def export_to_database(data, db_path):
    """导出特征信息到SQLite数据库"""

    conn = sqlite3.connect(db_path)
    cursor = conn.cursor()

    # 创建表结构
    cursor.execute('''
        CREATE TABLE IF NOT EXISTS features (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            part_id TEXT,
            part_name TEXT,
            feature_type TEXT,
            color TEXT,
            face_count INTEGER,
            face_ids TEXT,
            parameters TEXT
        )
    ''')

    # 插入数据
    for part in data['parts']:
        part_id = part['partId']
        part_name = part['partName']

        for group in part['featureRecognition']['featureGroups']:
            group_name = group['name']
            color = group['color']

            features = extract_features_from_group(group)
            params = extract_parameters(group)

            for feature in features:
                face_ids = ','.join(sid['id'] for sid in feature['shapeIDs'])

                cursor.execute('''
                    INSERT INTO features (part_id, part_name, feature_type, color,
                                         face_count, face_ids, parameters)
                    VALUES (?, ?, ?, ?, ?, ?, ?)
                ''', (part_id, part_name, group_name, color,
                      int(feature['shapeIDCount']), face_ids, params))

    conn.commit()
    conn.close()
```

---

## 11. 常见问题 (FAQ)

### 11.1 为什么有些孔分为通孔和盲孔？

**原因**: 通孔和盲孔的加工工艺不同：

- **通孔**: 钻头贯穿整个零件，通常工艺较简单
- **盲孔**: 钻头只钻到一定深度，需要控制深度，加工复杂度更高

**意义**: 分开表示有助于：

1. 工艺规划时选择不同的刀具和参数
2. 加工时间和成本估算
3. 质量检测标准不同

### 11.2 subGroups 是什么？

**答**: `subGroups` 是特征组的子分组结构，用于包含参数信息。

**结构对比**:

```
无参数特征组 (如Profile):
  - name
  - color
  - totalGroupFeatureCount
  - featureCount          ← 直接在组级别
  - features              ← 直接在组级别

有参数特征组 (如Hole):
  - name
  - color
  - totalGroupFeatureCount
  - subGroupCount         ← 子组数量
  - subGroups             ← 子组数组
      - parametersCount
      - parameters        ← 参数信息
      - featureCount      ← 在子组级别
      - features          ← 在子组级别
```

**为什么需要subGroups**:

1. 支持参数模板机制
2. 未来可扩展多个参数模板（当前始终为1个）
3. 保持数据结构的一致性和可扩展性

### 11.3 如何处理没有参数的特征组？

**答**: 没有参数的特征组（如Profile、Machining Face）直接在组级别包含 `features` 数组，不使用 `subGroups`。

**解析代码示例**:

```python
def extract_features_from_group(group):
    """从特征组提取特征列表"""

    # 检查是否有subGroups
    if 'subGroups' in group:
        # 有参数的组，从subGroups提取
        return group['subGroups'][0]['features']
    else:
        # 无参数的组，直接提取
        return group['features']
```

### 11.4 featureCount 和 totalGroupFeatureCount 的区别？

**答**:

| 字段                       | 位置         | 含义           |
|--------------------------|------------|--------------|
| `totalGroupFeatureCount` | 组级别        | 该组的总特征数量     |
| `featureCount`           | 子组级别 或 组级别 | 该子组（或组）的特征数量 |

**在当前实现中**: 两者总是相等，因为 `subGroupCount` 始终为 1。

**示例**:

```json
{
  "name": "Blind Hole(s)",
  "totalGroupFeatureCount": "7",
  // 组级别：总共7个特征
  "subGroupCount": "1",
  // 只有1个子组
  "subGroups": [
    {
      "featureCount": "7",
      // 子组级别：这个子组有7个特征
      "features": [
        ...
      ]
      // 7个特征
    }
  ]
}
```

### 11.5 为什么数值都是字符串格式？

**答**: 采用字符串格式的原因：

1. **精度保持**: 避免浮点数精度损失
2. **兼容性**: 某些JSON解析器对大数值支持有限
3. **格式控制**: 便于控制小数位数的显示

**使用时需要转换**:

```python
volume = float(geom_props['volume'])         # 转换为浮点数
feature_count = int(group['featureCount'])   # 转换为整数
```

### 11.6 如何识别特定类型的特征？

**答**: 通过 `name` 字段进行精确匹配。

**示例**:

```python
def find_features_by_type(data, feature_type_name):
    """根据类型名称查找特征"""

    results = []

    for part in data['parts']:
        for group in part['featureRecognition']['featureGroups']:
            if group['name'] == feature_type_name:
                results.append(group)

    return results

# 使用示例
blind_holes = find_features_by_type(data, "Blind Hole(s)")
```

**支持的类型名称列表**: 参见 [第8章 颜色编码规范](#8-颜色编码规范) 的表格。

### 11.7 一个特征可以有多少个面？

**答**: 取决于特征类型：

| 特征类型  | 典型面数量 | 范围     |
|-------|-------|--------|
| 简单圆柱孔 | 1-4   | 1-10   |
| 阶梯孔   | 5-8   | 5-15   |
| 沉孔    | 5     | 3-10   |
| 封闭型腔  | 12-23 | 10-50+ |
| 开口型腔  | 2-6   | 2-20   |
| 通槽    | 12+   | 10-30  |
| 加工面   | 1     | 1-3    |
| 外轮廓   | 18+   | 5-100+ |

**注意**: 复杂特征可能包含更多面（如带圆角、倒角的型腔）。

### 11.8 如何处理JSON中的重复面ID？

**答**: 在当前实现中，同一个面ID **可能出现在多个特征组中**，特别是：

- 过渡面可能属于多个相邻特征
- 共享边界的特征可能共享某些面

**处理建议**:

1. **可视化**: 允许面同时高亮多种颜色（混合或分层显示）
2. **分析**: 统计面的共享情况，识别特征间的拓扑关系
3. **优先级**: 为特征类型设置优先级，冲突时选择优先级高的

```python
def check_duplicate_face_ids(data):
    """检查重复的面ID"""

    face_id_map = {}  # {face_id: [feature_group_names]}

    for part in data['parts']:
        for group in part['featureRecognition']['featureGroups']:
            features = extract_features_from_group(group)

            for feature in features:
                for shape_id_obj in feature['shapeIDs']:
                    face_id = shape_id_obj['id']

                    if face_id not in face_id_map:
                        face_id_map[face_id] = []
                    face_id_map[face_id].append(group['name'])

    # 找出重复的面ID
    duplicates = {fid: groups for fid, groups in face_id_map.items() if len(groups) > 1}

    return duplicates
```

### 11.9 Axis参数的坐标系是什么？

**答**: Axis参数使用的是**CAD模型的全局坐标系**。

**坐标系约定**:

- X轴: 通常为零件长度方向
- Y轴: 零件宽度方向
- Z轴: 零件高度方向（通常为重力方向）

**典型Axis值**:

| 方向 | Axis值      | 说明    |
|----|------------|-------|
| 向上 | (0, 0, 1)  | 沿+Z方向 |
| 向下 | (0, 0, -1) | 沿-Z方向 |
| 向右 | (1, 0, 0)  | 沿+X方向 |
| 向左 | (-1, 0, 0) | 沿-X方向 |
| 向前 | (0, 1, 0)  | 沿+Y方向 |
| 向后 | (0, -1, 0) | 沿-Y方向 |

**单位向量**: Axis始终为单位向量，即 `√(x² + y² + z²) = 1`。

### 11.10 如何验证JSON文件的完整性？

**答**: 使用以下验证检查表：

```python
def validate_json_integrity(data):
    """验证JSON完整性的检查列表"""

    checks = []

    # 1. 版本字段存在
    checks.append(('version字段', 'version' in data))

    # 2. 至少有一个零件
    checks.append(('至少一个零件', len(data.get('parts', [])) > 0))

    for part in data.get('parts', []):
        part_id = part.get('partId', 'unknown')

        # 3. 零件有ID和名称
        checks.append((f'{part_id}: 有partId', 'partId' in part))
        checks.append((f'{part_id}: 有partName', 'partName' in part))

        # 4. 几何属性完整
        geom = part.get('geometricProperties', {})
        checks.append((f'{part_id}: 有体积', 'volume' in geom))
        checks.append((f'{part_id}: 有表面积', 'surfaceArea' in geom))
        checks.append((f'{part_id}: 有AABB', 'AABB' in geom))

        # 5. 特征识别结果存在
        feat_rec = part.get('featureRecognition', {})
        checks.append((f'{part_id}: 有featureRecognition', bool(feat_rec)))
        checks.append((f'{part_id}: 有totalFeatureCount', 'totalFeatureCount' in feat_rec))

        # 6. 特征数量一致性
        if 'totalFeatureCount' in feat_rec and 'featureGroups' in feat_rec:
            total_declared = int(feat_rec['totalFeatureCount'])
            total_computed = sum(
                int(g['totalGroupFeatureCount'])
                for g in feat_rec['featureGroups']
            )
            checks.append((
                f'{part_id}: 特征数量一致',
                total_declared == total_computed
            ))

    # 输出检查结果
    all_passed = True
    for check_name, passed in checks:
        status = "✓" if passed else "✗"
        print(f"{status} {check_name}")
        if not passed:
            all_passed = False

    return all_passed
```

---

## 12. 附录

### 12.1 完整JSON Schema定义

```json
{
  "$schema": "http://json-schema.org/draft-07/schema#",
  "title": "IFR Feature Recognition Result",
  "type": "object",
  "required": [
    "version",
    "parts"
  ],
  "properties": {
    "version": {
      "type": "string",
      "description": "JSON格式版本号"
    },
    "parts": {
      "type": "array",
      "items": {
        "$ref": "#/definitions/Part"
      }
    }
  },
  "definitions": {
    "Part": {
      "type": "object",
      "required": [
        "partId",
        "partName",
        "geometricProperties",
        "process",
        "featureRecognition"
      ],
      "properties": {
        "partId": {
          "type": "string"
        },
        "partName": {
          "type": "string"
        },
        "geometricProperties": {
          "$ref": "#/definitions/GeometricProperties"
        },
        "process": {
          "type": "string",
          "enum": [
            "CNC Machining Milling"
          ]
        },
        "featureRecognition": {
          "$ref": "#/definitions/FeatureRecognition"
        }
      }
    },
    "GeometricProperties": {
      "type": "object",
      "required": [
        "volume",
        "surfaceArea",
        "AABB"
      ],
      "properties": {
        "volume": {
          "type": "string",
          "pattern": "^[0-9]+\\.[0-9]{2}$"
        },
        "surfaceArea": {
          "type": "string",
          "pattern": "^[0-9]+\\.[0-9]{2}$"
        },
        "AABB": {
          "$ref": "#/definitions/AABB"
        }
      }
    },
    "AABB": {
      "type": "object",
      "required": [
        "dimensions",
        "minCorner",
        "maxCorner"
      ],
      "properties": {
        "dimensions": {
          "$ref": "#/definitions/Coordinate"
        },
        "minCorner": {
          "$ref": "#/definitions/Coordinate"
        },
        "maxCorner": {
          "$ref": "#/definitions/Coordinate"
        }
      }
    },
    "Coordinate": {
      "type": "object",
      "required": [
        "x",
        "y",
        "z"
      ],
      "properties": {
        "x": {
          "type": "string"
        },
        "y": {
          "type": "string"
        },
        "z": {
          "type": "string"
        }
      }
    },
    "FeatureRecognition": {
      "type": "object",
      "required": [
        "name",
        "totalFeatureCount",
        "featureGroups"
      ],
      "properties": {
        "name": {
          "type": "string",
          "const": "Feature Recognition"
        },
        "totalFeatureCount": {
          "type": "string",
          "pattern": "^[0-9]+$"
        },
        "featureGroups": {
          "type": "array",
          "items": {
            "$ref": "#/definitions/FeatureGroup"
          }
        }
      }
    },
    "FeatureGroup": {
      "type": "object",
      "required": [
        "name",
        "color",
        "totalGroupFeatureCount"
      ],
      "properties": {
        "name": {
          "type": "string"
        },
        "color": {
          "type": "string",
          "pattern": "^\\([0-9]{1,3}, [0-9]{1,3}, [0-9]{1,3}\\)$"
        },
        "totalGroupFeatureCount": {
          "type": "string",
          "pattern": "^[0-9]+$"
        },
        "subGroupCount": {
          "type": "string",
          "pattern": "^[0-9]+$"
        },
        "subGroups": {
          "type": "array",
          "items": {
            "$ref": "#/definitions/SubGroup"
          }
        },
        "featureCount": {
          "type": "string",
          "pattern": "^[0-9]+$"
        },
        "features": {
          "type": "array",
          "items": {
            "$ref": "#/definitions/Feature"
          }
        }
      }
    },
    "SubGroup": {
      "type": "object",
      "required": [
        "parametersCount",
        "parameters",
        "featureCount",
        "features"
      ],
      "properties": {
        "parametersCount": {
          "type": "string",
          "pattern": "^[0-9]+$"
        },
        "parameters": {
          "type": "array",
          "items": {
            "$ref": "#/definitions/Parameter"
          }
        },
        "featureCount": {
          "type": "string",
          "pattern": "^[0-9]+$"
        },
        "features": {
          "type": "array",
          "items": {
            "$ref": "#/definitions/Feature"
          }
        }
      }
    },
    "Parameter": {
      "type": "object",
      "required": [
        "name",
        "units",
        "value"
      ],
      "properties": {
        "name": {
          "type": "string"
        },
        "units": {
          "type": "string"
        },
        "value": {
          "type": "string"
        }
      }
    },
    "Feature": {
      "type": "object",
      "required": [
        "shapeIDCount",
        "shapeIDs"
      ],
      "properties": {
        "shapeIDCount": {
          "type": "string",
          "pattern": "^[0-9]+$"
        },
        "shapeIDs": {
          "type": "array",
          "items": {
            "$ref": "#/definitions/ShapeID"
          }
        },
        "innerFilletsIDs": {
          "type": "array",
          "description": "型腔内部圆角面ID列表（可选）",
          "items": {
            "type": "string",
            "pattern": "^[0-9]+$"
          }
        },
        "bossesIDs": {
          "type": "array",
          "description": "型腔内部凸台面ID列表（可选）",
          "items": {
            "type": "string",
            "pattern": "^[0-9]+$"
          }
        },
        "supportInfos": {
          "type": "array",
          "description": "倒角支撑面信息（可选，仅倒角特征）",
          "items": {
            "$ref": "#/definitions/SupportInfo"
          }
        }
      }
    },
    "SupportInfo": {
      "type": "object",
      "description": "倒角面的支撑面关系信息",
      "required": [
        "faceId",
        "supportFaceId1",
        "supportFaceId2"
      ],
      "properties": {
        "faceId": {
          "type": "string",
          "pattern": "^[0-9]+$",
          "description": "倒角面ID"
        },
        "supportFaceId1": {
          "type": "string",
          "pattern": "^[0-9]+$",
          "description": "第一个支撑面ID"
        },
        "supportFaceId2": {
          "type": "string",
          "pattern": "^[0-9]+$",
          "description": "第二个支撑面ID"
        }
      }
    },
    "ShapeID": {
      "type": "object",
      "required": [
        "id"
      ],
      "properties": {
        "id": {
          "type": "string",
          "pattern": "^[0-9]+$"
        }
      }
    }
  }
}
```

### 12.2 特征类型枚举表

#### 12.2.1 CNC_FeatureType 枚举

| 枚举值                             | 整数值 | 说明    |
|---------------------------------|-----|-------|
| Undefined                       | 0   | 未定义   |
| Hole_Round                      | 1   | 圆孔    |
| Hole_Countersink                | 2   | 沉孔    |
| Hole_Stepped                    | 3   | 阶梯孔   |
| Pocket                          | 5   | 型腔    |
| MachiningFace_CircularMilled    | 8   | 圆形铣削面 |
| MachiningFace_FlatFaceMilled    | 9   | 平面铣削面 |
| MachiningFace_FlatSideMilled    | 10  | 侧面平铣面 |
| MachiningFace_CurvedMilled      | 11  | 曲面铣削面 |
| MachiningFace_ConcaveFilletEdge | 12  | 凹圆角边  |
| MachiningFace_ConvexProfileEdge | 13  | 凸轮廓边  |
| Boss_General                    | 14  | 凸台    |
| Chamfer                         | 15  | 倒角    |
| Fillet                          | 16  | 圆角    |
| Composite_Feature               | 17  | 复合特征  |
| Generic_Feature                 | 18  | 通用特征  |
| Profile                         | 19  | 外轮廓   |

#### 12.2.2 CNC_PocketType 枚举

| 枚举值     | 整数值 | 说明            |
|---------|-----|---------------|
| Closed  | 0   | 封闭型腔 - 四周有壁   |
| Open    | 1   | 开口型腔 - 至少一侧开口 |
| Through | 2   | 通槽 - 完全贯穿零件   |

#### 12.2.3 CNC_MachiningFaceType 枚举

| 枚举值               | 整数值 | 说明    |
|-------------------|-----|-------|
| Undefined         | 0   | 未定义   |
| CircularMilled    | 1   | 圆形铣削面 |
| FlatFaceMilled    | 2   | 平面铣削面 |
| FlatSideMilled    | 3   | 侧面平铣面 |
| CurvedMilled      | 4   | 曲面铣削面 |
| ConcaveFilletEdge | 5   | 凹圆角边  |
| ConvexProfileEdge | 6   | 凸轮廓边  |

#### 12.2.4 CNC_ChamferType 枚举（JSON字符串值）

| 字符串值   | 说明                    |
|--------|------------------------|
| Edge   | 边倒角，沿单条边进行的倒角         |
| Vertex | 顶点倒角，在顶点位置形成的倒角       |
| Hybrid | 混合倒角，包含边倒角和顶点倒角的组合    |
| Planar | 平面倒角，形成平面斜切的倒角        |

### 12.3 参考资料

#### 12.3.1 源代码文件

| 文件路径                                                                   | 说明        |
|------------------------------------------------------------------------|-----------|
| `src/featureRecognizer/recognizer/CNC_MachiningFaceJsonSerializer.h`   | 序列化器头文件   |
| `src/featureRecognizer/recognizer/CNC_MachiningFaceJsonSerializer.cpp` | 序列化器实现    |
| `src/featureRecognizer/recognizer/CNC_FeatureTypes.h`                  | 特征类型定义    |
| `src/featureRecognizer/features/CNC_HoleFeature.h`                     | 孔特征类      |
| `src/featureRecognizer/features/CNC_PocketFeature.h`                   | 型腔特征类     |
| `src/featureRecognizer/features/CNC_ProfileFeature.h`                  | 外轮廓特征类    |
| `src/featureRecognizer/features/CNC_ChamferFeature.h`                  | 倒角特征类     |
| `src/featureRecognizer/features/CNC_FilletFeature.h`                   | 圆角特征类     |
| `src/featureRecognizer/brep/Chamfers/asiAlgo_ChamferClassifier.h`      | 倒角分类器     |
| `src/featureRecognizer/brep/Chamfers/asiAlgo_RecognizeChamfers.h`      | 倒角识别算法    |
| `src/featureRecognizer/brep/blends/`                                   | 圆角识别算法目录  |

#### 12.3.2 相关标准和规范

- **ISO 14649**: CNC加工的数据模型标准
- **STEP AP224**: 机械产品定义标准
- **JSON Schema**: http://json-schema.org/

#### 12.3.3 第三方库

- **OpenCASCADE**: CAD几何建模和拓扑处理
    - 官网: https://www.opencascade.com/
    - 文档: https://dev.opencascade.org/doc/overview/html/

- **nlohmann/json**: C++ JSON解析库
    - GitHub: https://github.com/nlohmann/json

### 12.4 版本历史

| 版本     | 日期         | 变更说明                                                          |
|--------|------------|---------------------------------------------------------------|
| v0.7.3 | 2025-12-01 | 新增圆角特征(5.6节)、添加Planar倒角类型、更新颜色编码和术语表                        |
| v0.7.2 | 2025-12-01 | 新增倒角特征(5.5节)、型腔附加字段(5.3.5节)、沉头孔特殊结构(5.2.7节)、更新JSON Schema    |
| v0.7.0 | 2025-10-28 | 初始版本，包含完整的JSON格式定义和解析指南                                      |

### 12.5 术语表

| 术语     | 英文                               | 说明                    |
|--------|----------------------------------|-------------------------|
| 面邻接图   | Face Adjacency Graph (FAG)       | 表示面与面邻接关系的图结构         |
| 轴对齐包围盒 | Axis-Aligned Bounding Box (AABB) | 与坐标轴平行的最小包围盒          |
| B-Rep  | Boundary Representation          | 边界表示法，CAD模型的表示方式      |
| 拓扑     | Topology                         | 几何对象的连接关系和结构          |
| 几何     | Geometry                         | 几何对象的形状和尺寸            |
| 通孔     | Through Hole                     | 完全贯穿零件的孔              |
| 盲孔     | Blind Hole                       | 不贯穿零件的孔，有底面           |
| 沉孔     | Countersink Hole                 | 用于沉头螺钉的台阶孔            |
| 阶梯孔    | Stepped Hole                     | 具有多个直径的孔              |
| 型腔     | Pocket                           | 零件内部的凹陷区域             |
| 通槽     | Through Pocket                   | 完全贯穿零件的槽              |
| 外轮廓    | Profile                          | 零件的外部轮廓面              |
| 倒角     | Chamfer                          | 边缘的斜切面，用于去除锐边         |
| 圆角     | Fillet                           | 边缘的圆弧过渡面，用于减少应力集中     |
| 凸圆角    | Convex Fillet                    | 外圆角，位于零件外部边缘          |
| 凹圆角    | Concave Fillet                   | 内圆角，位于零件内部角落          |
| 支撑面    | Support Face                     | 倒角依附的相邻主体面            |
| 内部圆角   | Inner Fillet                     | 型腔内部的圆角过渡面            |
| 凸台     | Boss                             | 型腔底部或侧壁上的凸起特征         |

### 12.6 联系方式

如有疑问或建议，请联系：

- **项目**: IFR (Interactive Feature Recognition)
- **代码位置**: `F:\Code\YQ\IFR`
- **文档版本**: v1.0.0

---

**文档结束**
