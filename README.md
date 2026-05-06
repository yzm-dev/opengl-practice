# OpenGL Practice

基于 OpenGL 4.1 的学习项目，使用 CMake Presets + Ninja Multi-Config 构建，每个子目录是一个独立可运行的子项目。

## 依赖

| 依赖 | 说明 |
|---|---|
| CMake ≥ 3.2 | 构建系统，使用 `CMakePresets.json` 配置 |
| Ninja | `Ninja Multi-Config` 构建生成器 |
| GLFW 3.3+ | 窗口与 OpenGL 上下文管理 |
| glad | OpenGL 4.1 Core 函数加载器（已内置于 `external/libs/glad/`） |

macOS 使用 Homebrew 安装：

```bash
brew install cmake ninja glfw
```

Linux（Debian/Ubuntu）：

```bash
sudo apt install build-essential cmake ninja-build libglfw3-dev
```

Windows 使用 [vcpkg](https://vcpkg.io) 安装 GLFW：

```powershell
$env:VCPKG_ROOT="C:\path\to\vcpkg"
vcpkg install glfw3:x64-windows
```

`windows` preset 会通过 `VCPKG_ROOT` 自动设置 vcpkg toolchain：

```json
"CMAKE_TOOLCHAIN_FILE": "$env{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
```

## Presets

查看当前平台可用的 configure presets：

```bash
cmake --list-presets
```

`CMakePresets.json` 使用 `condition` 按宿主系统筛选 preset：

| 平台 | Configure preset | Debug build preset | Release build preset |
|---|---|---|---|
| macOS | `macos` | `macos-debug` | `macos-release` |
| Linux | `linux` | `linux-debug` | `linux-release` |
| Windows | `windows` | `windows-debug` | `windows-release` |

构建目录格式为：

```text
build/<configure-preset>/
```

例如 macOS 会生成到 `build/macos/`。

## 配置与构建

macOS：

```bash
cmake --preset macos
cmake --build --preset macos-debug
```

Linux：

```bash
cmake --preset linux
cmake --build --preset linux-debug
```

Windows：

```powershell
cmake --preset windows
cmake --build --preset windows-debug
```

构建 Release：

```bash
cmake --build --preset macos-release
```

只构建某个 target：

```bash
cmake --build --preset macos-debug --target DrawTriangle
```

## 运行

macOS / Linux：

```bash
./build/macos/bin/Debug/DrawTriangle
```

Linux 将路径中的 `macos` 替换为 `linux`：

```bash
./build/linux/bin/Debug/DrawTriangle
```

Windows：

```powershell
.\build\windows\bin\Debug\DrawTriangle.exe
```

按 `ESC` 退出窗口。

## 项目结构

```
OpenGL-Practice/
├── CMakeLists.txt              # 根配置
├── external/
│   └── libs/
│       └── glad/               # OpenGL 4.1 Core 加载器（静态库）
│           ├── include/
│           └── src/
├── 1.DrawTriangle/             # 1.渲染橙色三角形
│   ├── src/main.cpp
│   └── shaders/
│       ├── triangle.vert
│       └── triangle.frag
└── 2.xxxxxx
```

## 新增子项目

1. 创建新子目录，如 `2.Textures`
2. 在其中新建 `src/main.cpp` 和 `shaders` 目录
3. 在子项目根目录添加 `CMakeLists.txt`，配置内容：
   ```cmake
   add_opengl_executable(Textures src/main.cpp)
   ```
4. 在项目根目录 `CMakeLists.txt` 末尾追加：
   ```cmake
   add_subdirectory(2.Textures)
   ```

CMake 会自动将该课程的 `shaders/` 绝对路径注入为 `SHADER_DIR` 宏，供代码中直接用。
