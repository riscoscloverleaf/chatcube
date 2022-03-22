//
// Created by lenz on 3/22/20.
//

#ifndef ROCHAT_CLIMAGEPNGLOADER_H
#define ROCHAT_CLIMAGEPNGLOADER_H

#include <string>
#include "CLImageLoader.h"

class CLImagePNGLoader : public CLImageLoader {
public:
    static osspriteop_area* load(const std::string& filename, int* width_ptr, int* height_ptr);
};


#endif //ROCHAT_CLIMAGEPNGLOADER_H
