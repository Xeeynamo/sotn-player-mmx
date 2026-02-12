VERSION	  ?= us
PL_NAME   := mmx
FILES     := pl_header pl_sprites pl pl_assets pl_anims pl_settings
FILES     += pl_hud pl_entities
FILES     += lz4/lz4
FILES     += d_18568 spriteparts pl_sprite_particles
FILES     += 1AC60 21250 24788 26C84 2A060 319C4 bss
ASSETS	  += hud.png items.png sprite_particles.png sprite_particles_1.png

SOTN_DIR     := sotn-decomp
SOTN_SDK	 := $(SOTN_DIR)/LICENSE
CONFIG_DIR   := $(SOTN_DIR)/config
INCLUDE_DIR  := $(SOTN_DIR)/include

PYTHON    := .venv/bin/python3
CROSS     := mipsel-linux-gnu-
CC        := $(CROSS)gcc
LD        := $(CROSS)ld
CPP       := $(CROSS)cpp
OBJCOPY   := $(CROSS)objcopy
CC_FLAGS  := -std=c11 -c -isystem $(INCLUDE_DIR) -G0 -O2 -g -funsigned-char -ffunction-sections -fdata-sections
CC_FLAGS  += -march=mips1 -mabi=32 -EL -mfp32 -fno-pic -mno-shared -mno-abicalls -fno-stack-protector -nostdlib
CC_FLAGS  += -Werror -Wall -Wno-unused-function -Wno-switch
CC_FLAGS  += -D_internal_version_us -DVERSION_PSX -DNO_LOGS
OBJS      := $(addprefix build/, $(addsuffix .o, $(FILES)))
SRC_FILES := $(addsuffix .c, $(FILES))
ASSETS_H  := $(addprefix src/assets/, $(addsuffix .inc, $(ASSETS)))

all: build
spritesheet: sprites/config.json
	$(PYTHON) tools/spritesheet.py merge sprites/config.json assets/spritesheet.png
clean:
	git clean -xfd build/
	git clean -xf assets/*.c.inc
	git clean -xf src/pl_sprites.c
	git clean -xf src/pl_sprite_particles.c
build: build/$(PL_NAME).bin

build/%.bin: build/%.elf
	$(OBJCOPY) -O binary $< $@
build/$(PL_NAME).elf: $(OBJS)
	grep -o '^[^/]*' $(CONFIG_DIR)/symbols.$(VERSION).txt > build/symbols.$(VERSION).txt
	$(LD) -o $@ --gc-sections -Map build/$(PL_NAME).map -T pl.ld \
		-T $(CONFIG_DIR)/undefined_syms.$(VERSION).txt \
		-T build/symbols.$(VERSION).txt \$^
build/%.o: src/%.c $(SOTN_SDK)
	mkdir -p $(dir $@)
	$(CC) $(CC_FLAGS) -o $@ $<
src/pl_assets.c: src/assets/pal.inc $(ASSETS_H)
src/pl_sprites.c: sprites/config.json
	$(PYTHON) tools/sprite_to_c.py encode sprites/config.json src/pl_sprites.c
sprites/config.json: assets/spritesheet.png
	$(PYTHON) tools/spritesheet.py split $< sprites/
src/pl_sprite_particles.c: assets/sprite_particles.yaml
	$(PYTHON) tools/sprite_packer.py $< $@
assets/sprite_particles.png assets/sprite_particles_1.png: src/pl_sprite_particles.c
src/assets/%.png.inc: assets/%.png build/lz4cmp
	cat $< | $(PYTHON) tools/png2raw.py | build/lz4cmp | xxd -i - $@
src/assets/pal.inc: assets/pal.yaml
	cat $< | $(PYTHON) tools/jascpal2c.py > $@
build/lz4cmp: tools/lz4cmp.c
	mkdir -p $(dir $@)
	gcc $^ -llz4 -O2 -o $@
$(SOTN_SDK):
	git submodule init $(SOTN_DIR)
	git submodule update $(SOTN_DIR)

PCSX_REDUX_PATH = $$(cat path)
run: build
	cd sotn-decomp && go run ./tools/sotn-disk inject ../.emu/sotn-mmx.us.bin BIN/RIC.BIN ../build/mmx.bin
	cd .emu && $(PCSX_REDUX_PATH) -run -iso sotn-mmx.us.bin

.PHONY: all build clean run spritesheet
