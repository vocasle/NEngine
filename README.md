# NEngine

This is my personal game engine. It is continuos work in progress.

I have started this project to learn internals of game engine.

I have strived to use third party libs as little as possible. Because the 
main goal is to have a firm understanding on how everything works under the hood.

Well, that was my goal 6 months ago. Right now I have a good grasp of core engine subsystems.

Recently I have switched to Vulkan. I have removed everything and started from scratch.
Right now I use SDL2 for platform indpendent window creation and user input processing.

Game engine is written for Windows and utilizes Vulkan.

List of third party libs:
1. ImGui
2. mikktspace
3. stb_image
4. SDL2
5. tinyobjloader
6. glm

**How to build**

**Windows**

Install Vulkan SDK.

Open CMake project in Visual Studio and build nengine executable.

**Linux**

Install Vulkan SDK, SDL2.

CMake will assume that it is possible to find Vulkan and SDL2 with `find_package`.

Execute:

    cmake -S . -B build-debug -DCMAKE_BUILD_TYPE=Debug
    cmake --build build-debug

There is an old brach `dx11` that is not supported by me. It uses DirecX 11 and contains
enough facilities to start a simple 3D game development.

From now on only `vulkan` branch will be supported.

## Roadmap

### Utility
1. Split execution into several threads. Probably for now it is enough to have two threads (Game and Render) and switch all I/O to async.
2. Save scene to some textual file and parse it on load to restore scene.
3. Add in-house format for meshes.

### Renderer
1. Add PBR for BasePass.
2. Add frustum culling.
3. Add shadows.
4. Add fog.
5. Add bloom.
6. Add particles.

#### Vulkan
Find out how those concepts work:
* Push constants
* Instanced rendering
* Dynamic uniforms
* Separate images and sampler descriptors
* Pipeline cache
* Multi-threaded command buffer generation
* Multiple subpasses
* Compute shaders

### Editor
1. Support scene saving/loading.
2. Support moving of the objects in the scene via CLI or GUI.
3. Highlight object selected in the Editor.

### Sandbox
1. Add basic scene that contains a pawn, few characters, few trees, few rocks or anything else. It should be possible to walk on the scene.
