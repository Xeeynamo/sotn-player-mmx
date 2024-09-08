VERSION	  ?= us
PL_NAME   := mmx
FILES     := pl_header pl_sprites pl pl_assets pl_anims pl_settings
FILES     += pl_hud pl_entities
FILES     += lz4/lz4
FILES     += d_18568 spriteparts
FILES     += 1AC60 21250 24788 26C84 2A060 2C4C4 319C4 bss
ASSETS	  += hud.png items.png particles.png

SOTN_DIR     := ../sotn-decomp
CONFIG_DIR   := $(SOTN_DIR)/config
INCLUDE_DIR  := $(SOTN_DIR)/include

CROSS     := mipsel-linux-gnu-
CC        := $(CROSS)gcc
LD        := $(CROSS)ld
CPP       := $(CROSS)cpp
OBJCOPY   := $(CROSS)objcopy
CC_FLAGS  := -c -I$(INCLUDE_DIR) -G0 -O2 -g -funsigned-char -w
CC_FLAGS  += -DVERSION_PSX -DVERSION_US
CC_FLAGS  += -march=mips1 -mabi=32 -EL -fno-pic -mno-shared -mno-abicalls -mfp32 -mno-llsc
CC_FLAGS  += -fno-stack-protector -nostdlib
OBJS      := $(addprefix build/, $(addsuffix .o, $(FILES)))
SRC_FILES := $(addsuffix .c, $(FILES))
ASSETS_H  := $(addprefix src/assets/, $(addsuffix .inc, $(ASSETS)))

all: build
spritesheet: sprites/config.json
	python3 tools/spritesheet.py merge sprites/config.json assets/spritesheet.png
clean:
	git clean -xfd build/
	git clean -xf assets/*.c.inc
	git clean -xf src/pl_sprites.c
build: build/$(PL_NAME).bin

build/%.bin: build/%.elf
	$(OBJCOPY) -O binary $< $@
build/$(PL_NAME).elf: $(OBJS)
	$(LD) -o $@ -Map build/$(PL_NAME).map -T pl.ld \
		-T $(CONFIG_DIR)/undefined_syms.$(VERSION).txt \
		-T $(CONFIG_DIR)/undefined_syms_auto.$(VERSION).dra.txt \
		-T $(CONFIG_DIR)/symbols.$(VERSION).txt \$^
build/%.o: src/%.c
	mkdir -p $(dir $@)
	$(CC) $(CC_FLAGS) -o $@ $<
src/pl_assets.c: src/assets/pal.inc $(ASSETS_H)
src/pl_sprites.c: sprites/config.json
	python3 tools/sprite_to_c.py encode sprites/config.json src/pl_sprites.c
sprites/config.json: assets/spritesheet.png
	python3 tools/spritesheet.py split $< sprites/
src/assets/%.png.inc: assets/%.png build/lz4cmp
	cat $< | python3 tools/png2raw.py | build/lz4cmp | xxd -i - $@
src/assets/pal.inc: assets/pal.yaml
	cat $< | python3 tools/palette.py > $@
build/lz4cmp: tools/lz4cmp.c
	mkdir -p $(dir $@)
	gcc -o $@ -llz4 -O2 $^

.PHONY: all build clean spritesheet
