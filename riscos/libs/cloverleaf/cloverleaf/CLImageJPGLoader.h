//
// Created by lenz on 3/23/20.
//

#ifndef ROCHAT_CLIMAGEJPGLOADER_H
#define ROCHAT_CLIMAGEJPGLOADER_H

#include <string>
#include <cloverleaf/CLImageLoader.h>

class CLImageJPGLoader : public CLImageLoader {
public:
    static osspriteop_area* load(const std::string& filename, int* width_ptr, int* height_ptr);
    static bool save(struct rosprite* sprite, const std::string &filename, int quality);
};


#endif //ROCHAT_CLIMAGEJPGLOADER_H
