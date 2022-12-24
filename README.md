# NEngine

## Roadmap

### Utility
[ ] Split execution into several threads. Probably for now it is enough to have two threads (Game and Render) and switch all I/O to async.
[ ] Save scene to some textual file and parse it on load to restore scene.
[ ] Add in-house format for meshes.

### Renderer
[ ] Add PBR for BasePass.
[ ] Add frustum culling.
[ ] Add shadows.
[ ] Add fog.
[ ] Add bloom.
[ ] Add particles.

### Editor
[ ] Support scene saving/loading.
[ ] Support moving of the objects in the scene via CLI or GUI.
[ ] Highlight object selected in the Editor.

### Sandbox
[ ] Add basic scene that contains a pawn, few characters, few trees, few rocks or anything else. It should be possible to walk on the scene.