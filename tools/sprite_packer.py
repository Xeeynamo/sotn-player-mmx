#!/usr/bin/env python3
"""Sprite atlas packer for particle sprites.

Reads assets/sprite_particles.yaml, crops and packs individual sprite PNGs
into 128x128 4bpp indexed atlases, and generates C sprite definitions.

Usage: python3 tools/sprite_packer.py assets/sprite_particles.yaml src/pl_particles.c
"""

import os
import sys
import yaml
from PIL import Image


ATLAS_SIZE = 128
# Place sprites in [1, 126] to avoid UV boundary edge cases
MARGIN = 1
USABLE_SIZE = ATLAS_SIZE - 2 * MARGIN  # 126


class Rect:
    __slots__ = ("x", "y", "w", "h")

    def __init__(self, x, y, w, h):
        self.x = x
        self.y = y
        self.w = w
        self.h = h


class MaxRectsPacker:
    """MaxRects bin packing with Best-Short-Side-Fit heuristic."""

    def __init__(self):
        self.free_rects = [Rect(MARGIN, MARGIN, USABLE_SIZE, USABLE_SIZE)]
        self.used_rects = []

    def insert(self, w, h):
        """Try to insert a rectangle of size w x h. Returns (x, y) or None."""
        best_x, best_y = 0, 0
        best_short = float("inf")
        best_long = float("inf")
        best_idx = -1

        for i, fr in enumerate(self.free_rects):
            if fr.w >= w and fr.h >= h:
                leftover_x = fr.w - w
                leftover_y = fr.h - h
                short = min(leftover_x, leftover_y)
                long = max(leftover_x, leftover_y)
                if short < best_short or (short == best_short and long < best_long):
                    best_x = fr.x
                    best_y = fr.y
                    best_short = short
                    best_long = long
                    best_idx = i

        if best_idx == -1:
            return None

        placed = Rect(best_x, best_y, w, h)
        self._split_free_rects(placed)
        self._prune_free_rects()
        self.used_rects.append(placed)
        return (best_x, best_y)

    def _split_free_rects(self, placed):
        new_free = []
        i = 0
        while i < len(self.free_rects):
            fr = self.free_rects[i]
            if not self._intersects(fr, placed):
                i += 1
                continue
            # Remove this free rect and generate up to 4 new ones
            self.free_rects.pop(i)

            # Left
            if placed.x > fr.x:
                new_free.append(Rect(fr.x, fr.y, placed.x - fr.x, fr.h))
            # Right
            if placed.x + placed.w < fr.x + fr.w:
                new_free.append(
                    Rect(
                        placed.x + placed.w,
                        fr.y,
                        (fr.x + fr.w) - (placed.x + placed.w),
                        fr.h,
                    )
                )
            # Top
            if placed.y > fr.y:
                new_free.append(Rect(fr.x, fr.y, fr.w, placed.y - fr.y))
            # Bottom
            if placed.y + placed.h < fr.y + fr.h:
                new_free.append(
                    Rect(
                        fr.x,
                        placed.y + placed.h,
                        fr.w,
                        (fr.y + fr.h) - (placed.y + placed.h),
                    )
                )
        self.free_rects.extend(new_free)

    def _prune_free_rects(self):
        """Remove free rects that are fully contained by another."""
        i = 0
        while i < len(self.free_rects):
            j = i + 1
            removed = False
            while j < len(self.free_rects):
                if self._contains(self.free_rects[j], self.free_rects[i]):
                    self.free_rects.pop(i)
                    removed = True
                    break
                if self._contains(self.free_rects[i], self.free_rects[j]):
                    self.free_rects.pop(j)
                else:
                    j += 1
            if not removed:
                i += 1

    @staticmethod
    def _intersects(a, b):
        return not (
            a.x >= b.x + b.w or a.x + a.w <= b.x or a.y >= b.y + b.h or a.y + a.h <= b.y
        )

    @staticmethod
    def _contains(outer, inner):
        return (
            inner.x >= outer.x
            and inner.y >= outer.y
            and inner.x + inner.w <= outer.x + outer.w
            and inner.y + inner.h <= outer.y + outer.h
        )


