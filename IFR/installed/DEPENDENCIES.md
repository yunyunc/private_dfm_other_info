# FeatureRecognizer 库依赖说明

版本: 0.7.0
更新日期: 2025-11-04

---

## 📦 依赖库版本

FeatureRecognizer 库在编译时使用以下依赖库版本：

| 依赖库 | 编译时版本 | 最低兼容版本 | 说明 |
|--------|-----------|-------------|------|
| **OpenCASCADE** | 7.9.1 | 7.8.0 | CAD 几何建模内核 |
| **Boost** | 1.88.0 | 1.70.0 | C++ 实用工具库（filesystem, system） |
| **Eigen3** | 3.4.0 | 3.3.0 | 线性代数库 |
| **spdlog** | 1.15.3 | 1.10.0 | 日志库 |
| **fmt** | 11.2.0 | 11.0.0 | 格式化库（spdlog 依赖） |

**内部依赖**（已编译进 DLL，外部无需安装）：
- **VF3** - 子图同构算法库（三个版本：标准、轻量、并行）

---

## 🚀 安装依赖（vcpkg）

### 方法 1: 使用 x64-windows-meshlib triplet（推荐）

这是项目使用的 triplet，与编译环境保持一致：

```bash
vcpkg install opencascade:x64-windows-meshlib
vcpkg install boost-filesystem:x64-windows-meshlib
vcpkg install boost-system:x64-windows-meshlib
vcpkg install eigen3:x64-windows-meshlib
vcpkg install spdlog:x64-windows-meshlib
vcpkg install fmt:x64-windows-meshlib
```

### 方法 2: 使用默认 triplet

如果你使用标准的 x64-windows triplet：

```bash
vcpkg install opencascade:x64-windows
vcpkg install boost-filesystem:x64-windows
vcpkg install boost-system:x64-windows
vcpkg install eigen3:x64-windows
vcpkg install spdlog:x64-windows
vcpkg install fmt:x64-windows
```

### 方法 3: 一次性安装

```bash
vcpkg install opencascade boost-filesystem boost-system eigen3 spdlog fmt
```

---

## 📋 依赖库详细说明

### 1. OpenCASCADE 7.9.1

**用途**: CAD 几何建模和拓扑操作核心库

**必需组件**:
- `FoundationClasses` - 基础类
- `ModelingData` - 建模数据结构
- `ModelingAlgorithms` - 建模算法
- `Visualization` - 可视化
- `ApplicationFramework` - 应用框架
- `DataExchange` - 数据交换（STEP, IGES）

**版本兼容性**:
- ✅ 7.9.x - 完全兼容（推荐）
- ✅ 7.8.x - 兼容
- ⚠️ 7.7.x 及以下 - 可能存在 API 差异

**vcpkg 安装**:
```bash
vcpkg install opencascade:x64-windows-meshlib
```

**验证版本**:
```cpp
#include <Standard_Version.hxx>
std::cout << "OpenCASCADE: " << OCC_VERSION_COMPLETE << std::endl;
```

---

### 2. Boost 1.88.0

**用途**: 文件系统操作和系统工具

**必需组件**:
- `boost-filesystem` - 文件系统操作
- `boost-system` - 系统错误处理

**版本兼容性**:
- ✅ 1.80.0 - 1.88.0 - 完全兼容（推荐）
- ✅ 1.70.0 - 1.79.0 - 兼容
- ⚠️ 1.70.0 以下 - 不支持

**vcpkg 安装**:
```bash
vcpkg install boost-filesystem:x64-windows-meshlib
vcpkg install boost-system:x64-windows-meshlib
```

**验证版本**:
```cpp
#include <boost/version.hpp>
std::cout << "Boost: " << BOOST_VERSION / 100000 << "."
          << BOOST_VERSION / 100 % 1000 << "."
          << BOOST_VERSION % 100 << std::endl;
```

---

### 3. Eigen3 3.4.0

