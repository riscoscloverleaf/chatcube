//
// Created by lenz on 2/3/20.
//

#ifndef ROCHAT_AVATARDATA_H
#define ROCHAT_AVATARDATA_H

#include "JsonData.h"

class AvatarData : public JsonData {
public:
    std::string pic_small;
    std::string name;

    AvatarData() {};
    AvatarData(const cJSON *jsonobj) {
        update_from_json(jsonobj);
    };

    void update_from_json(const cJSON *json);

    bool operator==(const AvatarData &other) const {return (name==other.name);}
    bool operator!=(const AvatarData &other) const {return (name!=other.name);}
};


#endif //ROCHAT_AVATARDATA_H