def crop_to_content(img):
    """Crop image to non-transparent bounding box. Returns (cropped_img, crop_x, crop_y)."""
    pixels = img.load()
    w, h = img.size
    min_x, min_y = w, h
    max_x, max_y = -1, -1

    for y in range(h):
        for x in range(w):
            if pixels[x, y] != 0:
                min_x = min(min_x, x)
                min_y = min(min_y, y)
                max_x = max(max_x, x)
                max_y = max(max_y, y)

    if max_x < 0:
        # Fully transparent - return 1x1
        return img.crop((0, 0, 1, 1)), 0, 0

    cropped = img.crop((min_x, min_y, max_x + 1, max_y + 1))
    return cropped, min_x, min_y


def try_split_sprite(img):
    """Try to split a sparse sprite into sub-rectangles for better packing.

    Returns list of (sub_img, sub_x, sub_y) tuples.
    Only splits if fill ratio < 50% and area > 200, and splitting saves space.
    """
    pixels = img.load()
    w, h = img.size
    area = w * h

    if area <= 200:
        return [(img, 0, 0)]

    # Count non-transparent pixels
    filled = sum(1 for y in range(h) for x in range(w) if pixels[x, y] != 0)
    fill_ratio = filled / area if area > 0 else 1.0

    if fill_ratio >= 0.5:
        return [(img, 0, 0)]

    # Try splitting by rows (find longest transparent band)
    best_split = None
    best_savings = 0

    # Try horizontal splits
    for y in range(1, h):
        # Check if row y is fully transparent
        band_start = y
        while y < h and all(pixels[x, y] == 0 for x in range(w)):
            y += 1
        band_len = y - band_start
        if band_len == 0:
            continue

        # Calculate savings from splitting here
        top_h = band_start
        bot_h = h - y
        if top_h <= 0 or bot_h <= 0:
            continue

        # Check top and bottom have content
        top_img = img.crop((0, 0, w, top_h))
        bot_img = img.crop((0, y, w, h))
        top_cropped, tx, ty = crop_to_content(top_img)
        bot_cropped, bx, by = crop_to_content(bot_img)

        sub_area = (
            top_cropped.size[0] * top_cropped.size[1]
            + bot_cropped.size[0] * bot_cropped.size[1]
        )
        savings = area - sub_area

        if sub_area < area * 0.8 and savings > best_savings:
            best_savings = savings
            best_split = (
                "h",
                [(top_cropped, tx, ty), (bot_cropped, bx, y + by)],
            )

    # Try vertical splits
    for x in range(1, w):
        band_start = x
        while x < w and all(pixels[x, y] == 0 for y in range(h)):
            x += 1
        band_len = x - band_start
        if band_len == 0:
            continue

        left_w = band_start
        right_w = w - x
        if left_w <= 0 or right_w <= 0:
            continue

        left_img = img.crop((0, 0, left_w, h))
        right_img = img.crop((x, 0, w, h))
        left_cropped, lx, ly = crop_to_content(left_img)
        right_cropped, rx, ry = crop_to_content(right_img)

        sub_area = (
            left_cropped.size[0] * left_cropped.size[1]
            + right_cropped.size[0] * right_cropped.size[1]
        )
        savings = area - sub_area

        if sub_area < area * 0.8 and savings > best_savings:
            best_savings = savings
            best_split = (
                "v",
                [(left_cropped, lx, ly), (right_cropped, x + rx, ry)],
            )

    if best_split:
        return best_split[1]

    return [(img, 0, 0)]


def sanitize_name(png_path):
    """Convert particles/buster_0.png to sprt_buster_0."""
    base = os.path.splitext(os.path.basename(png_path))[0]
    return f"sprt_{base}"


def to_pascal_case(name):
    """Convert snake_case or lowercase name to PascalCase."""
    return "".join(word.capitalize() for word in name.replace("-", "_").split("_"))