**用途**: 线性代数运算（矩阵、向量）

**版本兼容性**:
- ✅ 3.4.x - 完全兼容（推荐）
- ✅ 3.3.x - 兼容
- ⚠️ 3.2.x 及以下 - 可能缺少某些 API

**vcpkg 安装**:
```bash
vcpkg install eigen3:x64-windows-meshlib
```

**验证版本**:
```cpp
#include <Eigen/Core>
std::cout << "Eigen: " << EIGEN_WORLD_VERSION << "."
          << EIGEN_MAJOR_VERSION << "."
          << EIGEN_MINOR_VERSION << std::endl;
```

**注意**: Eigen 是纯头文件库，无需链接。

---

### 4. spdlog 1.15.3

**用途**: 高性能日志记录

**版本兼容性**:
- ✅ 1.14.0 - 1.15.x - 完全兼容（推荐）
- ✅ 1.10.0 - 1.13.x - 兼容
- ⚠️ 1.10.0 以下 - 不支持

**vcpkg 安装**:
```bash
vcpkg install spdlog:x64-windows-meshlib
```

**验证版本**:
```cpp
#include <spdlog/version.h>
std::cout << "spdlog: " << SPDLOG_VER_MAJOR << "."
          << SPDLOG_VER_MINOR << "."
          << SPDLOG_VER_PATCH << std::endl;
```

---

### 5. fmt 11.2.0

**用途**: 高性能格式化库（spdlog 依赖）

**版本兼容性**:
- ✅ 11.2.x - 完全兼容（推荐）
- ✅ 11.0.0 - 11.1.x - 兼容
- ⚠️ 11.0.0 以下 - 不支持

**vcpkg 安装**:
```bash
vcpkg install fmt:x64-windows-meshlib
```

**验证版本**:
```cpp
#include <fmt/core.h>
std::cout << "fmt: " << FMT_VERSION / 10000 << "."
          << FMT_VERSION / 100 % 100 << "."
          << FMT_VERSION % 100 << std::endl;
```

**重要提示**:
- fmt 是 spdlog 的必需依赖，版本必须与 spdlog 编译时使用的版本一致
- 如果 spdlog 与 fmt 版本不匹配，会导致运行时 DLL 加载失败
- 推荐在 vcpkg.json 中使用 overrides 固定 fmt 版本

---

## 🔧 CMake 配置

### 自动版本检查

`FeatureRecognizerConfig.cmake` 会自动检查依赖版本：

```cmake
find_package(FeatureRecognizer REQUIRED)
# 自动验证:
#   - OpenCASCADE >= 7.8.0
#   - Boost >= 1.70.0
#   - Eigen3 >= 3.3.0
#   - spdlog >= 1.10.0
#   - fmt >= 11.0.0
```

### 手动指定版本

如果需要使用特定版本：

```cmake
# 指定 vcpkg toolchain
set(CMAKE_TOOLCHAIN_FILE "path/to/vcpkg/scripts/buildsystems/vcpkg.cmake")

# 手动查找依赖（在 find_package(FeatureRecognizer) 之前）
find_package(OpenCASCADE 7.9.1 EXACT REQUIRED)
find_package(Boost 1.88.0 REQUIRED COMPONENTS filesystem system)
find_package(Eigen3 3.4.0 REQUIRED)
find_package(spdlog 1.15.3 REQUIRED)
find_package(fmt 11.2.0 REQUIRED)

# 然后查找 FeatureRecognizer
find_package(FeatureRecognizer REQUIRED)
```

---

## 🧪 验证安装

创建一个测试程序验证所有依赖：

