# DFM C# 接口说明

- 版本: v1.0
- 日期: 20260313
- 产物:
  - DLL: [OcctDfmSdk.dll](/D:/code/OcctImgui-worktree/dist/OcctDfmSdk/Release/OcctDfmSdk.dll)
  - 导入库: [OcctDfmSdk.lib](/D:/code/OcctImgui-worktree/dist/OcctDfmSdk/Release/OcctDfmSdk.lib)
  - C 头文件: [OcctDfmCapi.h](/D:/code/OcctImgui-worktree/src/api/OcctDfmCapi.h)
  - C# 示例: [OcctDfmNative.cs](/D:/code/OcctImgui-worktree/dist/OcctDfmSdk/csharp/OcctDfmNative.cs)

## 设计目标
- 给 `C# / PInvoke` 直接调用
- 不暴露 `TopoDS_Shape`、`AIS_InteractiveContext` 这类 C++ / OCCT 类型
- 输入使用 `wchar_t*`（Windows UTF-16）
- 输出使用基础类型或 UTF-16 字符串缓冲区

## 核心导出函数
- `OcctDfm_CreateSession`
- `OcctDfm_DestroySession`
- `OcctDfm_InitializeLoggingW`
- `OcctDfm_LoadTargetStepFileW`
- `OcctDfm_LoadDfmReportFromFileW`
- `OcctDfm_LoadDfmReportFromJsonW`
- `OcctDfm_Clear`
- `OcctDfm_HasDfmReport`
- `OcctDfm_IsProcessable`
- `OcctDfm_GetFaceSeverityW`
- `OcctDfm_GetFaceColorW`
- `OcctDfm_GetLastErrorLengthW`
- `OcctDfm_CopyLastErrorW`
- `OcctDfm_GetVisualizationJsonLengthW`
- `OcctDfm_CopyVisualizationJsonW`

## 可视化 JSON
`OcctDfm_GetVisualizationJsonLengthW` / `OcctDfm_CopyVisualizationJsonW` 返回一个 JSON，主要字段有：

```json
{
  "has_dfm_report": true,
  "processable": false,
  "step_face_index_scheme": "TopExp_Explorer face order, 1-based",
  "total_face_count": 6,
  "highlighted_face_count": 1,
  "faces": [
    {
      "face_id": "1",
      "severity": "red",
      "color": { "r": 1.0, "g": 0.3, "b": 0.3 },
      "violations": [
        {
          "severity": "red",
          "message": "dfm-red",
          "suggestions": ["s1", "s2"]
        }
      ]
    }
  ]
}
```

这份 JSON 的用途是：
- C# 宿主自己做高亮
- 或者把颜色/提示信息转成你目标软件自己的 UI 表示

## 最小调用流程
1. `OcctDfm_CreateSession`
2. `OcctDfm_LoadTargetStepFileW`
3. `OcctDfm_LoadDfmReportFromFileW` 或 `OcctDfm_LoadDfmReportFromJsonW`
4. `OcctDfm_GetVisualizationJsonLengthW`
5. `OcctDfm_CopyVisualizationJsonW`
6. C# 解析 JSON 并自行渲染

## 备注
- 这个版本适合普通 C# 程序直接接入
- 如果你的 C# 程序内部本身也包了一层 OCCT，那么仍然可以继续使用 C++ 类接口 [OcctDfmSdk.h](/D:/code/OcctImgui-worktree/src/api/OcctDfmSdk.h)
