# FeatureRecognizer 库使用指南

版本: 0.6.2
日期: 2025-10-28

---

## 📦 安装内容

此目录包含 FeatureRecognizer 库的完整安装，包括：

### 头文件

- `include/featureRecognizer/CNC_FeatureRecognizer.h` - 主 API 接口
- `include/featureRecognizer/CNC_FeatureExport.h` - 导出宏定义
- `include/featureRecognizer/CNC_Version.h` - 版本信息

### 库文件

- `bin/featureRecognizer.dll` - 动态链接库 (Windows)
- `lib/featureRecognizer.lib` - 导入库 (Windows)

### CMake 配置文件

- `lib/cmake/FeatureRecognizer/FeatureRecognizerConfig.cmake`
- `lib/cmake/FeatureRecognizer/FeatureRecognizerConfigVersion.cmake`
- `lib/cmake/FeatureRecognizer/FeatureRecognizerTargets.cmake`

---

## 🚀 使用方法

### 方法 1: 在 CMake 项目中使用

#### 1. 创建你的项目

```cmake
# CMakeLists.txt
cmake_minimum_required(VERSION 3.16)
project(MyFeatureRecognitionApp)

# 设置安装路径（指向此目录）
set(CMAKE_PREFIX_PATH "F:/Code/YQ/IFR/installed")

# 或者在命令行中指定:
# cmake -DCMAKE_PREFIX_PATH=F:/Code/YQ/IFR/installed ..

# 查找 FeatureRecognizer 库
# 这会自动查找所有依赖（OpenCASCADE, Boost, Eigen3, spdlog）
find_package(FeatureRecognizer REQUIRED)

# 创建可执行文件
add_executable(myapp main.cpp)

# 链接 FeatureRecognizer 库
target_link_libraries(myapp PRIVATE FeatureRecognizer::featureRecognizer)
```

#### 2. 编写你的代码

```cpp
// main.cpp
#include <featureRecognizer/CNC_FeatureRecognizer.h>
#include <iostream>
#include <string>

int main(int argc, char* argv[])
{
    std::cout << "FeatureRecognizer 使用示例" << std::endl;

    // 1. 创建识别器
    CNC_FeatureRecognizer recognizer;

    // 2. 显示版本信息
    CNC_Version version = CNC_FeatureRecognizer::getVersion();
    std::cout << "库版本: " << version.major << "."
              << version.minor << "." << version.patch << std::endl;

    // 3. 加载 CAD 模型
    std::string modelPath = "your_model.stp";
    if (!recognizer.loadModel(modelPath)) {
        std::cerr << "加载模型失败: " << recognizer.getLastError() << std::endl;
        return 1;
    }
    std::cout << "模型加载成功" << std::endl;

    // 4. 设置识别参数（JSON 格式）
    std::string jsonParams = R"({
        "recognitionStrategy": "RuleBasedOnly",
        "operationType": "Milling",
        "linearTolerance": 0.01,
        "recognizeHoles": true,
        "recognizePockets": true,
        "recognizeSlots": true,
        "maxRadius": 100.0,
        "meshSegmentationAngle": 30.0
    })";

    if (!recognizer.setParameters(jsonParams)) {
        std::cerr << "设置参数失败: " << recognizer.getLastError() << std::endl;
        return 1;
    }

    // 5. 执行特征识别
    std::cout << "开始特征识别..." << std::endl;
    if (!recognizer.recognize()) {
        std::cerr << "特征识别失败: " << recognizer.getLastError() << std::endl;
        return 1;
    }
    std::cout << "特征识别完成" << std::endl;

    // 6. 获取识别结果（JSON 格式）
    std::string jsonResults = recognizer.getResultsAsJson();
    std::cout << "识别结果:\n" << jsonResults << std::endl;

    // 7. 导出结果到文件
    std::string outputPath = "recognition_results.json";
    if (recognizer.exportResults(outputPath)) {
        std::cout << "结果已导出到: " << outputPath << std::endl;
    }

    return 0;
}
```

#### 3. 构建项目

```bash
# 配置项目
cmake -B build -S . -DCMAKE_PREFIX_PATH=F:/Code/YQ/IFR/installed

# 构建
cmake --build build --config Release

# 运行
./build/Release/myapp.exe your_model.stp
```

---

## 📋 API 参考

