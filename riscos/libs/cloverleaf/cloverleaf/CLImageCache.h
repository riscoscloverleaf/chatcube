//
// Created by lenz on 3/23/20.
//

#ifndef ROCHAT_CLIMAGECACHE_H
#define ROCHAT_CLIMAGECACHE_H

#include <string>
#include <map>
#include "CLImage.h"

void init_images_cache(int capacity); // capacity is number if images stored
CLImage* load_cached_image(const std::string& filename);

#endif //ROCHAT_CLIMAGECACHE_H
