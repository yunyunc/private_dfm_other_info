//
// Created by xxw on 2025/5/22.
//

//
// CNC_FeatureRecognizer.h
// 统一的CNC特征识别接口
// 版本: 2.0.0
//

#ifndef CNC_FeatureRecognizer_h
#define CNC_FeatureRecognizer_h

#include <string>
#include <vector>
#include <memory>
#include "CNC_FeatureExport.h"

//-----------------------------------------------------------------------------
// 版本信息结构
//-----------------------------------------------------------------------------
struct CNC_FEATURE_EXPORT CNC_Version
{
  int major; // 主版本号
  int minor; // 次版本号
  int patch; // 补丁版本号
};

//-----------------------------------------------------------------------------
// 统一特征识别接口类
//-----------------------------------------------------------------------------
class CNC_FEATURE_EXPORT CNC_FeatureRecognizer
{
public:
  // ========== 构造/析构 ==========
  CNC_FeatureRecognizer();
  ~CNC_FeatureRecognizer();

  CNC_FeatureRecognizer(const CNC_FeatureRecognizer&)            = delete;
  CNC_FeatureRecognizer& operator=(const CNC_FeatureRecognizer&) = delete;
  CNC_FeatureRecognizer(CNC_FeatureRecognizer&&)                 = delete;
  CNC_FeatureRecognizer& operator=(CNC_FeatureRecognizer&&)      = delete;

  // ========== 版本与信息接口 ==========

  /// @brief 获取库版本信息
  /// @return 版本结构体（major.minor.patch）
  static CNC_Version getVersion();

  /// @brief 获取版本字符串
  /// @return 格式化的版本字符串，如"1.0.0"
  static std::string getVersionString();

  /// @brief 获取支持的CAD文件格式列表
  /// @return 支持的格式列表，如["STEP", "IGES"]
  static std::vector<std::string> getSupportedFormats();

  // ========== 核心功能接口 ==========

  /// @brief 加载CAD模型文件
  /// @param filePath 模型文件的完整路径（支持STEP、IGES格式）
  /// @return true表示加载成功，false表示失败（调用getLastError()获取详细错误）
  bool loadModel(const std::string& filePath);

  /// @brief 从序列化字符串加载CAD模型
  /// @param strShape 序列化后的形状字符串
  /// @param isBin true表示二进制格式，false表示Base64格式（默认）
  /// @return true表示加载成功，false表示失败（调用getLastError()获取详细错误）
  ///
  /// @details 使用场景：
  /// - 从网络传输接收到的序列化模型数据
  /// - 从缓存或数据库加载预序列化的模型
  /// - 跨进程通信传递模型数据
  ///
  /// @code{.cpp}
  /// // 示例：从Base64字符串加载
  /// CNC_FeatureRecognizer recognizer;
  /// if (recognizer.loadModelFromString(base64String, false)) {
  ///     recognizer.recognize();
  /// } else {
  ///     std::cerr << recognizer.getLastError() << std::endl;
  /// }
  /// @endcode
  ///
  /// @note 序列化字符串可通过 asiAlgo_ShapeSerializer::Serialize() 生成
  bool loadModelFromString(const std::string& strShape, bool isBin = false);

  /// @brief 设置识别参数（JSON格式）
  /// @param jsonParams JSON格式的参数字符串
  /// @return true表示参数设置成功，false表示失败
  ///
  /// @details JSON参数格式示例：
  /// @code{.json}
  /// {
  ///   "recognitionStrategy": "RuleBasedOnly",  // or "HybridGraphOnly"
  ///   "operationType": "Milling",              // or "Turning"
  ///   "linearTolerance": 0.01,
  ///   "recognizeHoles": true,
  ///   "recognizePockets": true,
  ///   "recognizeSlots": true,
  ///   "maxRadius": 100.0,
  ///   "meshSegmentationAngle": 30.0
  /// }
  /// @endcode
  bool setParameters(const std::string& jsonParams);

  /// @brief 执行特征识别
  /// @return true表示识别成功，false表示失败
  bool recognize();

  // ========== 结果获取接口 ==========

  /// @brief 获取识别结果（JSON格式）
  /// @return JSON格式的识别结果字符串
  std::string getResultsAsJson() const;

  /// @brief 导出识别结果到JSON文件
  /// @param filePath 输出文件的完整路径
  /// @return true表示导出成功，false表示失败
  bool exportResults(const std::string& filePath) const;

  // ========== 错误处理 ==========

  /// @brief 获取最后一次操作的错误信息
  /// @return 错误描述字符串，如果没有错误则返回空字符串
  std::string getLastError() const;

private:
  class Impl;                  // 前向声明
  std::unique_ptr<Impl> pImpl; // Pimpl指针
};

#endif // CNC_FeatureRecognizer_h
