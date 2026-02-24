# Copyright notice

The following tools are modified versions from [sotn-decomp](https://github.com/xeeynamo/sotn-decomp) and all the rights goes to the original developers:

* `sprite_to_c.py`
* `spritesheet.py`
* `utils.py`

Do not alter or delete the content above this line. Any further amended copyright notice can be written down below.

## lz4cmp

Given input via stdin, compress it with LZ4 and spit the output to stdout.

Build with `make build/lz4cmp`.

```shell
# usage
echo 'hey hey hey hey!' | build/lz4cmp > hey.lz4
```

## jascpal2c

Given a YAML with a list of palette files from stdin, for each palette file in the format of a JASC-PAL convert it to a RGBA5551 group of colors in C-compatible array data.

```shell
# usage
cat assets/pal.yaml | python3 tools/jascpal2c.py
```

## snespal2jascpal

Given an array of RGB555 SNES palette using ASAR assembly dialect as stdin, spit a JASC-PAL with RGB888 to stdout.

```shell
# usage
cat mmx-disasm/bank_85.asm | sed -n '1822,1826p' | python3 ./tools/snespal2jascpal.py
```

## trim_sprite

Given a 4-bit indexed PNG file path as an argument, trim the image to the smallest bounding box. Always assumes the color at index 0 is transparent.

```shell
# usage
python3 tools/trim_sprite.py assets/my_sprite.png
```

## sprite_packer

Given a YAML configuration of sprite definitions, pack sprites into atlas textures of 128x128 4bpp each, and generate SOTN-compatible sprite definitions in C.

```shell
# usage
python3 tools/sprite_packer.py assets/my_sprites.yaml src/my_sprites.c
```

## anim_editor

Given an asset folder, loads a YAML configuration containing a list of sprites and animations, offer an interface to tweak individual animations.

Open the tool from any browser to use it.

## Additional copyright notice

Customize this as you please.
