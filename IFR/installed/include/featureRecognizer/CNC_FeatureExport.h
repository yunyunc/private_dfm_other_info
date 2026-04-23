//
// Created by xxw on 2025/5/28.
//
// CNC_FeatureExport.h
// CNC特征识别库导出/导入宏定义

#ifndef CNC_FeatureExport_h
#define CNC_FeatureExport_h

//-----------------------------------------------------------------------------
// 平台相关的导出/导入宏
//-----------------------------------------------------------------------------
#ifdef _WIN32
    #ifdef CNC_FEATURE_RECOGNIZER_EXPORTS
        #define CNC_FEATURE_EXPORT __declspec(dllexport)
    #else
        #define CNC_FEATURE_EXPORT __declspec(dllimport)
    #endif
#else
    #ifdef CNC_FEATURE_RECOGNIZER_EXPORTS
        #define CNC_FEATURE_EXPORT __attribute__((visibility("default")))
    #else
        #define CNC_FEATURE_EXPORT
    #endif
#endif

//-----------------------------------------------------------------------------
// 模板类导出宏
//-----------------------------------------------------------------------------
#ifdef _WIN32
    #define CNC_FEATURE_TEMPLATE_EXPORT
#else
    #define CNC_FEATURE_TEMPLATE_EXPORT CNC_FEATURE_EXPORT
#endif

//-----------------------------------------------------------------------------
// 局部函数宏（不导出）
//-----------------------------------------------------------------------------
#define CNC_FEATURE_LOCAL

//-----------------------------------------------------------------------------
// C API导出宏
//-----------------------------------------------------------------------------
#ifdef __cplusplus
    #define CNC_FEATURE_C_EXPORT extern "C" CNC_FEATURE_EXPORT
#else
    #define CNC_FEATURE_C_EXPORT CNC_FEATURE_EXPORT
#endif

#endif // CNC_FeatureExport_h 