### CNC_FeatureRecognizer 类

#### 静态方法

- `static CNC_Version getVersion()` - 获取库版本信息
- `static std::string getVersionString()` - 获取版本字符串
- `static std::vector<std::string> getSupportedFormats()` - 获取支持的 CAD 格式

#### 实例方法

**模型加载**

- `bool loadModel(const std::string& filePath)` - 从文件加载 CAD 模型（支持 STEP, IGES）
- `bool loadModelFromString(const std::string& strShape, bool isBin = false)` - 从序列化字符串加载模型
    - `strShape`: 序列化后的形状字符串
    - `isBin`: true表示二进制格式，false表示Base64格式（默认）
    - 使用场景：网络传输、缓存加载、跨进程通信

**参数设置与识别**

- `bool setParameters(const std::string& jsonParams)` - 设置识别参数（JSON 格式）
- `bool recognize()` - 执行特征识别

**结果获取**

- `std::string getResultsAsJson() const` - 获取识别结果（JSON 格式）
- `bool exportResults(const std::string& filePath) const` - 导出结果到 JSON 文件

**错误处理**

- `std::string getLastError() const` - 获取最后一次错误信息

---

## 🔧 环境要求

### 必需的依赖库

使用此库需要安装以下依赖（推荐使用 vcpkg）：

| 依赖库             | 编译时版本  | 说明                |
|-----------------|--------|-------------------|
| **OpenCASCADE** | 7.9.1  | CAD 几何建模内核      |
| **Boost**       | 1.88.0 | 文件系统和系统工具       |
| **Eigen3**      | 3.4.0  | 线性代数库           |
| **spdlog**      | 1.15.3 | 日志库             |
| **fmt**         | 11.2.0 | 格式化库（spdlog 依赖） |

**注意**: 建议使用相同或兼容版本以确保最佳兼容性。详细的版本兼容性说明请参考项目根目录的 `DEPENDENCIES.md`。

### vcpkg 安装命令（推荐）

使用与编译环境相同的 triplet：

```bash
vcpkg install opencascade:x64-windows-meshlib
vcpkg install boost-filesystem:x64-windows-meshlib
vcpkg install boost-system:x64-windows-meshlib
vcpkg install eigen3:x64-windows-meshlib
vcpkg install spdlog:x64-windows-meshlib
vcpkg install fmt:x64-windows-meshlib
```

或使用默认 triplet：

```bash
vcpkg install opencascade eigen3 boost-filesystem boost-system spdlog fmt
```

---

## 📝 支持的 CAD 格式

- **STEP** (.stp, .step)
- **IGES** (.igs, .iges)

---

## 📊 识别结果格式

识别结果以 JSON 格式输出，包含以下特征类型：

### 孔特征（自动区分通孔/盲孔）

- **Blind Hole(s)** - 盲孔，颜色: (200, 100, 100)
- **Through Hole(s)** - 通孔，颜色: (240, 135, 132)
- **Stepped Blind Hole(s)** - 阶梯盲孔，颜色: (150, 0, 90)
- **Stepped Through Hole(s)** - 阶梯通孔，颜色: (204, 0, 125)
- **Countersink Blind Hole(s)** - 沉孔盲孔，颜色: (150, 0, 90)
- **Countersink Through Hole(s)** - 沉孔通孔，颜色: (204, 0, 125)

### 型腔特征

- **Closed Pocket(s)** - 封闭口袋，颜色: (81, 20, 0)
- **Open Pocket(s)** - 开放口袋，颜色: (120, 60, 30)
- **Through Pocket(s)** - 通槽，颜色: (160, 100, 60)

### 其他特征

- **Profile Feature(s)** - 外轮廓特征
- **Machining Face(s)** - 加工面特征

**详细格式说明**: 完整的JSON格式解析指南请参考 `docs/IFR特征识别结果JSON格式解析指南.md`

---

## 🔍 识别参数说明

JSON 格式的识别参数示例：

```json
{
  "recognitionStrategy": "RuleBasedOnly",
  // 或 "HybridGraphOnly"
  "operationType": "Milling",
  // 或 "Turning"
  "linearTolerance": 0.01,
  // 线性容差
  "angularTolerance": 5.0,
  // 角度容差（度）
  "recognizeHoles": true,
  // 识别孔特征
  "recognizePockets": true,
  // 识别型腔特征
  "recognizeSlots": true,
  // 识别槽特征
  "maxRadius": 100.0,
  // 最大半径
  "meshSegmentationAngle": 30.0
  // 网格分割角度（度）
}
```