def generate_animations(config, output_dir):
    """Generate animation .inc files from the animations section of the YAML."""
    animations = config.get("animations")
    if not animations:
        return

    for anim_group in animations:
        group_name = anim_group["name"]
        pascal_name = to_pascal_case(group_name)
        hitboxes = anim_group["hitboxes"]
        anims = anim_group["anims"]

        lines = []
        lines.append("// Auto-generated by tools/sprite_packer.py - DO NOT EDIT")

        # Generate enum
        lines.append(f"typedef enum {{")
        for anim in anims:
            enum_entry = f"{pascal_name}_{to_pascal_case(anim['name'])}"
            lines.append(f"    {enum_entry},")
        lines.append(f"}} {pascal_name}Anims;")

        # Generate hitboxes array
        hitbox_strs = [
            "{" + ", ".join(str(v) for v in hb) + "}" for hb in hitboxes
        ]
        lines.append(
            f"static s8 {group_name}_hitboxes[][4] = {{{', '.join(hitbox_strs)}}};"
        )

        # Generate animation frame arrays
        for anim in anims:
            anim_name = anim["name"]
            var_name = f"{group_name}_anim_{anim_name}"
            poses = anim["poses"]
            loop_index = anim.get("loop")

            pose_strs = []
            for pose in poses:
                duration = pose["duration"]
                frame = pose["frame"]
                hitbox = pose["hitbox"]
                pose_strs.append(f"POSE({duration}, {frame}, {hitbox})")

            if loop_index is not None:
                pose_strs.append(f"POSE_LOOP({loop_index})")
            else:
                pose_strs.append("POSE_END")

            lines.append(
                f"static AnimationFrame {var_name}[] = {{{', '.join(pose_strs)}}};"
            )

        # Generate pointer array
        lines.append(f"static AnimationFrame* {group_name}_anims[] = {{")
        for anim in anims:
            lines.append(f"    {group_name}_anim_{anim['name']},")
        lines.append("};")

        out_path = os.path.join(output_dir, f"anims_{group_name}.inc")
        with open(out_path, "w") as f:
            f.write("\n".join(lines) + "\n")
        print(f"  Generated {out_path}")


