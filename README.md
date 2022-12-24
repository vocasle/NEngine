# NEngine

This is my personal game engine. It is continuos work in progress.

I have started this project to learn internals of game engine.

I strive to use third party libs as little as possible. Because the 
main goal is to have a firm understanding on how everything works under the hood.

Game engine is nailed to Windows and DirectX 11. I would like to switch to 
Vulkan in future and add an abstraction layer to support Linux as well. But those are
plans for distant future.

List of third party libs:
1. ImGui
2. ImGuizmo
3. mikktspace
4. stb_image
5. stb_image_write
6. tinygltf

**How to build**

For now I use CMake + Visual Studio 2022 to build the project. 

To build with VS 2022 you just need to open the project as CMake project.

To build with CMake navigate to root of the repo and execute:

	cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -S . - B build-debug -G Ninja
	cmake --build build-debug


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

### Editor
1. Support scene saving/loading.
2. Support moving of the objects in the scene via CLI or GUI.
3. Highlight object selected in the Editor.

### Sandbox
1. Add basic scene that contains a pawn, few characters, few trees, few rocks or anything else. It should be possible to walk on the scene.
