//
// Created by lenz on 3/23/20.
//

#include <stdlib.h>
#include <cstring>
#include "tinct.h"
#include "CLImageLoader.h"

osspriteop_area* CLImageLoader::create_sprite_area(int width_px, int height_px) {
    int area_size = 16 + 44 + width_px * height_px * 4;
    osspriteop_area* sprite_area;
    sprite_area = (osspriteop_area*)malloc(area_size);

    if (!sprite_area)
        return nullptr;

    osspriteop_header *sprite;

    /* area control block */
    sprite_area->size = area_size;
    sprite_area->sprite_count = 1;
    sprite_area->first = 16;
    sprite_area->used = area_size;

    /* sprite control block */
    sprite = (osspriteop_header *) (sprite_area + 1);
    sprite->size = area_size - 16;
    memset(sprite->name, 0x00, 12);
    strncpy(sprite->name, "bitmap", 12);
    sprite->width = width_px - 1;
    sprite->height = height_px - 1;
    sprite->left_bit = 0;
    sprite->right_bit = 31;
    sprite->image = sprite->mask = 44;
    sprite->mode = tinct_SPRITE_MODE;

    return sprite_area;
}

unsigned char * CLImageLoader::sprite_buffer(osspriteop_area* sprite_area)
{
    /* image data area should exist */
    if (sprite_area)
        return ((unsigned char *) (sprite_area)) + 16 + 44;

    return NULL;
}
