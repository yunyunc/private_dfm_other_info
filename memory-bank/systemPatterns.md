# 系统模式

- 版本: v0.1
- 更新时间: 20260210
- 架构主线: MVVM（Model / View / ViewModel）
- 模块组织:
  - src/model: 数据模型与导入
  - src/view: 渲染与界面视图
  - src/viewmodel: 交互状态与命令协调
  - src/mvvm: Signal、Property、MessageBus 等基础设施
  - src/window、src/input、src/utils、src/ais: 窗口、输入、日志、网格数据源
- 关键模式: 事件驱动（Signal/MessageBus）、分层解耦、可测试核心库（OcctImguiLib + tests）
- 兼容性约束: 不破坏已有测试入口与 CMake preset 工作流