### 参数详细说明

| 参数名                     | 类型     | 默认值             | 说明                                                                 |
|-------------------------|--------|-----------------|--------------------------------------------------------------------|
| `recognitionStrategy`   | string | "RuleBasedOnly" | 识别策略：<br/>- "RuleBasedOnly": 仅基于规则<br/>- "HybridGraphOnly": 混合图论方法 |
| `operationType`         | string | "Milling"       | 加工类型：<br/>- "Milling": 铣削<br/>- "Turning": 车削                      |
| `linearTolerance`       | double | 0.01            | 线性容差（单位：模型单位）                                                      |
| `angularTolerance`      | double | 5.0             | 角度容差（单位：度）                                                         |
| `recognizeHoles`        | bool   | true            | 是否识别孔特征                                                            |
| `recognizePockets`      | bool   | true            | 是否识别型腔特征                                                           |
| `recognizeSlots`        | bool   | true            | 是否识别槽特征                                                            |
| `maxRadius`             | double | 1e10            | 最大识别半径限制                                                           |
| `meshSegmentationAngle` | double | 20.0            | 网格分割角度（单位：度）                                                       |

---

## 🚀 高级用法

### 使用序列化字符串加载模型

适用于网络传输、缓存或跨进程通信场景：

```cpp
#include <featureRecognizer/CNC_FeatureRecognizer.h>
#include <iostream>
#include <fstream>

int main() {
    CNC_FeatureRecognizer recognizer;

    // 方式1：从文件读取序列化字符串
    std::ifstream file("serialized_model.txt");
    std::string serializedShape((std::istreambuf_iterator<char>(file)),
                                std::istreambuf_iterator<char>());
    file.close();

    // 加载序列化模型（Base64格式）
    if (!recognizer.loadModelFromString(serializedShape, false)) {
        std::cerr << "加载失败: " << recognizer.getLastError() << std::endl;
        return 1;
    }

    // 方式2：从网络接收的二进制数据
    // std::string binaryData = receivedFromNetwork();
    // recognizer.loadModelFromString(binaryData, true);

    // 设置参数并识别
    std::string params = R"({"recognizeHoles": true})";
    recognizer.setParameters(params);

    if (recognizer.recognize()) {
        std::string results = recognizer.getResultsAsJson();
        std::cout << "识别成功:\n" << results << std::endl;
    }

    return 0;
}
```

### 批量处理多个模型

```cpp
#include <featureRecognizer/CNC_FeatureRecognizer.h>
#include <vector>
#include <iostream>

void processModels(const std::vector<std::string>& modelPaths) {
    CNC_FeatureRecognizer recognizer;

    // 设置一次参数，重复使用
    std::string params = R"({
        "recognitionStrategy": "RuleBasedOnly",
        "operationType": "Milling",
        "recognizeHoles": true,
        "recognizePockets": true,
        "recognizeSlots": true
    })";

    if (!recognizer.setParameters(params)) {
        std::cerr << "参数设置失败" << std::endl;
        return;
    }

    for (const auto& path : modelPaths) {
        std::cout << "处理: " << path << std::endl;

        if (!recognizer.loadModel(path)) {
            std::cerr << "  加载失败: " << recognizer.getLastError() << std::endl;
            continue;
        }

        if (!recognizer.recognize()) {
            std::cerr << "  识别失败: " << recognizer.getLastError() << std::endl;
            continue;
        }

        // 导出结果
        std::string outputPath = path + "_results.json";
        if (recognizer.exportResults(outputPath)) {
            std::cout << "  结果已保存: " << outputPath << std::endl;
        }
    }
}
```

---

## 🆘 故障排除

### 问题 1: 找不到 FeatureRecognizer

**错误**: `CMake Error: Could not find a package configuration file provided by "FeatureRecognizer"`

**解决方案**:

```cmake
# 确保设置了正确的 CMAKE_PREFIX_PATH
set(CMAKE_PREFIX_PATH "F:/Code/YQ/IFR/installed")
```

或在命令行中指定:

```bash
cmake -DCMAKE_PREFIX_PATH=F:/Code/YQ/IFR/installed ..
```

