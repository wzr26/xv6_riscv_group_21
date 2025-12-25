// user/sprite.h
// Sprite format and structures for animation

#ifndef SPRITE_H
#define SPRITE_H

#include "types.h"

// Sprite header structure
typedef struct sprite_header {
    uint16 width;
    uint16 height;
    uint32 color_key;    // transparent color
    uint32 flags;        // format flags (0 = raw pixels, 1 = RLE compressed)
} sprite_header_t;

// Sprite metadata for animation
typedef struct sprite {
    sprite_header_t header;
    uint32 *pixels;      // pixel data
    int size;            // size in bytes
} sprite_t;

// Sprite rendering context
typedef struct sprite_context {
    sprite_t sprite;
    int x, y;            // position
    int vx, vy;          // velocity
    int active;          // 0 = inactive, 1 = active
} sprite_context_t;

// Load sprite from file
sprite_t *sprite_load(const char *filename);

// Free sprite memory
void sprite_free(sprite_t *spr);

// Create sprite from raw pixel array
sprite_t *sprite_create(int width, int height, uint32 *pixels, uint32 color_key);

// Draw sprite at position
void sprite_draw(sprite_t *spr, int x, int y);

// Draw sprite with color key transparency
void sprite_draw_transparent(sprite_t *spr, int x, int y, uint32 color_key);

#endif // SPRITE_H
