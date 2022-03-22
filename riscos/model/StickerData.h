//
// Created by lenz on 2/3/20.
//

#ifndef ROCHAT_STICKERDATA_H
#define ROCHAT_STICKERDATA_H

#include <vector>
#include "JsonData.h"

#define msg_STICKER_THUMB_WIDTH  150 // in pixels
#define msg_STICKER_THUMB_HEIGHT 150 // in pixels

class StickerData : public JsonData {
public:
    std::string pic;
    std::string pic_small;
    std::string name;

    StickerData() {};
    StickerData(const cJSON *jsonobj) {
        update_from_json(jsonobj);
    };

    void update_from_json(const cJSON *jsonobj);

    bool operator==(const StickerData &other) const {return (name==other.name);}
    bool operator!=(const StickerData &other) const {return (name!=other.name);}
};

class StickerGroupData : public JsonData {
public:
    std::string name;
    std::string pic_small;
    std::vector<StickerData> items;

    StickerGroupData() {};
    StickerGroupData(const cJSON *jsonobj) {
        update_from_json(jsonobj);
    };

    void update_from_json(const cJSON *jsonobj);

    bool operator==(const StickerGroupData &other) const {return (name==other.name);}
    bool operator!=(const StickerGroupData &other) const {return (name!=other.name);}
};


#endif //ROCHAT_STICKERDATA_H
