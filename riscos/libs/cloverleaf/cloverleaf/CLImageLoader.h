//
// Created by lenz on 3/23/20.
//

#ifndef ROCHAT_CLIMAGELOADER_H
#define ROCHAT_CLIMAGELOADER_H

#include <oslib/osspriteop.h>

class CLImageLoader {
public:
    static osspriteop_area* create_sprite_area(int width_px, int height_px);
    static unsigned char * sprite_buffer(osspriteop_area* sprite_area);
};


#endif //ROCHAT_CLIMAGELOADER_H
