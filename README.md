Graphics Submission  - Ryan Holt

Running the File

By default the camera will move around the scene. The following keyboard commands can also be used.

- P: Stop the passage of time
- O: Resume the passage of time
- K: Make time go in backwards
- L: Make time go forwards
- Q: Stop the autocamera
- The camera can then be moved using the mouse and WASD

During the day the mountains far off in the distance can be seen reflected off the water

NOTE: The .exe needs to be kept exactly one folder deeper than the project root (At the time of making I did not know how to use CMake to use absolute pathing to project root). In the repository it is kept in /x64, but if rebuilt it will move into /x64/Debug. It can still run when launched from VS potentially but the .exe should be moved back.

NOTE2: The startup project should be Graphics Coursework/Blank Project

NOTE3: The console may be filled with *OpenGL Debug Output: Source(OpenGL), Type(Error), Priority(High), Using glTexParameteri in a Core context with parameter <param> and enum '0x2900' which was removed from Core OpenGL (GL\_INVALID\_ENUM)*. This program was made in VS2019 and this output appears to be related to having since built the project in VS2022 from my (limited) testing. The scene still renders as intended however.

Third Party Assets

- Grass Texture: [Grass Textures - Set 2 | OpenGameArt.org](https://opengameart.org/content/tileable-grass-textures-set-2)
- Rock Texture: [Generic rock texture | OpenGameArt.org](https://opengameart.org/content/generic-rock-texture)
- Marble Texture: [Pixel-Furnace | Marble Tiles Texture](https://textures.pixel-furnace.com/texture?name=Marble%20Tiles)
- Heightmap: [Landscape Height Maps - Motion Forge Pictures](https://www.motionforgepictures.com/height-maps/)

Everything else provided by NU
