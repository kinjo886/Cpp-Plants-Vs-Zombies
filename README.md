# 🧟‍♂️ C++ 植物大战僵尸 (Plants vs. Zombies) - EasyX 复刻版

  基于 C++ 和 EasyX 图形库开发的高还原度塔防游戏。
  本项目不依赖现成游戏引擎，从底层实现了图形渲染算法、向量数学库及游戏核心逻辑。

## 📖 项目简介

本项目完全复刻了经典游戏《植物大战僵尸》的核心体验。本项目是基于 Bilibili 博主 **程序员Rock** 的教程视频《【程序员Rock】C语言项目：完整版植物大战僵尸！可能是B站最好的植物大战僵尸教程了！》开发的一款桌面休闲游戏。通过使用 C 语言结合 EasyX 图形库，还原了经典游戏《植物大战僵尸》的核心逻辑与玩法。
与简单的 Demo 不同，本项目底层封装了自定义的图形渲染管线以支持 PNG 透明通道，并构建了向量数学库来处理游戏实体的运动与碰撞。

游戏包含了完整的开场运镜动画、UI 交互、阳光资源管理、植物种植防御及僵尸进攻逻辑。

## 🎮 游戏演示

<!-- 💡 建议运行程序后截图替换此处图片 -->

## ✨ 核心功能与技术亮点

1. 底层图形技术 (tools.cpp)

- 透明通道渲染：由于 EasyX 原生对 PNG 透明度支持有限，项目中手动实现了基于 AlphaBlend 算法的 putimagePNG 函数。

- 像素级混合：通过位运算处理 ARGB 数据，实现了完美的抗锯齿边缘和半透明阴影效果。

2. 数学物理库 (vector2.cpp)

- 向量封装：独立封装了 vector2 结构体，重载了 +, -, *, / 运算符。

- 几何运算：实现了点积、叉积、模长计算及贝塞尔曲线算法，用于处理子弹轨迹和实体碰撞检测。

3. 游戏逻辑系统 (main.cpp)

- 完整流程控制：

  - 开场动画：实现了类似原版的背景卷动 (startUI) 和卡槽下落动画 (barsDown)。

  - 状态机管理：管理游戏状态（进行中 GOING、胜利 WIN、失败 FAIL）。

- 实体交互：

  - 植物系统：目前实现了 豌豆射手、向日葵、大嘴花 三种核心植物。

  - 僵尸 AI：实现了僵尸的生成、移动、捕食植物及死亡判定。

  - 资源系统：实现了阳光的生成与拾取逻辑。

## 🛠️ 技术栈

- 开发语言: C++ (STL)

- 图形库: EasyX Graphics Library

- 系统 API: Windows GDI (MSIMG32.LIB), Windows Multimedia (Winmm.lib)

- 开发环境: Visual Studio 2022

## 📂 目录结构
```text
PlantsVsZombies/
├── 📄 main.cpp           # 游戏主入口，包含主循环与核心逻辑
├── 📄 vector2.h/.cpp     # 自定义二维向量库 (数学物理基础)
├── 📄 tools.h/.cpp       # 绘图工具库 (PNG透明贴图实现)
├── 📂 res/               # 游戏素材 (图片/音效)
└── 📄 PvZ.sln            # Visual Studio 解决方案
```

## 🚀 如何运行 (How to Run)

1. 环境准备：

- 安装 Visual Studio 2022。

- 安装 EasyX 图形库。

2.克隆代码：
```
git clone [https://github.com/kinjo886/Cpp-Plants-Vs-Zombies.git](https://github.com/kinjo886/Cpp-Plants-Vs-Zombies.git)
```

3. 编译配置：

- 使用 Visual Studio 打开 .sln 文件。

- 确保项目链接了必要的库（代码中已通过 #pragma comment 自动链接）：

  - Winmm.lib (用于音效播放)

  - MSIMG32.LIB (用于 AlphaBlend 透明绘图)

- 建议选择 Release / x86 模式以获得最佳性能。

4. 运行：点击“本地 Windows 调试器”启动游戏。

## 致谢与版权说明
本项目代码参考并跟随 Bilibili 教学视频编写，特此向原作者表示感谢：

- 教程来源：[程序员Rock - Bilibili](https://space.bilibili.com/485705286?spm_id_from=333.788.upinfo.detail.click)

- 视频链接：[【程序员Rock】C语言项目：完整版植物大战僵尸！](https://www.bilibili.com/video/BV1vM4y1X7Kb/?spm_id_from=333.337.search-card.all.click&vd_source=82e0699084c38bc7273728d632ce1e0a)

**声明**：本项目仅用于个人学习和编程技术交流，不涉及任何商业用途。游戏素材版权归原制作方（PopCap Games/EA）所有。

👨‍💻 作者信息
吴俊 (Wu Jun)

- Email: 23jwu2@stu.edu.cn

- Portfolio: [访问我的主页](https://github.com/kinjo886/JIANLI_CV)

