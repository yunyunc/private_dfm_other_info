# FeatureRecognizer 快速开始

版本: 0.6.0

---

## 📦 依赖库版本（重要！）

| 依赖库         | 编译版本       | 最低版本   | vcpkg 安装                                        |
|-------------|------------|--------|-------------------------------------------------|
| OpenCASCADE | **7.9.1**  | 7.8.0  | `vcpkg install opencascade:x64-windows-meshlib` |
| Boost       | **1.88.0** | 1.70.0 | `vcpkg install boost-filesystem boost-system`   |
| Eigen3      | **3.4.0**  | 3.3.0  | `vcpkg install eigen3:x64-windows-meshlib`      |
| spdlog      | **1.15.3** | 1.10.0 | `vcpkg install spdlog:x64-windows-meshlib`      |

⚠️ **重要**: 使用不兼容的版本可能导致运行时错误！建议使用相同或兼容版本。

---

## 🚀 3 步开始使用

### 1. 安装依赖

```bash
# 使用 vcpkg 安装所有依赖
vcpkg install opencascade:x64-windows-meshlib
vcpkg install boost-filesystem:x64-windows-meshlib
vcpkg install boost-system:x64-windows-meshlib
vcpkg install eigen3:x64-windows-meshlib
vcpkg install spdlog:x64-windows-meshlib
```

### 2. 配置 CMake

```cmake
# CMakeLists.txt
cmake_minimum_required(VERSION 3.16)
project(MyApp)

# 设置路径
set(CMAKE_PREFIX_PATH "F:/Code/YQ/IFR/installed")
set(CMAKE_TOOLCHAIN_FILE "[vcpkg]/scripts/buildsystems/vcpkg.cmake")

# 查找库（自动验证版本）
find_package(FeatureRecognizer REQUIRED)

# 创建可执行文件
add_executable(myapp main.cpp)
target_link_libraries(myapp PRIVATE FeatureRecognizer::featureRecognizer)
```

### 3. 编写代码

```cpp
#include <featureRecognizer/CNC_FeatureRecognizer.h>
#include <iostream>

int main() {
    // 创建识别器
    CNC_FeatureRecognizer recognizer;

    // 加载模型
    if (!recognizer.loadModel("model.stp")) {
        std::cerr << "加载失败: " << recognizer.getLastError() << std::endl;
        return 1;
    }

    // 设置参数
    std::string params = R"({
        "recognitionStrategy": "RuleBasedOnly",
        "operationType": "Milling",
        "recognizeHoles": true,
        "recognizePockets": true
    })";
    recognizer.setParameters(params);

    // 执行识别
    if (!recognizer.recognize()) {
        std::cerr << "识别失败: " << recognizer.getLastError() << std::endl;
        return 1;
    }

    // 获取结果
    std::string results = recognizer.getResultsAsJson();
    std::cout << "识别结果:\n" << results << std::endl;

    // 导出到文件
    recognizer.exportResults("results.json");

    return 0;
}
```

---

## 🔍 版本验证

如果 `find_package(FeatureRecognizer)` 失败，你会看到类似错误：

```
CMake Error: FeatureRecognizer requires OpenCASCADE >= 7.8.0, which was not found.
  Compiled with: OpenCASCADE 7.9.1
  Install with: vcpkg install opencascade:x64-windows-meshlib
```

按照提示安装正确版本的依赖库即可。

---

## 📖 完整文档

- **使用指南**: `README_使用指南.md`
- **依赖详情**: `../DEPENDENCIES.md`

---

## 💡 提示

1. **版本兼容**: 建议使用与编译版本相同的依赖库
2. **vcpkg triplet**: 使用 `x64-windows-meshlib` 与编译环境一致
3. **错误信息**: CMake 会自动检查版本并给出详细提示
4. **运行时错误**: 确保 `bin/featureRecognizer.dll` 在 PATH 中

---

**快速帮助**: 查看 `README_使用指南.md` 获取详细信息
