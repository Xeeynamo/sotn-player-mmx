# Mega Dracula X

Mega Man X in Castlevania: Symphony of the Night

This is a proof of concept to set the fundamentals of adding custom playable characters in Castlevania: Symphony of the Night.

I invite developers to use this repository, create a fork and start iterating on this existing source for their custom characters. I strongly encourage to keep your custom characters open source. More on that in the paragraph down below.

## Build guide

As a minimum requirement, clone [sotn-decomp](https://github.com/Xeeynamo/sotn-decomp) to the directory above, so your fold structure will look like the following:

```plain
repo/
├── sotn-decomp/
└── sotn-player-mmx/
```

### Build guide via GCC

This is the easiest way to build your character for the PlayStation 1 version of SOTN. Follow the [Build guide](https://github.com/Xeeynamo/sotn-decomp/wiki/Build) and ensure you can build `sotn-decomp` locally. You need to do this at least once.

Now you can just build your custom character with `make`.

### Build guide via CMake

This is an advanced and work-in-progress technique to build your custom character cross-platform and target it to multiple platforms such as PlayStation 1, PlayStation Portable and so on.

Open `../sotn-decomp/CMakeLists.txt` and put the following lines of text right before `add_library(core ${SOURCE_FILES_CORE})`:

```cmake
set(SOTN_MMX_DIR ${CMAKE_SOURCE_DIR}/../sotn-player-mmx)
include(${SOTN_MMX_DIR}/mmx.cmake)
list(TRANSFORM SOURCE_FILES_MMX PREPEND ${SOTN_MMX_DIR}/)
set(SOURCE_FILES_CORE ${SOURCE_FILES_CORE} ${SOURCE_FILES_MMX})
```

Follow the build guide for GCC and run `make` at least before invoking `cmake`.

## Developing a custom character

It is highly recommended to start developing your custom character by forking this existing repository. This will automatically provide you all the fundamentals to [distribute your character](https://github.com/xeeynamo/sotn-player-mmx/releases), [track bugs](https://github.com/xeeynamo/sotn-player-mmx/issues), allow people to [propose tweaks and enhancements](https://github.com/xeeynamo/sotn-player-mmx/pulls), maintain a [list of custom characters for SOTN](https://github.com/Xeeynamo/sotn-player-mmx/forks) and to spread the knowledge on how you made your unique character so amazing. It is also a way to give back to the authors of [sotn-decomp](https://github.com/Xeeynamo/sotn-decomp/graphs/contributors).

### Creating sprites

All the sprites are contained in the `sprites/` folder and collected via the file `sprites/config.json`, which will describe what are the X and Y coordinates to the center of the sprite. All the sprites need to be 4bpp 16-colors.

You might find easier to work on a spritesheet instead of individual sprites, especially when it comes on ensuring the character remains centered on the screen and it does not warp on every frame. This is done by the command `make spritesheet`. The command will generate a file called `spritesheet.png`, which contains all the frames in a single PNG file to work with. This spritesheet will also need to use maximum 16 colors. The palette used will be the one from the first frame in `sprites/`. Each sprite is in a grid of 96x96 and this behaviour can be changed in `tools/spritesheet.py`.

I highly recommend the tool [Aseprite](https://github.com/aseprite/aseprite) to edit your sprites (it is free if you compile it your own) when modifying `spritesheet.png`. Aseprite will allow you to paste frames in your spritesheet while retaining the palette order and automatically mapping the colors.

To split your spritesheet back into small frames, please do a `make split`. This will replace all the content in `sprites/` with the content of your `spritesheet.png`, so be careful.

If your character individual frames warp during the animation, adjust the X and Y values in `sprites/config.json`.

### Creating your custom palette

Your custom palette is located at `assets/pal.yaml` where a list of palette files is held. Doing a `make` will create the file `src/assets/pal.inc`, which can be included and used in-game. Each file is a simple JASC-PAL file that can be used from aseprite or any text editor.

By default, this player sample uses `PLAYER.palette = 0x8120;`. Ignore that `0x8000` and focus on `0x120`. That is the palette entry that starts at `g_Clut + 0x1200`, which is why this sample code is doing `memcpy(g_Clut + 0x1200, pl_palettes, sizeof(pl_palettes));` when installing the palette array. If you want to use the second palette from `pal.yaml` then you need to programmatically change the palette with ``PLAYER.palette = 0x8121;` and so on.

If you want your character to use an animated palette, create a function to call at every frame that contains the following:

```c
RECT vramPalette = {0, 240, 256, 16};
LoadImage(&vramPalette, g_Clut + 0x1000);
```

Modify the palette data starting from `g_Clut[0x1200]`. Remember this is an expensive operation for the PSX hardware. You can tweak the `RECT` to upload a smaller portion of data instead of the whole palette.

### Creating animations

Animations are defined in `src/pl_anims.c` and they are in the following format:

```c
static AnimationFrame anim_fall[] = {
    {8, FRAME(20, 4)},
    {80, FRAME(21, 4)},
    A_END};
```

The first number is the amount of half frames the frame will spent being on the screen before the animation system moves to the next frame. If you want your frame to be in the screen for four frames, then put `8`, exactly double the amount.

The next part `FRAME` accepts two parameters: the Frame ID and the Hitbox ID. Specifying the Frame ID `20` will take the 20th frame in `sprites/` and draw it on screen.

Every animation needs a terminator. You can use one of the following animation terminators:

* `A_END` will freeze the animation to the last frame
* `A_LOOP_AT` will loop into the existing animation starting from the specified animation index (or row).
* `A_JUMP_AT` will terminate the current animation and jump to another animation with its own set of frame definitions.

## Distributing your custom character

TODO: CI
