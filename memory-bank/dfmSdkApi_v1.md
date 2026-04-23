# DFM SDK 接口说明

- 版本: v1.0
- 日期: 20260313
- 产物:
  - DLL: [OcctDfmSdk.dll](/D:/code/OcctImgui-worktree/build/ReleaseWT/bin/Release/Release/OcctDfmSdk.dll)
  - 导入库: [OcctDfmSdk.lib](/D:/code/OcctImgui-worktree/build/ReleaseWT/lib/Release/Release/OcctDfmSdk.lib)
  - 头文件: [OcctDfmSdk.h](/D:/code/OcctImgui-worktree/src/api/OcctDfmSdk.h)

## 适用范围
- 这个接口面向 OCCT/C++ 宿主程序
- 外部程序需要能访问 `TopoDS_Shape`、`TopoDS_Face`、`AIS_InteractiveContext`、`V3d_View`
- 外部程序需要与本 DLL 使用兼容的 MSVC / CRT / OpenCASCADE 运行时

## 核心类
- `OcctDfmSdkSession`

## 主要能力
- 载入目标零件:
  - `loadTargetShape(const TopoDS_Shape& shape)`
  - `loadTargetStepFile(const std::string& stepFilePath)`
- 载入 DFM 报告:
  - `loadDfmReportFromJson(const std::string& jsonString)`
  - `loadDfmReportFromFile(const std::string& reportFilePath)`
- 查询映射关系:
  - `getFaceId(const TopoDS_Face& face)`
  - `getFaceSeverity(const std::string& faceId)`
  - `getFaceDisplayColor(const std::string& faceId)`
  - `getViolationsForFace(const std::string& faceId)`
- 生成/显示可视化:
  - `buildOverlay()`
  - `displayOverlay(const Handle(AIS_InteractiveContext)& context, const Handle(V3d_View)& view = {})`
  - `clearOverlay(const Handle(AIS_InteractiveContext)& context, const Handle(V3d_View)& view = {})`

## 最小调用示例
```cpp
#include "api/OcctDfmSdk.h"

OcctDfmSdkSession sdk;
sdk.initializeLogging("logs");

if (!sdk.loadTargetStepFile("part.step")) {
    throw std::runtime_error(sdk.getLastError());
}

if (!sdk.loadDfmReportFromFile("report.json")) {
    throw std::runtime_error(sdk.getLastError());
}

if (!sdk.displayOverlay(context, view)) {
    throw std::runtime_error("failed to display DFM overlay");
}
```

## 集成说明
- 如果宿主程序已经自己管理模型显示，可只调用 `buildOverlay()`，自己决定何时 `Display`
- 如果只需要颜色/违规信息，不需要显示，可只用查询接口
- 若调用 `loadTargetStepFile(...)`，DLL 内部会用 STEP 读入目标 shape
- 若宿主程序已经有 shape，优先用 `loadTargetShape(...)`，少一次文件读取

## 当前限制
- 目前不是纯 C ABI；不要跨不同编译器/不同 STL ABI 直接混用
- 当前只打包了 `featureRecognizer.dll` 这个项目自带依赖；若宿主环境没有相同版本的 OpenCASCADE / vcpkg runtime，还需要补齐运行库
