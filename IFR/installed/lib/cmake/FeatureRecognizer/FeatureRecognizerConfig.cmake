#-----------------------------------------------------------------------------
# FeatureRecognizerConfig.cmake
# CMake配置文件 - 用于外部项目查找和使用 FeatureRecognizer 库
#
# 使用方法:
#   find_package(FeatureRecognizer REQUIRED)
#   target_link_libraries(myapp PRIVATE FeatureRecognizer::featureRecognizer)
#-----------------------------------------------------------------------------


####### Expanded from @PACKAGE_INIT@ by configure_package_config_file() #######
####### Any changes to this file will be overwritten by the next CMake run ####
####### The input file was FeatureRecognizerConfig.cmake.in                            ########

get_filename_component(PACKAGE_PREFIX_DIR "${CMAKE_CURRENT_LIST_DIR}/../../../" ABSOLUTE)

macro(set_and_check _var _file)
  set(${_var} "${_file}")
  if(NOT EXISTS "${_file}")
    message(FATAL_ERROR "File or directory ${_file} referenced by variable ${_var} does not exist !")
  endif()
endmacro()

macro(check_required_components _NAME)
  foreach(comp ${${_NAME}_FIND_COMPONENTS})
    if(NOT ${_NAME}_${comp}_FOUND)
      if(${_NAME}_FIND_REQUIRED_${comp})
        set(${_NAME}_FOUND FALSE)
      endif()
    endif()
  endforeach()
endmacro()

####################################################################################

# 版本信息
set(FeatureRecognizer_VERSION_MAJOR 0)
set(FeatureRecognizer_VERSION_MINOR 7)
set(FeatureRecognizer_VERSION_PATCH 3)
set(FeatureRecognizer_VERSION "0.7.3")

#-----------------------------------------------------------------------------
# 查找所有依赖库 (使用 vcpkg CONFIG 模式)
#-----------------------------------------------------------------------------
#
# FeatureRecognizer 库编译时使用的依赖版本：
#   - OpenCASCADE: 7.9.1
#   - Boost:       1.88.0
#   - Eigen3:      3.4.0
#   - spdlog:      1.15.3
#
# 建议使用相同或兼容版本。使用 vcpkg 安装:
#   vcpkg install opencascade:x64-windows-meshlib
#   vcpkg install boost-filesystem:x64-windows-meshlib
#   vcpkg install boost-system:x64-windows-meshlib
#   vcpkg install eigen3:x64-windows-meshlib
#   vcpkg install spdlog:x64-windows-meshlib
#-----------------------------------------------------------------------------

# OpenCASCADE - 必需组件
# 编译时版本: 7.9.1
# 注意: 由于 OpenCASCADE 使用 ExactVersion 策略，这里不指定版本号以兼容不同版本
# 建议使用 7.8.0 或更高版本
find_package(OpenCASCADE REQUIRED CONFIG
  COMPONENTS
    FoundationClasses
    ModelingData
    ModelingAlgorithms
    Visualization
    ApplicationFramework
    DataExchange
)

if(NOT OpenCASCADE_FOUND)
  set(FeatureRecognizer_NOT_FOUND_MESSAGE
      "FeatureRecognizer requires OpenCASCADE >= 7.8.0, which was not found.\n\
       Compiled with: OpenCASCADE 7.9.1\n\
       Install with: vcpkg install opencascade:x64-windows-meshlib")
  set(FeatureRecognizer_FOUND FALSE)
  return()
endif()

# Eigen3 - 线性代数库
# 编译时版本: 3.4.0
# 最低兼容版本: 3.3.0
find_package(Eigen3 3.3.0 CONFIG REQUIRED)
if(NOT Eigen3_FOUND)
  set(FeatureRecognizer_NOT_FOUND_MESSAGE
      "FeatureRecognizer requires Eigen3 >= 3.3.0, which was not found.\n\
       Compiled with: Eigen3 3.4.0\n\
       Install with: vcpkg install eigen3:x64-windows-meshlib")
  set(FeatureRecognizer_FOUND FALSE)
  return()
endif()

# Boost - 文件系统和系统库
# 编译时版本: 1.88.0
# 最低兼容版本: 1.70.0
find_package(Boost 1.70.0 REQUIRED COMPONENTS filesystem system)
if(NOT Boost_FOUND)
  set(FeatureRecognizer_NOT_FOUND_MESSAGE
      "FeatureRecognizer requires Boost >= 1.70.0 (filesystem, system), which was not found.\n\
       Compiled with: Boost 1.88.0\n\
       Install with: vcpkg install boost-filesystem boost-system")
  set(FeatureRecognizer_FOUND FALSE)
  return()
endif()

# spdlog - 日志库
# 编译时版本: 1.15.3
# 最低兼容版本: 1.10.0
find_package(spdlog 1.10.0 CONFIG REQUIRED)
if(NOT spdlog_FOUND)
  set(FeatureRecognizer_NOT_FOUND_MESSAGE
      "FeatureRecognizer requires spdlog >= 1.10.0, which was not found.\n\
       Compiled with: spdlog 1.15.3\n\
       Install with: vcpkg install spdlog:x64-windows-meshlib")
  set(FeatureRecognizer_FOUND FALSE)
  return()
endif()

#-----------------------------------------------------------------------------
# 包含导出的 targets 文件
#-----------------------------------------------------------------------------
include("${CMAKE_CURRENT_LIST_DIR}/FeatureRecognizerTargets.cmake")

#-----------------------------------------------------------------------------
# 设置包含目录和库变量 (向后兼容)
#-----------------------------------------------------------------------------
get_target_property(FeatureRecognizer_INCLUDE_DIRS
                    FeatureRecognizer::featureRecognizer
                    INTERFACE_INCLUDE_DIRECTORIES)
set(FeatureRecognizer_LIBRARIES FeatureRecognizer::featureRecognizer)

#-----------------------------------------------------------------------------
# 检查所有必需的组件
#-----------------------------------------------------------------------------
check_required_components(FeatureRecognizer)

#-----------------------------------------------------------------------------
# 成功消息
#-----------------------------------------------------------------------------
if(NOT FeatureRecognizer_FIND_QUIETLY)
  message(STATUS "Found FeatureRecognizer: ${FeatureRecognizer_VERSION}")
  message(STATUS "  Include dirs: ${FeatureRecognizer_INCLUDE_DIRS}")
  message(STATUS "  Libraries: ${FeatureRecognizer_LIBRARIES}")
endif()