```cpp
// test_dependencies.cpp
#include <iostream>

// OpenCASCADE
#include <Standard_Version.hxx>

// Boost
#include <boost/version.hpp>

// Eigen3
#include <Eigen/Core>

// spdlog
#include <spdlog/version.h>

// fmt
#include <fmt/core.h>

int main()
{
    std::cout << "=== 依赖库版本检查 ===" << std::endl;

    // OpenCASCADE
    std::cout << "OpenCASCADE: " << OCC_VERSION_COMPLETE << std::endl;

    // Boost
    std::cout << "Boost: " << BOOST_VERSION / 100000 << "."
              << BOOST_VERSION / 100 % 1000 << "."
              << BOOST_VERSION % 100 << std::endl;

    // Eigen3
    std::cout << "Eigen: " << EIGEN_WORLD_VERSION << "."
              << EIGEN_MAJOR_VERSION << "."
              << EIGEN_MINOR_VERSION << std::endl;

    // spdlog
    std::cout << "spdlog: " << SPDLOG_VER_MAJOR << "."
              << SPDLOG_VER_MINOR << "."
              << SPDLOG_VER_PATCH << std::endl;

    // fmt
    std::cout << "fmt: " << FMT_VERSION / 10000 << "."
              << FMT_VERSION / 100 % 100 << "."
              << FMT_VERSION % 100 << std::endl;

    std::cout << "\n所有依赖库已正确安装！" << std::endl;
    return 0;
}
```

编译运行：

```bash
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=[vcpkg]/scripts/buildsystems/vcpkg.cmake
cmake --build build --config Release
./build/Release/test_dependencies.exe
```

预期输出：
```
=== 依赖库版本检查 ===
OpenCASCADE: 7.9.1
Boost: 1.88.0
Eigen: 3.4.0
spdlog: 1.15.3
fmt: 11.2.0

所有依赖库已正确安装！
```

---

## 🆘 常见问题

### Q1: 版本不匹配会怎样？

**A**: `FeatureRecognizerConfig.cmake` 会在 find_package 时检查最低版本要求。如果版本过低，会显示错误信息并提示如何安装。

### Q2: 可以使用更高版本的依赖吗？

**A**:
- ✅ **小版本升级** (如 OpenCASCADE 7.9.1 → 7.9.2) - 通常安全
- ⚠️ **次版本升级** (如 Boost 1.88 → 1.89) - 应该兼容，建议测试
- ❌ **主版本升级** (如 Eigen 3.x → 4.x) - 可能不兼容

### Q3: vcpkg 安装的版本与编译版本不同怎么办？

**A**: vcpkg 会根据其仓库状态安装最新稳定版。如果版本差异较大：
1. 固定 vcpkg 版本到特定 commit（推荐）
2. 使用 vcpkg baseline 功能
3. 接受新版本并进行兼容性测试

### Q4: 如何固定 vcpkg 包版本？

**A**: 在项目根目录创建 `vcpkg.json`：

```json
{
  "name": "my-project",
  "version": "1.0.0",
  "dependencies": [
    { "name": "opencascade", "version>=": "7.9.1" },
    { "name": "boost-filesystem", "version>=": "1.88.0" },
    { "name": "boost-system", "version>=": "1.88.0" },
    { "name": "eigen3", "version>=": "3.4.0" },
    { "name": "spdlog", "version>=": "1.15.3" },
    { "name": "fmt", "version>=": "11.2.0" }
  ],
  "overrides": [
    { "name": "fmt", "version": "11.2.0" }
  ]
}
```

---

## 📞 技术支持

如遇到依赖相关问题：

1. 检查 vcpkg 安装的版本：`vcpkg list`
2. 查看 CMake 找到的版本：添加 `--debug-find` 选项
3. 参考项目文档：`INSTALL_说明.md`

---

## 📝 更新历史

### 2025-11-04
- 添加 fmt 11.2.0 依赖库文档
- 更新所有安装命令包含 fmt
- 添加 fmt 版本固定说明（解决 DLL 版本不匹配问题）
- 更新测试程序和 vcpkg.json 示例

### 2025-10-21
- 初始版本，记录所有依赖库版本
- 添加版本检查到 CMake 配置
- 创建详细的安装和验证说明