### 问题 2: 找不到依赖库

**错误**: `FeatureRecognizer requires OpenCASCADE, which was not found.`

**解决方案**:
确保你的 CMake 能找到所有依赖库。如果使用 vcpkg:

```bash
cmake -DCMAKE_TOOLCHAIN_FILE=[vcpkg root]/scripts/buildsystems/vcpkg.cmake \
      -DCMAKE_PREFIX_PATH=F:/Code/YQ/IFR/installed \
      ..
```

### 问题 3: 运行时找不到 DLL

**错误**: `无法启动此程序，因为计算机中丢失 featureRecognizer.dll`

**解决方案**:

- 将 `F:/Code/YQ/IFR/installed/bin` 添加到系统 PATH
- 或者将 `featureRecognizer.dll` 复制到可执行文件所在目录

---

## 📞 技术支持与文档资源

### 示例代码

- **CNC_FeatureRecognizer 使用示例**: `F:/Code/YQ/IFR/src/tests/examples/CNC_FeatureRecognizer_Example.cpp`

### 详细文档

- **JSON格式解析指南**: `F:/Code/YQ/IFR/docs/IFR特征识别结果JSON格式解析指南.md`
    - 完整的JSON Schema定义
    - 所有特征类型说明（含孔特征二级分组）
    - 解析示例代码（Python/C++/JavaScript）
    - 颜色编码规范
    - 常见问题解答

- **调试指南**: `F:/Code/YQ/IFR/docs/如何在RelWithDebInfo模式下调试IFR源码.md`
    - RelWithDebInfo 构建配置
    - Visual Studio 调试设置
    - CMakeLists.txt 实现详解

- **项目配置**: `F:/Code/YQ/IFR/CLAUDE.md`
    - 项目架构说明
    - 编码规范
    - 依赖管理

### Benchmark 测试

- **测试套件配置**: `F:/Code/YQ/IFR/tests/benchmarks/config/benchmark_suite.json`
- **测试模型目录**: `F:/Code/YQ/IFR/tests/benchmarks/models/`

---

## 📄 许可证

请参阅项目根目录的 LICENSE 文件。

---

**版本历史**

- **0.6.2** (2025-10-28) - 孔特征识别增强与文档完善
    - ✨ **孔特征二级分组**：自动区分通孔和盲孔，生成独立特征组
        - 新增 6 种孔特征类型：Blind Hole(s), Through Hole(s), Stepped Blind/Through Hole(s), Countersink Blind/Through
          Hole(s)
        - 实现 `determineIsThrough()` 辅助函数，支持 CNC_HoleFeature、CNC_SteppedHoleFeature、CNC_CountersinkHoleFeature
        - 新增 `generateHoleGroupName()` 和 `getHoleColor()` 辅助函数
    - 🎨 **颜色编码系统优化**：通孔和盲孔使用不同颜色区分
        - 通孔：(240, 135, 132) 粉红色 / 盲孔：(200, 100, 100) 深粉色
        - 阶梯孔/沉孔通孔：(204, 0, 125) / 盲孔：(150, 0, 90)
    - 🔧 **毛坯面识别器改进**：
        - 新增 `RemoveIsolatedFaces()` 函数，移除外轮廓候选面中的孤立面
        - 采用保守策略：单面候选集合跳过孤立性检查
        - 基于 AAG 邻接关系判断面的连通性
    - 📚 **文档完善**：
        - 新增《IFR特征识别结果JSON格式解析指南》（12章节，1万+字）
        - 更新 Benchmark 测试套件配置（新增 part23_x_t 和 pocket-boss 模型）
    - 🐛 **Bug修复**：修复孔特征命名固定为 "Through Hole(s)" 的问题

- **0.6.1** (2025-10-24) - JSON序列化器改进
    - ✨ 新API：使用JSON格式参数配置
    - ✨ 新增 `loadModelFromString()` 支持序列化字符串加载
    - ✨ 新增 `recognize()` 方法替代 `Perform()`
    - ✨ 新增 `getResultsAsJson()` 返回JSON格式结果
    - ✨ 新增 `getLastError()` 获取详细错误信息
    - 🔧 简化构造函数：无参数构造
    - 📚 更新测试代码以使用新API

- **0.6.0** (2025-10-21) - 添加 CMake install 支持，使用 Pimpl 模式重构