def main():
    if len(sys.argv) != 3:
        print(f"Usage: {sys.argv[0]} <yaml> <output_c>", file=sys.stderr)
        sys.exit(1)

    yaml_path = sys.argv[1]
    output_c = sys.argv[2]
    assets_dir = os.path.dirname(yaml_path)
    # Atlas PNGs are saved next to the YAML: sprite_particles.yaml -> sprite_particles.png
    atlas_base = os.path.splitext(yaml_path)[0]

    with open(yaml_path) as f:
        config = yaml.safe_load(f)

    tpage = config["tpage"]
    quadrant = config.get("quadrant", 0)
    default_pal = config.get("palette", "PAL_PARTICLES")

    # Step 1: Load and deduplicate images
    # Map from PNG path to (cropped_img, crop_x, crop_y, sub_pieces)
    image_cache = {}

    for sprite in config["sprites"]:
        png_path = sprite["name"]
        if png_path in image_cache:
            continue

        full_path = os.path.join(assets_dir, png_path)
        img = Image.open(full_path)
        if img.mode != "P":
            print(f"Warning: {png_path} is not indexed, converting", file=sys.stderr)
            img = img.convert("P")

        cropped, crop_x, crop_y = crop_to_content(img)
        pieces = try_split_sprite(cropped)

        # Each piece: (sub_img, sub_x_in_cropped, sub_y_in_cropped)
        image_cache[png_path] = {
            "crop_x": crop_x,
            "crop_y": crop_y,
            "pieces": pieces,
        }

    # Step 2: Collect all unique pieces to pack
    # piece_key -> (sub_img, w, h)
    pack_items = []
    piece_to_pack_idx = {}

    for png_path, data in image_cache.items():
        for pi, (sub_img, sub_x, sub_y) in enumerate(data["pieces"]):
            key = (png_path, pi)
            sw, sh = sub_img.size
            piece_to_pack_idx[key] = len(pack_items)
            pack_items.append(
                {
                    "key": key,
                    "img": sub_img,
                    "w": sw,
                    "h": sh,
                    "atlas": -1,
                    "ax": 0,
                    "ay": 0,
                }
            )

    # Sort by area descending for better packing
    sorted_indices = sorted(
        range(len(pack_items)),
        key=lambda i: pack_items[i]["w"] * pack_items[i]["h"],
        reverse=True,
    )

    # Step 3: Pack into atlases using MaxRects
    atlases = []
    current_packer = MaxRectsPacker()
    current_atlas_idx = 0
    atlases.append(current_packer)

    for idx in sorted_indices:
        item = pack_items[idx]
        result = current_packer.insert(item["w"], item["h"])
        if result is None:
            # Start new atlas
            current_atlas_idx += 1
            pass  # New atlas started
            current_packer = MaxRectsPacker()
            atlases.append(current_packer)
            result = current_packer.insert(item["w"], item["h"])
            if result is None:
                print(
                    f"Error: piece {item['key']} ({item['w']}x{item['h']}) "
                    f"doesn't fit in empty atlas!",
                    file=sys.stderr,
                )
                sys.exit(1)
        item["atlas"] = current_atlas_idx
        item["ax"] = result[0]
        item["ay"] = result[1]

    num_atlases = len(atlases)
    print(f"Packed {len(pack_items)} pieces into {num_atlases} atlas(es)")

    # Step 4: Generate atlas images
    # Get palette from first image
    ref_palette = None
    for item in pack_items:
        if item["img"].mode == "P":
            ref_palette = item["img"].getpalette()
            break

    for atlas_idx in range(num_atlases):
        atlas = Image.new("P", (ATLAS_SIZE, ATLAS_SIZE), 0)
        if ref_palette:
            atlas.putpalette(ref_palette)

        for item in pack_items:
            if item["atlas"] != atlas_idx:
                continue
            atlas.paste(item["img"], (item["ax"], item["ay"]))

        if atlas_idx == 0:
            out_path = f"{atlas_base}.png"
        else:
            out_path = f"{atlas_base}_{atlas_idx}.png"
        atlas.save(out_path)
        print(f"  Saved {out_path}")

    # Step 5: Generate C code
    lines = []
    lines.append("// Auto-generated by tools/sprite_packer.py - DO NOT EDIT")
    lines.append('#include "pl.h"')
    lines.append("")
    lines.append("#define PAGE(quadrant, half) ((quadrant) << 2 | ((half) & 3))")
    lines.append("")

    # Track variable names to handle duplicates
    used_names = {}
    sprite_vars = []  # (var_name, sprite_entry) pairs

    for sprite in config["sprites"]:
        png_path = sprite["name"]
        data = image_cache[png_path]
        user_cx = sprite.get("cx", 0)
        user_cy = sprite.get("cy", 0)
        pal = sprite.get("pal", default_pal)
        flipx = sprite.get("flipx", False)

        base_name = sanitize_name(png_path)
        # Handle duplicate names (same PNG referenced multiple times)
        if base_name in used_names:
            used_names[base_name] += 1
            var_name = f"{base_name}_{used_names[base_name]}"
        else:
            used_names[base_name] = 0
            var_name = base_name

        pieces = data["pieces"]
        crop_x = data["crop_x"]
        crop_y = data["crop_y"]
        count = len(pieces)
        flags = 2 if flipx else 0

        parts = []
        for pi, (sub_img, sub_x, sub_y) in enumerate(pieces):
            key = (png_path, pi)
            pack_idx = piece_to_pack_idx[key]
            item = pack_items[pack_idx]
            sw, sh = item["w"], item["h"]
            atlas_idx = item["atlas"]

            sx = item["ax"]
            sy = item["ay"]
            ex = sx + sw
            ey = sy + sh

            abs_quadrant = quadrant + atlas_idx
            cur_tpage = tpage + abs_quadrant // 4
            cur_quadrant = abs_quadrant % 4
            page_val = f"PAGE(0x{cur_tpage:02X}, {cur_quadrant})"

            # Calculate cx/cy
            cx = user_cx + crop_x + sub_x
            cy = user_cy + crop_y + sub_y

            if flipx:
                cx = -(user_cx + crop_x + sub_x + sw)

            parts.append(
                f"    {flags}, {cx}, {cy}, {sw}, {sh}, {pal}, {page_val}, {sx}, {sy}, {ex}, {ey},"
            )

        lines.append(f"static u16 {var_name}[] = {{")
        lines.append(f"    {count},")
        for part in parts:
            lines.append(part)
        lines.append("};")

        sprite_vars.append(var_name)

    # Generate pointer array
    lines.append("")
    lines.append("SpriteParts* sprite_particles[] = {")
    lines.append("    NULL,")
    for var_name in sprite_vars:
        lines.append(f"    (SpriteParts*){var_name},")
    lines.append("};")
    lines.append("")

    with open(output_c, "w") as f:
        f.write("\n".join(lines))
    print(f"  Generated {output_c} with {len(sprite_vars)} sprites")

    # Step 6: Generate animation .inc files
    anims_output_dir = os.path.join(os.path.dirname(output_c), "assets")
    os.makedirs(anims_output_dir, exist_ok=True)
    generate_animations(config, anims_output_dir)


if __name__ == "__main__":
    main()
