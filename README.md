# NEngine

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